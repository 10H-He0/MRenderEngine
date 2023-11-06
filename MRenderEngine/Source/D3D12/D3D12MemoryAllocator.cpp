#include "D3D12MemoryAllocator.h"

using namespace DirectX;
using namespace Microsoft::WRL;

MD3D12BuddyAllocator::MD3D12BuddyAllocator(ID3D12Device* IDevice, const MAllocatorInitData& InitData)
	:D3DDevice(IDevice), InitData(InitData)
{
	Initialize();
}

MD3D12BuddyAllocator::~MD3D12BuddyAllocator()
{
	if (BackingHeap)
	{
		BackingHeap->Release();
	}
	if (BackingResource)
	{
		delete BackingResource;
	}
}

void MD3D12BuddyAllocator::Initialize()
{
	if (InitData.AllocationStrategy == EAllocationStrategy::PlacedResource)
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
		D3D12_HEAP_DESC Desc = {};
		Desc.SizeInBytes = DEFAULT_POOL_SIZE;
		Desc.Properties = HeapProperties;
		Desc.Alignment = 0;
		Desc.Flags = InitData.HeapFlags;

		ID3D12Heap* Heap = nullptr;
		ThrowIfFailed(D3DDevice->CreateHeap(&Desc, IID_PPV_ARGS(&Heap)));
		Heap->SetName(L"MD3D12BuddyAllocator BcackingHeap");

		BackingHeap = Heap;
	}
	else
	{
		CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
		D3D12_RESOURCE_STATES HeapResourceState;
		if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
		{
			HeapResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		else
		{
			HeapResourceState = D3D12_RESOURCE_STATE_COMMON;
		}

		CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DEFAULT_POOL_SIZE, InitData.ResourceFlags);
		ComPtr<ID3D12Resource> Resource;
		ThrowIfFailed(D3DDevice->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&BufferDesc,
			HeapResourceState,
			nullptr,
			IID_PPV_ARGS(&Resource)));
		Resource->SetName(L"MD3D12BuddyAllocator BackingResource");

		BackingResource = new MD3D12Resource(Resource);

		if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
		{
			BackingResource->Map();
		}
	}

	MaxOrder = UnitSizeToOrder(SizeToUnitSize(DEFAULT_POOL_SIZE));

	for (uint32_t i = 0; i <= MaxOrder; i++)
	{
		FreeBlocks.emplace_back(std::set<uint32_t>());
	}

	FreeBlocks[MaxOrder].insert((uint32_t)0);
}

bool MD3D12BuddyAllocator::AllocResource(uint32_t Size, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation)
{
	uint32_t SizeToAllocate = GetSizeToAllocate(Size, Alignment);

	if (CanAllocate(SizeToAllocate))
	{
		const uint32_t UnitSize = SizeToUnitSize(SizeToAllocate);
		const uint32_t Order = UnitSizeToOrder(UnitSize);
		const uint32_t Offset = AllocateBlock(Order);
		const uint32_t AllocSize = UnitSize * MinBlockSize;
		TotalAllocaSize += AllocSize;

		const uint32_t OffsetFromBaseOfResource = GetAllocOffsetInBytes(Offset);
		uint32_t AlignedOffsetFromBaseOfResource = OffsetFromBaseOfResource;
		if (Alignment != 0 && AlignedOffsetFromBaseOfResource % Alignment != 0)
		{
			AlignedOffsetFromBaseOfResource = AlignArbitrary(AlignedOffsetFromBaseOfResource, Alignment);

			uint32_t Padding = AlignedOffsetFromBaseOfResource - OffsetFromBaseOfResource;
			assert((Padding + Size) <= AllocSize);
		}
		assert((AlignedOffsetFromBaseOfResource % Alignment) == 0);

		ResourceLocation.SetType(MD3D12ResourceLocation::EResourceLocationType::SubAllocation);
		ResourceLocation.BlockData.Order = Order;
		ResourceLocation.BlockData.Offset = Offset;
		ResourceLocation.BlockData.ActualUsedSize = Size;
		ResourceLocation.Allocator = this;

		if (InitData.AllocationStrategy == EAllocationStrategy::ManualSubAllocation)
		{
			ResourceLocation.UnderlyingResource = BackingResource;
			ResourceLocation.OffsetFromBaseOfResource = AlignedOffsetFromBaseOfResource;
			ResourceLocation.GPUVirtualAddress = BackingResource->GPUVirtualAddress + AlignedOffsetFromBaseOfResource;

			if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
			{
				ResourceLocation.MappedAddress = (uint8_t*)BackingResource->MappedBaseAddress + AlignedOffsetFromBaseOfResource;
			}
		}
		else
		{
			ResourceLocation.OffsetFromBaseOfHeap = AlignedOffsetFromBaseOfResource;
		}
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t MD3D12BuddyAllocator::GetSizeToAllocate(uint32_t Size, uint32_t Alignment)
{
	uint32_t SizeToAllocate = Size;

	if (Alignment != 0 && MinBlockSize % Alignment != 0)
	{
		SizeToAllocate = Size + Alignment;
	}

	return SizeToAllocate;
}

bool MD3D12BuddyAllocator::CanAllocate(uint32_t SizeToAllocate)
{
	if (TotalAllocaSize == DEFAULT_POOL_SIZE)
	{
		return false;
	}

	uint32_t BlockSize = DEFAULT_POOL_SIZE;
	for (int i = (int)FreeBlocks.size() - 1; i >= 0; i--)
	{
		if (FreeBlocks[i].size() && BlockSize >= SizeToAllocate)
		{
			return true;
		}

		BlockSize = BlockSize >> 1;

		if (BlockSize < SizeToAllocate) return false;
	}

	return false;
}

uint32_t MD3D12BuddyAllocator::AllocateBlock(uint32_t  Order)
{
	uint32_t Offset;

	if (Order > MaxOrder)
	{
		assert(false);
	}

	if (FreeBlocks[Order].size() == 0)
	{
		uint32_t Left = AllocateBlock(Order + 1);
		uint32_t UnitSize = OrderToUnitSize(Order);
		uint32_t Right = Left + UnitSize;
		FreeBlocks[Order].insert(Right);
		Offset = Left;
	}
	else
	{
		auto It = FreeBlocks[Order].cbegin();
		Offset = *It;

		FreeBlocks[Order].erase(*It);
	}
	return Offset;
}

void MD3D12BuddyAllocator::DeAllocate(MD3D12ResourceLocation& ResourceLocation)
{
	DeletionQueue.push_back(ResourceLocation.BlockData);
}

void MD3D12BuddyAllocator::CleanUpAllocations()
{
	for (int i = 0; i < DeletionQueue.size(); i++)
	{
		const MD3D12BlockData& Block = DeletionQueue[i];

		DeAllocateInternal(Block);
	}

	DeletionQueue.clear();
}

void MD3D12BuddyAllocator::DeAllocateInternal(const MD3D12BlockData& Block)
{
	DeAllocateBlock(Block.Offset, Block.Order);

	uint32_t Size = OrderToUnitSize(Block.Order) * MinBlockSize;
	TotalAllocaSize -= Size;

	if (InitData.AllocationStrategy == EAllocationStrategy::PlacedResource)
	{
		assert(Block.PlacedResource != nullptr);

		delete Block.PlacedResource;
	}
}

void MD3D12BuddyAllocator::DeAllocateBlock(uint32_t Offset, uint32_t Order)
{
	uint32_t Size = OrderToUnitSize(Order);
	uint32_t Buddy = GetBuddyOffset(Offset, Size);

	auto It = FreeBlocks[Order].find(Buddy);
	if (It != FreeBlocks[Order].end())
	{
		DeAllocateBlock(min(Offset, Buddy), Order + 1);
		FreeBlocks[Order].erase(*It);
	}
	else
	{
		FreeBlocks[Order].insert(Offset);
	}
}

MD3D12MultiBuddyAllocator::MD3D12MultiBuddyAllocator(ID3D12Device* IDevice, const MD3D12BuddyAllocator::MAllocatorInitData& InitData)
	:Device(IDevice), InitData(InitData)
{

}

MD3D12MultiBuddyAllocator::~MD3D12MultiBuddyAllocator()
{

}

bool MD3D12MultiBuddyAllocator::AllocateResource(uint32_t Size, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation)
{
	for (auto& Allocator : Allocators)
	{
		if (Allocator->AllocResource(Size, Alignment, ResourceLocation))
		{
			return true;
		}
	}

	auto Allocator = std::make_shared<MD3D12BuddyAllocator>(Device, InitData);
	Allocators.push_back(Allocator);

	bool Result = Allocator->AllocResource(Size, Alignment, ResourceLocation);
	assert(Result);

	return true;
}

void MD3D12MultiBuddyAllocator::CleanUpAllocations()
{
	for (auto& Allocator : Allocators)
	{
		Allocator->CleanUpAllocations();
	}
}

MD3D12DefaultBufferAllocator::MD3D12DefaultBufferAllocator(ID3D12Device* IDevice)
{
	MD3D12BuddyAllocator::MAllocatorInitData DefaultInitData;
	DefaultInitData.AllocationStrategy = MD3D12BuddyAllocator::EAllocationStrategy::ManualSubAllocation;
	DefaultInitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	DefaultInitData.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;
	Allocator = std::make_unique<MD3D12MultiBuddyAllocator>(IDevice, DefaultInitData);

	MD3D12BuddyAllocator::MAllocatorInitData UavInitData;
	UavInitData.AllocationStrategy = MD3D12BuddyAllocator::EAllocationStrategy::ManualSubAllocation;
	UavInitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	UavInitData.ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	UavAllocator = std::make_unique<MD3D12MultiBuddyAllocator>(IDevice, UavInitData);

	Device = IDevice;
}

void MD3D12DefaultBufferAllocator::AllocateDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation)
{
	if (ResourceDesc.Flags == D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
	{
		UavAllocator->AllocateResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
	}
	else
	{
		Allocator->AllocateResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
	}
}

void MD3D12DefaultBufferAllocator::CleanUpAllocations()
{
	Allocator->CleanUpAllocations();
	UavAllocator->CleanUpAllocations();
}

MD3D12UploadBufferAllocator::MD3D12UploadBufferAllocator(ID3D12Device* IDevice)
{
	MD3D12BuddyAllocator::MAllocatorInitData InitData;
	InitData.AllocationStrategy = MD3D12BuddyAllocator::EAllocationStrategy::ManualSubAllocation;
	InitData.HeapType = D3D12_HEAP_TYPE_UPLOAD;
	InitData.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;
	Allocator = std::make_unique<MD3D12MultiBuddyAllocator>(IDevice, InitData);

	Device = IDevice;
}

void* MD3D12UploadBufferAllocator::AllocateUploadResource(uint32_t Size, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation)
{
	Allocator->AllocateResource(Size, Alignment, ResourceLocation);

	return ResourceLocation.MappedAddress;
}

void MD3D12UploadBufferAllocator::CleanUpAllocations()
{
	Allocator->CleanUpAllocations();
}

MD3D12TextureResourceAllocator::MD3D12TextureResourceAllocator(ID3D12Device* IDevice)
{
	MD3D12BuddyAllocator::MAllocatorInitData InitData;
	InitData.AllocationStrategy = MD3D12BuddyAllocator::EAllocationStrategy::PlacedResource;
	InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	InitData.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
	Allocator = std::make_unique<MD3D12MultiBuddyAllocator>(IDevice, InitData);

	Device = IDevice;
}

void MD3D12TextureResourceAllocator::AllocateTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, MD3D12ResourceLocation& ResourceLocation)
{
	const D3D12_RESOURCE_ALLOCATION_INFO Info = Device->GetResourceAllocationInfo(0, 1, &ResourceDesc);
	Allocator->AllocateResource((uint32_t)Info.SizeInBytes, DEFAULT_RESOURCE_ALIGNMENT, ResourceLocation);

	ComPtr<ID3D12Resource> Resource;
	ID3D12Heap* BackingHeap = ResourceLocation.Allocator->GetBackingHeap();
	uint64_t HeapOffset = ResourceLocation.OffsetFromBaseOfHeap;
	Device->CreatePlacedResource(BackingHeap, HeapOffset, &ResourceDesc, ResourceState, nullptr, IID_PPV_ARGS(&Resource));

	MD3D12Resource* NewResource = new MD3D12Resource(Resource);
	ResourceLocation.UnderlyingResource = NewResource;
	ResourceLocation.BlockData.PlacedResource = NewResource;
}

void MD3D12TextureResourceAllocator::CleanUpAllocations()
{
	Allocator->CleanUpAllocations();
}
#include "D3D12Buffer.h"
#include "D3D12RHI.h"

MD3D12ConstantBufferRef MD3D12RHI::CreateConstantBuffer(const void* Content, uint32_t Size)
{
	MD3D12ConstantBufferRef ConstantBufferRef = std::make_shared<MD3D12ConstantBuffer>();

	auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
	void* MappedData = UploadBufferAllocator->AllocateUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, ConstantBufferRef->ResourceLocation);

	memcpy(MappedData, Content, Size);

	return ConstantBufferRef;
}

MD3D12StructuredBufferRef MD3D12RHI::CreateStructuredBuffer(const void* Content, uint32_t ElementSize, uint32_t ElementCount)
{
	assert(Content != nullptr && ElementSize > 0 && ElementCount > 0);

	MD3D12StructuredBufferRef StructuredBufferRef = std::make_shared<MD3D12StructuredBuffer>();

	auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
	uint32_t DataSize = ElementSize * ElementCount;
	void* MappedData = UploadBufferAllocator->AllocateUploadResource(DataSize, ElementSize, StructuredBufferRef->ResourceLocation);

	memcpy(MappedData, Content, DataSize);

	{
		MD3D12ResourceLocation& Location = StructuredBufferRef->ResourceLocation;
		const uint64_t Offset = Location.OffsetFromBaseOfResource;
		ID3D12Resource* BufferResource = Location.UnderlyingResource->D3DResource.Get();

		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
		SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		SrvDesc.Buffer.StructureByteStride = ElementSize;
		SrvDesc.Buffer.NumElements = ElementCount;
		SrvDesc.Buffer.FirstElement = Offset / ElementSize;

		StructuredBufferRef->SetSRV(std::make_unique<MD3D12ShaderResourceView>(GetDevice(), SrvDesc, BufferResource));
	}

	return StructuredBufferRef;
}

MD3D12RWStructuredBufferRef MD3D12RHI::CreateRWStructuredBuffer(uint32_t ElementSize, uint32_t ElementCount)
{
	MD3D12RWStructuredBufferRef RWStructuredBufferRef = std::make_shared<MD3D12RWStructuredBuffer>();

	uint32_t DataSize = ElementSize * ElementCount;
	CreateDefaultBuffer(DataSize, ElementSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, RWStructuredBufferRef->ResourceLocation);

	MD3D12ResourceLocation& Location = RWStructuredBufferRef->ResourceLocation;
	const uint64_t Offset = Location.OffsetFromBaseOfResource;
	ID3D12Resource* BufferResource = Location.UnderlyingResource->D3DResource.Get();

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
		SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		SrvDesc.Buffer.StructureByteStride = ElementSize;
		SrvDesc.Buffer.NumElements = ElementCount;
		SrvDesc.Buffer.FirstElement = Offset / ElementSize;

		RWStructuredBufferRef->SetSRV(std::make_unique<MD3D12ShaderResourceView>(GetDevice(), SrvDesc, BufferResource));
	}

	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};
		UavDesc.Format = DXGI_FORMAT_UNKNOWN;
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		UavDesc.Buffer.StructureByteStride = ElementSize;
		UavDesc.Buffer.NumElements = ElementCount;
		UavDesc.Buffer.FirstElement = Offset / ElementSize;
		UavDesc.Buffer.CounterOffsetInBytes = 0;

		RWStructuredBufferRef->SetUAV(std::make_unique<MD3D12UnorderedAccessView>(GetDevice(), UavDesc, BufferResource));
	}

	return RWStructuredBufferRef;
}

MD3D12VertexBufferRef MD3D12RHI::CreateVertexBuffer(const void* Content, uint32_t Size)
{
	MD3D12VertexBufferRef VertexBufferRef = std::make_shared<MD3D12VertexBuffer>();

	CreateAndInitDefaultBuffer(Content, Size, DEFAULT_RESOURCE_ALIGNMENT, VertexBufferRef->ResourceLocation);

	return VertexBufferRef;
}

MD3D12IndexBufferRef MD3D12RHI::CreateIndexBuffer(const void* Content, uint32_t Size)
{
	MD3D12IndexBufferRef IndexBufferRef = std::make_shared<MD3D12IndexBuffer>();

	CreateAndInitDefaultBuffer(Content, Size, DEFAULT_RESOURCE_ALIGNMENT, IndexBufferRef->ResourceLocation);

	return IndexBufferRef;
}

MD3D12ReadBackBufferRef MD3D12RHI::CreateReadBackBuffer(uint32_t Size)
{
	MD3D12ReadBackBufferRef ReadBackBufferRef = std::make_shared<MD3D12ReadBackBuffer>();

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource;

	ThrowIfFailed(Device->GetD3DDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&Resource)
	));

	MD3D12Resource* NewResource = new MD3D12Resource(Resource, D3D12_RESOURCE_STATE_COPY_DEST);
	ReadBackBufferRef->ResourceLocation.UnderlyingResource = NewResource;
	ReadBackBufferRef->ResourceLocation.SetType(MD3D12ResourceLocation::EResourceLocationType::StandAlone);

	return ReadBackBufferRef;
}

void MD3D12RHI::CreateDefaultBuffer(uint32_t Size, uint32_t Alignment, D3D12_RESOURCE_FLAGS Flag, MD3D12ResourceLocation& ResourceLocation)
{
	D3D12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(Size, Flag);
	auto DefaultBufferAllocator = GetDevice()->GetDefaultBufferAllocator();
	DefaultBufferAllocator->AllocateDefaultResource(ResourceDesc, Alignment, ResourceLocation);
}

void MD3D12RHI::CreateAndInitDefaultBuffer(const void* Content, uint32_t Size, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation)
{
	CreateDefaultBuffer(Size, Alignment, D3D12_RESOURCE_FLAG_NONE, ResourceLocation);

	MD3D12ResourceLocation UploadResourceLocation;
	auto UploadBufferAllocator = GetDevice()->GetUploadBufferAllocator();
	void* MappedData = UploadBufferAllocator->AllocateUploadResource(Size, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);

	memcpy(MappedData, Content, Size);

	MD3D12Resource* DefaultBuffer = ResourceLocation.UnderlyingResource;
	MD3D12Resource* UploadBuffer = UploadResourceLocation.UnderlyingResource;

	TransitionResource(DefaultBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
	CopyBufferRegion(DefaultBuffer, ResourceLocation.OffsetFromBaseOfResource, UploadBuffer, UploadResourceLocation.OffsetFromBaseOfResource, Size);
}
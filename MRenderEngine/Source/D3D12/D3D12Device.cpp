#include "D3D12Device.h"
#include "D3D12RHI.h"

MD3D12Device::MD3D12Device(MD3D12RHI* ID3D12RHI)
	:D3D12RHI(ID3D12RHI)
{
	Initialize();
}

MD3D12Device::~MD3D12Device()
{

}

void MD3D12Device::Initialize()
{
	HRESULT HardWareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice));

	if (FAILED(HardWareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> WarpAdapter;

		ThrowIfFailed(D3D12RHI->GetDXGIFactory()->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(WarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));
	}

	CommandContext = std::make_unique<MD3D12CommandContext>(this);

	DefaultBufferAllocator = std::make_unique<MD3D12DefaultBufferAllocator>(D3DDevice.Get());

	UploadBufferAllocator = std::make_unique<MD3D12UploadBufferAllocator>(D3DDevice.Get());

	TextureResourceAllocator = std::make_unique<MD3D12TextureResourceAllocator>(D3DDevice.Get());

	RTVHeapSlotAllocator = std::make_unique<MD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 200);

	DSVHeapSlotAllocator = std::make_unique<MD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 200);

	SRVHeapSlotAllocator = std::make_unique<MD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 200);
}

MD3D12HeapSlotAllocator* MD3D12Device::GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
	switch (HeapType)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		return SRVHeapSlotAllocator.get();
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		return RTVHeapSlotAllocator.get();
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		return DSVHeapSlotAllocator.get();
		break;
	default:
		return nullptr;
		break;
	}
}
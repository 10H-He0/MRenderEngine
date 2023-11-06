#include "D3D12RHI.h"
#include <assert.h>

#define InstalledDebugLayers true

using Microsoft::WRL::ComPtr;

MD3D12RHI::MD3D12RHI()
{

}

MD3D12RHI::~MD3D12RHI()
{
	Destory();
}

void MD3D12RHI::Initialize(HWND WindowHandle, int WindowWidth, int WindowHeight)
{
	UINT DXGIFactoryFlag = 0;

#if (defined(DEBUG) || defined(_DEBUG)) && InstalledDebugLayers 
	{
		ComPtr<ID3D12Debug> DebugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(DebugController.GetAddressOf())));
		DebugController->EnableDebugLayer();
	}

	ComPtr<IDXGIInfoQueue> DxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(DxgiInfoQueue.GetAddressOf()))))
	{
		DXGIFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;

		DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlag, IID_PPV_ARGS(DXGIFactory.GetAddressOf())));

	Device = std::make_unique<MD3D12Device>(this);

	ViewportInfo.WindowHandle = WindowHandle;
	ViewportInfo.BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	ViewportInfo.DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ViewportInfo.bEnable4xMSAA = false;
	ViewportInfo.QualityOf4xMSAA = GetSupportMSAAQuality(ViewportInfo.BackBufferFormat);

	Viewport = std::make_unique<MD3D12Viewport>(this, ViewportInfo, WindowWidth, WindowHeight);

#ifdef _DEBUG
	LogAdapters();
#endif
}

void MD3D12RHI::Destory()
{
	EndFrame();

	Viewport.reset();

	Device.reset();
}

const MD3D12ViewportInfo& MD3D12RHI::GetViewportInfo()
{
	return ViewportInfo;
}

IDXGIFactory4* MD3D12RHI::GetDXGIFactory()
{
	return DXGIFactory.Get();
}

void MD3D12RHI::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (DXGIFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void MD3D12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, ViewportInfo.BackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void MD3D12RHI::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}

UINT MD3D12RHI::GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat)
{
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(Device->GetD3DDevice()->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	UINT QualityOf4xMsaa = msQualityLevels.NumQualityLevels;
	assert(QualityOf4xMsaa > 0 && "Unexpected MSAA quality level.");

	return QualityOf4xMsaa;
}

void MD3D12RHI::FlushCommandQueue()
{
	GetDevice()->GetCommandContext()->FlushCommandQueue();
}

void MD3D12RHI::ExcuteCommandList()
{
	GetDevice()->GetCommandContext()->ExcuteCommandList();
}

void MD3D12RHI::ResetCommandList()
{
	GetDevice()->GetCommandContext()->ResetCommandList();
}

void MD3D12RHI::ResetCommandAllocator()
{
	GetDevice()->GetCommandContext()->ResetCommandAllocator();
}

void MD3D12RHI::Present()
{
	GetViewport()->Present();
}

void MD3D12RHI::ResizeViewport(int NewWidth, int NewHeight)
{
	GetViewport()->OnResize(NewWidth, NewHeight);
}

void MD3D12RHI::TransitionResource(MD3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_STATES StateBefore = Resource->CurrentState;

	if (StateBefore != StateAfter)
	{
		GetDevice()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->D3DResource.Get(), StateBefore, StateAfter));
		Resource->CurrentState = StateAfter;
	}
}

void MD3D12RHI::CopyResource(MD3D12Resource* DstResource, MD3D12Resource* SrcResource)
{
	GetDevice()->GetCommandList()->CopyResource(DstResource->D3DResource.Get(), SrcResource->D3DResource.Get());
}

void MD3D12RHI::CopyBufferRegion(MD3D12Resource* DstResource, UINT64 DstOffset, MD3D12Resource* SrcResource, UINT64 SrcOffset, UINT64 Size)
{
	GetDevice()->GetCommandList()->CopyBufferRegion(DstResource->D3DResource.Get(), DstOffset, SrcResource->D3DResource.Get(), SrcOffset, Size);
}

void MD3D12RHI::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox)
{
	GetDevice()->GetCommandList()->CopyTextureRegion(Dst, DstX, DstY, DstZ, Src, SrcBox);
}

void MD3D12RHI::SetVertexBuffer(const MD3D12VertexBufferRef& VertexBuffer, UINT Offset, UINT Stride, UINT Size)
{
	const MD3D12ResourceLocation& ResourceLocation = VertexBuffer->ResourceLocation;
	MD3D12Resource* Resource = ResourceLocation.UnderlyingResource;
	TransitionResource(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

	D3D12_VERTEX_BUFFER_VIEW VBV;
	VBV.BufferLocation = ResourceLocation.GPUVirtualAddress + Offset;
	VBV.StrideInBytes = Stride;
	VBV.SizeInBytes = Size;
	GetDevice()->GetCommandList()->IASetVertexBuffers(0, 1, &VBV);
}

void MD3D12RHI::SetIndexBuffer(const MD3D12IndexBufferRef& IndexBuffer, UINT Offset, DXGI_FORMAT Format, UINT Size)
{
	const MD3D12ResourceLocation& ResourceLocation = IndexBuffer->ResourceLocation;
	MD3D12Resource* Resource = ResourceLocation.UnderlyingResource;
	TransitionResource(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

	D3D12_INDEX_BUFFER_VIEW IBV;
	IBV.BufferLocation = Resource->GPUVirtualAddress + Offset;
	IBV.Format = Format;
	IBV.SizeInBytes = Size;
	GetDevice()->GetCommandList()->IASetIndexBuffer(&IBV);
}

void MD3D12RHI::EndFrame()
{
	GetDevice()->GetDefaultBufferAllocator()->CleanUpAllocations();

	GetDevice()->GetUploadBufferAllocator()->CleanUpAllocations();

	GetDevice()->GetTextureResourceAllocator()->CleanUpAllocations();

	GetDevice()->GetCommandContext()->EndFrame();
}
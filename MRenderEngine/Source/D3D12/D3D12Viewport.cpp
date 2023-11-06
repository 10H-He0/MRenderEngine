#include "D3D12Viewport.h"
#include "D3D12RHI.h"

MD3D12Viewport::MD3D12Viewport(MD3D12RHI* ID3D12RHI, const MD3D12ViewportInfo& Info, int Width, int Height)
	:D3D12RHI(ID3D12RHI), ViewportInfo(Info), ViewportWidth(Width), ViewportHeight(Height)
{
	Initialize();
}

MD3D12Viewport::~MD3D12Viewport()
{

}

void MD3D12Viewport::Initialize()
{
	CreateSwapChain();
}

void MD3D12Viewport::CreateSwapChain()
{
	SwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC Desc;
	Desc.BufferDesc.Width = ViewportWidth;
	Desc.BufferDesc.Height = ViewportHeight;
	Desc.BufferDesc.RefreshRate.Numerator = 60;
	Desc.BufferDesc.RefreshRate.Denominator = 1;
	Desc.BufferDesc.Format = ViewportInfo.BackBufferFormat;
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Desc.SampleDesc.Count = ViewportInfo.bEnable4xMSAA ? 4 : 1;
	Desc.SampleDesc.Quality = ViewportInfo.bEnable4xMSAA ? (ViewportInfo.QualityOf4xMSAA - 1) : 0;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.BufferCount = SwapChainBufferCount;
	Desc.OutputWindow = ViewportInfo.WindowHandle;
	Desc.Windowed = true;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = D3D12RHI->GetDevice()->GetCommandQueue();

	ThrowIfFailed(D3D12RHI->GetDXGIFactory()->CreateSwapChain(CommandQueue.Get(), &Desc, SwapChain.GetAddressOf()));
}

void MD3D12Viewport::OnResize(int NewWidth, int NewHeight)
{
	ViewportWidth = NewWidth;
	ViewportHeight = NewHeight;

	D3D12RHI->GetDevice()->GetCommandContext()->FlushCommandQueue();

	D3D12RHI->GetDevice()->GetCommandContext()->ResetCommandList();

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		RenderTargetTextures[i].reset();
	}
	DepthStencilTexture.reset();

	ThrowIfFailed(SwapChain->ResizeBuffers(SwapChainBufferCount, ViewportWidth, ViewportHeight,
		ViewportInfo.BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	CurrentBackBuffer = 0;

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> SwapChainBuffer = nullptr;
		ThrowIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer)));

		D3D12_RESOURCE_DESC BackBufferDesc = SwapChainBuffer->GetDesc();

		MTextureInfo TextureInfo;
		TextureInfo.RTVFormat = BackBufferDesc.Format;
		TextureInfo.InitState = D3D12_RESOURCE_STATE_PRESENT;
		RenderTargetTextures[i] = D3D12RHI->CreateTexture(SwapChainBuffer, TextureInfo, TexCreate_RTV);
	}

	MTextureInfo TextureInfo;
	TextureInfo.Type = ETextureType::TEXTURE_2D;
	TextureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TextureInfo.Width = ViewportWidth;
	TextureInfo.Height = ViewportHeight;
	TextureInfo.Depth = 1;
	TextureInfo.MipCount = 1;
	TextureInfo.Arraysize = 1;
	TextureInfo.InitState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	TextureInfo.Format = DXGI_FORMAT_R24G8_TYPELESS;
	TextureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	DepthStencilTexture = D3D12RHI->CreateTexture(TextureInfo, TexCreate_DSV | TexCreate_SRV);

	D3D12RHI->GetDevice()->GetCommandContext()->ExcuteCommandList();

	D3D12RHI->GetDevice()->GetCommandContext()->FlushCommandQueue();
}

void MD3D12Viewport::GetD3DViewport(D3D12_VIEWPORT& OutD3DViewport, D3D12_RECT& OutD3DRect)
{
	OutD3DViewport.TopLeftX = 0;
	OutD3DViewport.TopLeftY = 0;
	OutD3DViewport.Width = static_cast<float>(ViewportWidth);
	OutD3DViewport.Height = static_cast<float>(ViewportHeight);
	OutD3DViewport.MinDepth = 0.0f;
	OutD3DViewport.MaxDepth = 1.0f;

	OutD3DRect = { 0, 0, ViewportWidth, ViewportHeight };
}

void MD3D12Viewport::Present()
{
	ThrowIfFailed(SwapChain->Present(0, 0));
	CurrentBackBuffer = (CurrentBackBuffer + 1) % SwapChainBufferCount;
}

MD3D12Resource* MD3D12Viewport::GetCurrentBackBuffer() const
{
	return RenderTargetTextures[CurrentBackBuffer]->GetResource();
}

MD3D12RenderTargetView* MD3D12Viewport::GetCurrentBackBufferView() const
{
	return RenderTargetTextures[CurrentBackBuffer]->GetRTV();
}

float* MD3D12Viewport::GetCurrentBackBufferClearValue() const
{
	return RenderTargetTextures[CurrentBackBuffer]->GetRTVClearValuePtr();
}

MD3D12DepthStencilView* MD3D12Viewport::GetDepthStencilView() const
{
	return RenderTargetTextures[CurrentBackBuffer]->GetDSV();
}

MD3D12ShaderResourceView* MD3D12Viewport::GetDepthShaderResourceView() const
{
	return RenderTargetTextures[CurrentBackBuffer]->GetSRV();
}

MD3D12ViewportInfo MD3D12Viewport::GetViewportInfo() const
{
	return ViewportInfo;
}
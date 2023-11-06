#pragma once

#include "D3D12Utils.h"
#include "D3D12Texture.h"

class MD3D12RHI;

struct MD3D12ViewportInfo
{
	HWND WindowHandle;

	DXGI_FORMAT BackBufferFormat;
	DXGI_FORMAT DepthStencilFormat;

	bool bEnable4xMSAA = false;
	UINT QualityOf4xMSAA = 0;
};

class MD3D12Viewport
{
public:
	MD3D12Viewport(MD3D12RHI* ID3D12RHI, const MD3D12ViewportInfo& Info, int Width, int Height);

	~MD3D12Viewport();

public:
	void OnResize(int NewWidth, int NewHeight);

	void GetD3DViewport(D3D12_VIEWPORT& OutD3DViewport, D3D12_RECT& OutD3DRect);

	void Present();

	MD3D12Resource* GetCurrentBackBuffer() const;

	MD3D12RenderTargetView* GetCurrentBackBufferView() const;

	float* GetCurrentBackBufferClearValue() const;

	MD3D12DepthStencilView* GetDepthStencilView() const;

	MD3D12ShaderResourceView* GetDepthShaderResourceView() const;

	MD3D12ViewportInfo GetViewportInfo() const;

private:
	void Initialize();

	void CreateSwapChain();

private:
	MD3D12RHI* D3D12RHI = nullptr;

	MD3D12ViewportInfo ViewportInfo;
	int ViewportWidth = 0;
	int ViewportHeight = 0;

	static const int SwapChainBufferCount = 2;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = nullptr;
	int CurrentBackBuffer = 0;

	MD3D12TextureRef RenderTargetTextures[SwapChainBufferCount];

	MD3D12TextureRef DepthStencilTexture = nullptr;
};
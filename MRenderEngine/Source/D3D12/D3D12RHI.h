#pragma once

#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include "D3D12Buffer.h"
#include "D3D12Texture.h"
#include "Texture/TextureInfo.h"
#include "Math/Math.h"

class MD3D12RHI
{
public:
	MD3D12RHI();

	~MD3D12RHI();

	void Initialize(HWND WindowHandle, int WindowWidth, int WindowHeight);

	void Destory();

public:
	MD3D12Device* GetDevice() { return Device.get(); }

	MD3D12Viewport* GetViewport() { return Viewport.get(); }

	const MD3D12ViewportInfo& GetViewportInfo();

	IDXGIFactory4* GetDXGIFactory();

public:
	void FlushCommandQueue();

	void ExcuteCommandList();

	void ResetCommandList();

	void ResetCommandAllocator();

	void Present();

	void ResizeViewport(int NewWidth, int NewHeight);

	void TransitionResource(MD3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter);

	void CopyResource(MD3D12Resource* DstResource, MD3D12Resource* SrcResource);

	void CopyBufferRegion(MD3D12Resource* DstResource, UINT64 DstOffset, MD3D12Resource* SrcResource, UINT64 SrcOffset, UINT64 Size);

	void CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox);

	MD3D12ConstantBufferRef CreateConstantBuffer(const void* Content, uint32_t Size);

	MD3D12StructuredBufferRef CreateStructuredBuffer(const void* Content, uint32_t ElementSize, uint32_t ElementCount);

	MD3D12RWStructuredBufferRef CreateRWStructuredBuffer(uint32_t ElementSize, uint32_t ElementCount);

	MD3D12VertexBufferRef CreateVertexBuffer(const void* Content, uint32_t Size);

	MD3D12IndexBufferRef CreateIndexBuffer(const void* Content, uint32_t Size);

	MD3D12ReadBackBufferRef CreateReadBackBuffer(uint32_t Size);

	MD3D12TextureRef CreateTexture(const MTextureInfo& TextureInfo, uint32_t CreateFlag, TVector4 RTVClearValue = TVector4::Zero);

	MD3D12TextureRef CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource, MTextureInfo& TextureInfo, uint32_t CreateFlag);

	void UploadTextureData(MD3D12TextureRef Texture, const std::vector<D3D12_SUBRESOURCE_DATA>& InitData);

	void SetVertexBuffer(const MD3D12VertexBufferRef& VertexBuffer, UINT Offset, UINT Stride, UINT Size);

	void SetIndexBuffer(const MD3D12IndexBufferRef& IndexBuffer, UINT Offset, DXGI_FORMAT Format, UINT Size);

	void EndFrame();

private:
	void CreateDefaultBuffer(uint32_t Size, uint32_t Alignment, D3D12_RESOURCE_FLAGS Flag, MD3D12ResourceLocation& ResourceLocation);

	void CreateAndInitDefaultBuffer(const void* Content, uint32_t Size, uint32_t Alignment, MD3D12ResourceLocation& ResourceLocation);

	MD3D12TextureRef CreateTextureResource(const MTextureInfo& TextureInfo, uint32_t CreateFlag, TVector4 RTVClearValue);

	void CreateTextureView(MD3D12TextureRef TextureRef, const MTextureInfo& TextureInfo, uint32_t CreateFlag);

private:
	void LogAdapters();

	void LogAdapterOutputs(IDXGIAdapter* adapter);

	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	UINT GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat);

private:
	std::unique_ptr<MD3D12Device> Device = nullptr;

	std::unique_ptr<MD3D12Viewport> Viewport = nullptr;

	MD3D12ViewportInfo ViewportInfo;

	Microsoft::WRL::ComPtr<IDXGIFactory4>DXGIFactory = nullptr;
};
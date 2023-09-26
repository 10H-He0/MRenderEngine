#pragma once

#include "D3D12HeapSlotAllocator.h"
#include "D3D12Device.h"

class MD3D12View
{
public:
	MD3D12View(MD3D12Device* IDevice, D3D12_DESCRIPTOR_HEAP_TYPE IType, ID3D12Resource* IResource);

	virtual ~MD3D12View();

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() { return HeapSlot.Handle; }

private:
	void Destroy();

protected:
	MD3D12Device* Device = nullptr;

	MD3D12HeapSlotAllocator* HeapSlotAllocator = nullptr;

	ID3D12Resource* Resource = nullptr;

	MD3D12HeapSlotAllocator::HeapSlot HeapSlot;

	D3D12_DESCRIPTOR_HEAP_TYPE Type;
};

class MD3D12ShaderResourceView : public MD3D12View
{
public:
	MD3D12ShaderResourceView(MD3D12Device* IDevice, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, ID3D12Resource* IResource);

	virtual ~MD3D12ShaderResourceView();

protected:
	void CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc);
};

class MD3D12RenderTargetView : public MD3D12View
{
public:
	MD3D12RenderTargetView(MD3D12Device* IDevice, const D3D12_RENDER_TARGET_VIEW_DESC& Desc, ID3D12Resource* IResource);

	virtual ~MD3D12RenderTargetView();

protected:
	void CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& Desc);
};

class MD3D12DepthStencilView : public MD3D12View
{
public:
	MD3D12DepthStencilView(MD3D12Device* IDevice, const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc, ID3D12Resource* IResource);

	virtual ~MD3D12DepthStencilView();

protected:
	void CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc);
};

class MD3D12UnorderedAccessView : public MD3D12View
{
public:
	MD3D12UnorderedAccessView(MD3D12Device* IDevice, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, ID3D12Resource* IResource);

	virtual ~MD3D12UnorderedAccessView();

protected:
	void CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc);
};

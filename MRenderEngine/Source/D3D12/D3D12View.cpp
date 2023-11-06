#include "D3D12View.h"
#include "D3D12Device.h"
#include <assert.h>

MD3D12View::MD3D12View(MD3D12Device* IDevice, D3D12_DESCRIPTOR_HEAP_TYPE IType, ID3D12Resource* IResource)
	:Device(IDevice),
	Type(IType),
	Resource(IResource)
{
	HeapSlotAllocator = Device->GetHeapSlotAllocator(Type);

	if (HeapSlotAllocator)
	{
		HeapSlot = HeapSlotAllocator->AllocateHeapSlot();
		assert(HeapSlot.Handle.ptr != 0);
	}
}

MD3D12View::~MD3D12View()
{
	Destroy();
}

void MD3D12View::Destroy()
{
	if (HeapSlotAllocator)
	{
		HeapSlotAllocator->FreeHeapSlot(HeapSlot);
	}
}

MD3D12ShaderResourceView::MD3D12ShaderResourceView(MD3D12Device* IDevice, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, ID3D12Resource* IResource)
	:MD3D12View(IDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, IResource)
{
	CreateShaderResourceView(Desc);
}

MD3D12ShaderResourceView::~MD3D12ShaderResourceView()
{

}

void MD3D12ShaderResourceView::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc)
{
	Device->GetD3DDevice()->CreateShaderResourceView(Resource, &Desc, HeapSlot.Handle);
}

MD3D12RenderTargetView::MD3D12RenderTargetView(MD3D12Device* IDevice, const D3D12_RENDER_TARGET_VIEW_DESC& Desc, ID3D12Resource* IResource)
	:MD3D12View(IDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, IResource)
{
	CreateRenderTargetView(Desc);
}

MD3D12RenderTargetView::~MD3D12RenderTargetView()
{

}

void MD3D12RenderTargetView::CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& Desc)
{
	Device->GetD3DDevice()->CreateRenderTargetView(Resource, &Desc, HeapSlot.Handle);
}

MD3D12DepthStencilView::MD3D12DepthStencilView(MD3D12Device* IDevice, const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc, ID3D12Resource* IResource)
	:MD3D12View(IDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, IResource)
{
	CreateDepthStencilView(Desc);
}

MD3D12DepthStencilView::~MD3D12DepthStencilView()
{

}

void MD3D12DepthStencilView::CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc)
{
	Device->GetD3DDevice()->CreateDepthStencilView(Resource, &Desc, HeapSlot.Handle);
}

MD3D12UnorderedAccessView::MD3D12UnorderedAccessView(MD3D12Device* IDevice, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, ID3D12Resource* IResource)
	:MD3D12View(IDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, IResource)
{
	CreateUnorderedAccessView(Desc);
}

MD3D12UnorderedAccessView::~MD3D12UnorderedAccessView()
{

}

void MD3D12UnorderedAccessView::CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc)
{
	Device->GetD3DDevice()->CreateUnorderedAccessView(Resource, nullptr, &Desc, HeapSlot.Handle);
}

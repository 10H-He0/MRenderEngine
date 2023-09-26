#include "D3D12DescriptorCache.h"
#include "D3D12Device.h"

MD3D12DescriptorCache::MD3D12DescriptorCache(MD3D12Device* IDevice)
	:Device(IDevice)
{
	CreateCacheCbvSrvUavDescriptorHeap();

	CreateCacheRtvDescriptorHeap();
}

MD3D12DescriptorCache::~MD3D12DescriptorCache()
{

}

void MD3D12DescriptorCache::CreateCacheCbvSrvUavDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
	SrvHeapDesc.NumDescriptors = MaxCbvSrvUavDescriptorCount;
	SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ThrowIfFailed(Device->GetD3DDevice()->CreateDescriptorHeap(&SrvHeapDesc, IID_PPV_ARGS(&CacheCbvSrvUavDescriptorHeap)));
	SetDebugName(CacheCbvSrvUavDescriptorHeap.Get(), L"MD3D12DescriptorCache CacheCbvSrvUavDescriptorHeap");

	CbvSrvUavDescriptorSize = Device->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE MD3D12DescriptorCache::AppendCbvSrvUavDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcDescriptors)
{
	uint32_t SlotNeeded = (uint32_t)SrcDescriptors.size();
	assert(CbvSrvUavDescriptorOffset + SlotNeeded < MaxCbvSrvUavDescriptorCount);

	auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);
	Device->GetD3DDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotNeeded, SlotNeeded, SrcDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	auto GpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);

	CbvSrvUavDescriptorOffset += SlotNeeded;

	return GpuDescriptorHandle;
}

void MD3D12DescriptorCache::ResetCacheCbvSrvUavDescriptorHeap()
{
	CbvSrvUavDescriptorOffset = 0;
}

void MD3D12DescriptorCache::CreateCacheRtvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
	RtvHeapDesc.NumDescriptors = MaxRtvDescriptorCount;
	RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(Device->GetD3DDevice()->CreateDescriptorHeap(&RtvHeapDesc, IID_PPV_ARGS(&CacheRtvDescriptorHeap)));
	SetDebugName(CacheRtvDescriptorHeap.Get(), L"MD3D12DescriptorCache CacheRtvDescriptorHeap");

	RtvDescriptorSize = Device->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void MD3D12DescriptorCache::AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGPUHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCPUHandle)
{
	uint32_t SlotNeeded = (uint32_t)RtvDescriptors.size();
	assert(RtvDescriptorOffset + SlotNeeded < MaxRtvDescriptorCount);

	auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
	Device->GetD3DDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotNeeded, SlotNeeded, RtvDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	OutGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
	
	OutCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

	RtvDescriptorOffset += SlotNeeded;
}

void MD3D12DescriptorCache::ResetCacheRtvDescriptorHeap()
{
	RtvDescriptorOffset = 0;
}

void MD3D12DescriptorCache::Reset()
{
	ResetCacheCbvSrvUavDescriptorHeap();

	ResetCacheRtvDescriptorHeap();
}
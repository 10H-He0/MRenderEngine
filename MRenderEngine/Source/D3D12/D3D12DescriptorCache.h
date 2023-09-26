#pragma once

#include "D3D12Utils.h"

class MD3D12Device;

class MD3D12DescriptorCache
{
public:
	MD3D12DescriptorCache(MD3D12Device* IDevice);

	~MD3D12DescriptorCache();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheCbvSrvUavDescriptorHeap() { return CacheCbvSrvUavDescriptorHeap; }

	CD3DX12_GPU_DESCRIPTOR_HANDLE AppendCbvSrvUavDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrvDescriptors);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheRtvDescripptorHeap() { return CacheRtvDescriptorHeap; }

	void AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGPUHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCPUHandle);

	void Reset();

private:
	void CreateCacheCbvSrvUavDescriptorHeap();

	void CreateCacheRtvDescriptorHeap();

	void ResetCacheCbvSrvUavDescriptorHeap();

	void ResetCacheRtvDescriptorHeap();

private:
	MD3D12Device* Device = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheCbvSrvUavDescriptorHeap = nullptr;

	UINT CbvSrvUavDescriptorSize;

	static const int MaxCbvSrvUavDescriptorCount = 2048;

	uint32_t CbvSrvUavDescriptorOffset = 0;

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheRtvDescriptorHeap = nullptr;

	UINT RtvDescriptorSize;

	static const int MaxRtvDescriptorCount = 1024;

	uint32_t RtvDescriptorOffset = 0;
};
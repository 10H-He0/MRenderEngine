#pragma once

#include "D3D12Utils.h"
#include "D3D12DescriptorCache.h"

class MD3D12Device;

class MD3D12CommandContext
{
public:
	MD3D12CommandContext(MD3D12Device* IDevice);

	~MD3D12CommandContext();

	void CreateCommandContext();

	void DestroyCommandContext();

	ID3D12GraphicsCommandList* GetCommandList() { return CommandList.Get(); }

	ID3D12CommandQueue* GetCommandQueue() { return CommandQueue.Get(); }

	MD3D12DescriptorCache* GetDescriptorCache() { return DescriptorCache.get(); }

	void ResetCommandList();

	void ResetCommandAllocator();

	void ExcuteCommandList();

	void FlushCommandQueue();

	void EndFrame();

private:
	MD3D12Device* Device = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator = nullptr;

	std::unique_ptr<MD3D12DescriptorCache> DescriptorCache = nullptr;

private:
	Microsoft::WRL::ComPtr<ID3D12Fence> Fence = nullptr;

	UINT64 CurrentFenceValue = 0;
};
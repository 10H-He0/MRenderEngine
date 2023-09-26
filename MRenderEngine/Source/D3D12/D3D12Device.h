#pragma once

#include "D3D12CommandContext.h"
#include "D3D12MemoryAllocator.h"
#include "D3D12HeapSlotAllocator.h"

class MD3D12RHI;

class MD3D12Device
{
public:
	MD3D12Device(MD3D12RHI* ID3D12RHI);

	~MD3D12Device();

	ID3D12Device* GetD3DDevice() { return D3DDevice.Get(); }

	MD3D12CommandContext* GetCommandContext() { return CommandContext.get(); }

	ID3D12GraphicsCommandList* GetCommandList() { return CommandContext->GetCommandList(); }

	ID3D12CommandQueue* GetCommandQueue() { return CommandContext->GetCommandQueue(); }

	MD3D12DefaultBufferAllocator* GetDefaultBufferAllocator() { return DefaultBufferAllocator.get(); }

	MD3D12UploadBufferAllocator* GetUploadBufferAllocator() { return UploadBufferAllocator.get(); }

	MD3D12TextureResourceAllocator* GetTextureResourceAllocator() { return TextureResourceAllocator.get(); }

	MD3D12HeapSlotAllocator* GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);

private:
	void Initialize();

private:
	MD3D12RHI* D3D12RHI = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> D3DDevice = nullptr;

	std::unique_ptr<MD3D12CommandContext> CommandContext = nullptr;

private:
	std::unique_ptr<MD3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;

	std::unique_ptr<MD3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;

	std::unique_ptr<MD3D12TextureResourceAllocator> TextureResourceAllocator = nullptr;

	std::unique_ptr<MD3D12HeapSlotAllocator> RTVHeapSlotAllocator = nullptr;

	std::unique_ptr<MD3D12HeapSlotAllocator> DSVHeapSlotAllocator = nullptr;

	std::unique_ptr<MD3D12HeapSlotAllocator> SRVHeapSlotAllocator = nullptr;
};
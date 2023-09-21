#pragma once

#include "D3D12Utils.h"
#include <list>

class MD3D12HeapSlotAllocator
{
public:
	typedef D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;
	typedef decltype(DescriptorHandle::ptr) DescriptorHandleRaw;

	struct HeapSlot
	{
		uint32_t HeapIndex;
		DescriptorHandle Handle;
	};

private:
	struct FreeRange
	{
		DescriptorHandleRaw Start;
		DescriptorHandleRaw End;
	};

	struct HeapEntry
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
		std::list<FreeRange> FreeList;

		HeapEntry() {}
	};

public:
	MD3D12HeapSlotAllocator(ID3D12Device* IDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t NumDescriptorsPerHeap);

	~MD3D12HeapSlotAllocator();

	HeapSlot AllocateHeapSlot();

	void FreeHeapSlot(const HeapSlot& Slot);

private:
	D3D12_DESCRIPTOR_HEAP_DESC CreateDesc(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t NumDescritorsPerHeap);

	void AllocateHeap();

private:
	ID3D12Device* Device;

	const D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;

	const uint32_t DescriptorSize;

	std::vector<HeapEntry> HeapMap;
};
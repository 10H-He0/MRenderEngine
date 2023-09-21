#include "D3D12HeapSlotAllocator.h"
#include <assert.h>

MD3D12HeapSlotAllocator::MD3D12HeapSlotAllocator(ID3D12Device* IDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t NumDescriptorsPerHeap)
	:Device(IDevice),
	HeapDesc(CreateDesc(type, NumDescriptorsPerHeap)),
	DescriptorSize(Device->GetDescriptorHandleIncrementSize(type))
{

}

MD3D12HeapSlotAllocator::~MD3D12HeapSlotAllocator()
{

}

D3D12_DESCRIPTOR_HEAP_DESC MD3D12HeapSlotAllocator::CreateDesc(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t NumDescritorsPerHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.Type = type;
	HeapDesc.NumDescriptors = NumDescritorsPerHeap;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HeapDesc.NodeMask = 0;

	return HeapDesc;
}

void MD3D12HeapSlotAllocator::AllocateHeap()
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
	ThrowIfFailed(Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&Heap)));
	SetDebugName(Heap.Get(), L"MD3D12HeapSlotAllocator Descriptor Heap");

	DescriptorHandle HeapBsae = Heap->GetCPUDescriptorHandleForHeapStart();
	assert(HeapBsae.ptr != 0);

	HeapEntry Entry;
	Entry.Heap = Heap;
	Entry.FreeList.push_back({ HeapBsae.ptr, HeapBsae.ptr + (SIZE_T)HeapDesc.NumDescriptors * DescriptorSize });

	HeapMap.push_back(Entry);
}

MD3D12HeapSlotAllocator::HeapSlot MD3D12HeapSlotAllocator::AllocateHeapSlot()
{
	int EntryIndex = -1;
	for (int i = 0; i < HeapMap.size(); i++)
	{
		if (HeapMap[i].FreeList.size() > 0)
		{
			EntryIndex = i;
			break;
		}
	}

	if (EntryIndex == -1)
	{
		AllocateHeap();

		EntryIndex = (int)(HeapMap.size() - 1);
	}

	HeapEntry& Entry = HeapMap[EntryIndex];
	assert(Entry.FreeList.size() > 0);

	FreeRange& Range = Entry.FreeList.front();
	HeapSlot Slot = { (uint32_t)EntryIndex, Range.Start };

	Range.Start += DescriptorSize;
	if (Range.Start == Range.End)
	{
		Entry.FreeList.pop_front();
	}

	return Slot;
}

void MD3D12HeapSlotAllocator::FreeHeapSlot(const HeapSlot& Slot)
{
	assert(Slot.HeapIndex < HeapMap.size());
	HeapEntry& Entry = HeapMap[Slot.HeapIndex];

	FreeRange NewRange =
	{
		Slot.Handle.ptr,
		Slot.Handle.ptr + DescriptorSize
	};

	bool bFound = false;
	for (auto Node = Entry.FreeList.begin(); Node != Entry.FreeList.end() && !bFound; Node++)
	{
		FreeRange& Range = *Node;
		assert(Range.Start < Range.End);

		if (Range.Start == NewRange.End)
		{
			Range.Start = NewRange.Start;
			bFound = true;
		}
		else if (Range.End == NewRange.Start)
		{
			Range.End = NewRange.End;
			bFound = true;
		}
		else
		{
			assert(Range.End > NewRange.Start || Range.Start > NewRange.Start);
			if (Range.Start > NewRange.Start)
			{
				Entry.FreeList.insert(Node, NewRange);
				bFound = true;
			}
		}
	}

	if (!bFound)
	{
		Entry.FreeList.push_back(NewRange);
	}
}
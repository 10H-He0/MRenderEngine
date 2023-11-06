#pragma once

#include "D3D12Utils.h"

class MD3D12BuddyAllocator;

class MD3D12Resource
{
public:
	MD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> ID3DResource, D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON);

	void Map();
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource = nullptr;

	D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

	D3D12_RESOURCE_STATES CurrentState;

	//UploadBuffer用到
	void* MappedBaseAddress = nullptr;
};

struct MD3D12BlockData
{
	uint32_t Offset = 0;
	uint32_t Order = 0;
	uint32_t ActualUsedSize = 0;

	MD3D12Resource* PlacedResource = nullptr;
};

class MD3D12ResourceLocation
{
public:
	enum class EResourceLocationType
	{
		Undefined,
		StandAlone,
		SubAllocation,
	};

public:
	MD3D12ResourceLocation();

	~MD3D12ResourceLocation();

	void ReleaseResource();

	void SetType(EResourceLocationType type) { ResourceLocationType = type; }

public:
	EResourceLocationType ResourceLocationType = EResourceLocationType::Undefined;

	//SubAllocation
	MD3D12BuddyAllocator* Allocator = nullptr;

	MD3D12BlockData BlockData;

	//StandAlone
	MD3D12Resource* UnderlyingResource = nullptr;

	union
	{
		uint64_t OffsetFromBaseOfResource;
		uint64_t OffsetFromBaseOfHeap;
	};

	D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

	//UploadBuffer用到
	void* MappedAddress = nullptr;
};

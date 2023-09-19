#include "D3D12Resource.h"
#include "D3D12MemoryAllocator.h"

using namespace Microsoft::WRL;

MD3D12Resource::MD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> ID3DResource, D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON)
	:D3DResource(ID3DResource), CurrentState(InitState)
{
	if (D3DResource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
	{
		GPUVirtualAddress = D3DResource->GetGPUVirtualAddress();
	}
}

void MD3D12Resource::Map()
{
	ThrowIfFailed(D3DResource->Map(0, nullptr, &MappedBaseAddress));
}

MD3D12ResourceLocation::MD3D12ResourceLocation()
{

}

MD3D12ResourceLocation::~MD3D12ResourceLocation()
{
	ReleaseResource();
}

void MD3D12ResourceLocation::ReleaseResource()
{
	switch (ResourceLocationType)
	{
	case MD3D12ResourceLocation::EResourceLocationType::Undefined:
		break;
	case MD3D12ResourceLocation::EResourceLocationType::StandAlone:
		delete UnderlyingResource;

		break;
	case MD3D12ResourceLocation::EResourceLocationType::SubAllocation:
		if (Allocator)
		{
			Allocator->DeAllocate(*this);
		}

		break;
	default:
		break;
	}
}
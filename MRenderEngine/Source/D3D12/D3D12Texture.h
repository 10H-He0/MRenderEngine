#pragma once

#include "D3D12Resource.h"
#include "D3D12View.h"
#include "Math/Math.h"

class MD3D12Texture
{
public:
	MD3D12Resource* GetResource() { return ResourceLocation.UnderlyingResource; }

	ID3D12Resource* GetD3DResource() { return ResourceLocation.UnderlyingResource->D3DResource.Get(); }

	void SetRTVClearValue(TVector4 ClearValue) { RTVClearValue = ClearValue; }

	TVector4 GetRTVClearValue() { return RTVClearValue; }

	float* GetRTVClearValuePtr() { return (float*)&RTVClearValue; }

	MD3D12ShaderResourceView* GetSRV(UINT Index = 0)
	{
		assert(SRVs.size() > Index);

		return SRVs[Index].get();
	}

	void AddSRV(std::unique_ptr<MD3D12ShaderResourceView>& SRV)
	{
		SRVs.emplace_back(std::move(SRV));
	}

	MD3D12RenderTargetView* GetRTV(UINT Index = 0)
	{
		assert(RTVs.size() > Index);

		return RTVs[Index].get();
	}

	void AddRTV(std::unique_ptr<MD3D12RenderTargetView>& RTV)
	{
		RTVs.emplace_back(std::move(RTV));
	}

	MD3D12DepthStencilView* GetDSV(UINT Index = 0)
	{
		assert(DSVs.size() > Index);

		return DSVs[Index].get();
	}

	void AddDSV(std::unique_ptr<MD3D12DepthStencilView>& DSV)
	{
		DSVs.emplace_back(std::move(DSV));
	}

	MD3D12UnorderedAccessView* GetUAV(UINT Index = 0)
	{
		assert(UAVs.size() > Index);

		return UAVs[Index].get();
	}

	void AddUAV(std::unique_ptr<MD3D12UnorderedAccessView>& UAV)
	{
		UAVs.emplace_back(std::move(UAV));
	}

public:
	MD3D12ResourceLocation ResourceLocation;

private:
	std::vector<std::unique_ptr<MD3D12ShaderResourceView>> SRVs;

	std::vector<std::unique_ptr<MD3D12RenderTargetView>> RTVs;

	std::vector<std::unique_ptr<MD3D12DepthStencilView>> DSVs;

	std::vector<std::unique_ptr<MD3D12UnorderedAccessView>> UAVs;

	TVector4 RTVClearValue;
};

typedef std::shared_ptr<MD3D12Texture> MD3D12TextureRef;
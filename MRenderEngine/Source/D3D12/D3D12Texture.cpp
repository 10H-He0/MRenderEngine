#include "D3D12Texture.h"
#include "D3D12RHI.h"

MD3D12TextureRef MD3D12RHI::CreateTexture(const MTextureInfo& TextureInfo, uint32_t CreateFlag, TVector4 RTVClearValue = TVector4::Zero)
{
	MD3D12TextureRef TextureRef = CreateTextureResource(TextureInfo, CreateFlag, RTVClearValue);

	CreateTextureView(TextureRef, TextureInfo, CreateFlag);

	return TextureRef;
}

MD3D12TextureRef MD3D12RHI::CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource, MTextureInfo& TextureInfo, uint32_t CreateFlag)
{
	MD3D12TextureRef TextureRef = std::make_shared<MD3D12Texture>();

	MD3D12Resource* NewResource = new MD3D12Resource(D3DResource, TextureInfo.InitState);
	TextureRef->ResourceLocation.UnderlyingResource = NewResource;
	TextureRef->ResourceLocation.SetType(MD3D12ResourceLocation::EResourceLocationType::StandAlone);

	CreateTextureView(TextureRef, TextureInfo, CreateFlag);

	return TextureRef;
}

MD3D12TextureRef MD3D12RHI::CreateTextureResource(const MTextureInfo& TextureInfo, uint32_t CreateFlag, TVector4 RTVClearValue)
{
	MD3D12TextureRef TextureRef = std::make_shared<MD3D12Texture>();

	D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;

	D3D12_RESOURCE_DESC TexDesc;
	ZeroMemory(&TexDesc, sizeof(D3D12_RESOURCE_DESC));
	TexDesc.Dimension = TextureInfo.Dimension;
	TexDesc.Alignment = 0;
	TexDesc.Width = TextureInfo.Width;
	TexDesc.Height = (uint32_t)TextureInfo.Height;
	TexDesc.DepthOrArraySize = (TextureInfo.Depth > 1) ? (uint16_t)TextureInfo.Depth : (uint16_t)TextureInfo.Arraysize;
	TexDesc.MipLevels = (uint16_t)TextureInfo.MipCount;
	TexDesc.Format = TextureInfo.Format;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	bool bCreateRTV = CreateFlag & (TexCreate_RTV | TexCreate_CubeRTV);
	bool bCreateDSV = CreateFlag & (TexCreate_DSV | TexCreate_CubeDSV);
	bool bCreateUAV = CreateFlag & TexCreate_UAV;

	if (bCreateRTV)
	{
		TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	else if (bCreateDSV)
	{
		TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}
	else if (bCreateUAV)
	{
		TexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}
	else
	{
		TexDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	bool bReadOnlyTexture = !(bCreateRTV | bCreateDSV | bCreateUAV);
	if (bReadOnlyTexture)
	{
		auto TextureResourceAllocator = GetDevice()->GetTextureResourceAllocator();
		TextureResourceAllocator->AllocateTextureResource(ResourceState, TexDesc, TextureRef->ResourceLocation);

		auto TextureResource = TextureRef->GetD3DResource();
		assert(TextureResource);
	}
	else
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource;

		CD3DX12_CLEAR_VALUE ClearValue = {};
		CD3DX12_CLEAR_VALUE* ClearValuePtr = nullptr;

		if (bCreateRTV)
		{
			ClearValue = CD3DX12_CLEAR_VALUE(TexDesc.Format, (float*)&RTVClearValue);
			ClearValuePtr = &ClearValue;

			TextureRef->SetRTVClearValue(RTVClearValue);
		}
		else if (bCreateDSV)
		{
			FLOAT Depth = 1.0f;
			UINT8 Stencil = 0;
			ClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, Depth, Stencil);
			ClearValuePtr = &ClearValue;
		}

		GetDevice()->GetD3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&TexDesc,
			TextureInfo.InitState,
			ClearValuePtr,
			IID_PPV_ARGS(&Resource)
		);

		MD3D12Resource* NewResource = new MD3D12Resource(Resource, TextureInfo.InitState);
		TextureRef->ResourceLocation.UnderlyingResource = NewResource;
		TextureRef->ResourceLocation.SetType(MD3D12ResourceLocation::EResourceLocationType::StandAlone);
	}

	return TextureRef;
}

void MD3D12RHI::CreateTextureView(MD3D12TextureRef TextureRef, const MTextureInfo& TextureInfo, uint32_t CreateFlag)
{
	auto TextureResource = TextureRef->GetD3DResource();

	if (CreateFlag & TexCreate_SRV)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	
		if (TextureInfo.SRVFormat == DXGI_FORMAT_UNKNOWN)
		{
			SRVDesc.Format = TextureInfo.Format;
		}
		else
		{
			SRVDesc.Format = TextureInfo.SRVFormat;
		}

		if (TextureInfo.Type == ETextureType::TEXTURE_2D)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.MipLevels = (uint16_t)TextureInfo.MipCount;
		}
		else if (TextureInfo.Type == ETextureType::TEXTURE_CUBE)
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			SRVDesc.TextureCube.MostDetailedMip = 0;
			SRVDesc.TextureCube.MipLevels = (uint16_t)TextureInfo.MipCount;
		}
		else
		{
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			SRVDesc.Texture3D.MostDetailedMip = 0;
			SRVDesc.Texture3D.MipLevels = (uint16_t)TextureInfo.MipCount;
		}

		TextureRef->AddSRV(std::make_unique<MD3D12ShaderResourceView>(GetDevice(), SRVDesc, TextureResource));
	}

	if (CreateFlag & TexCreate_RTV)
	{
		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;

		if (TextureInfo.RTVFormat == DXGI_FORMAT_UNKNOWN)
		{
			RTVDesc.Format = TextureInfo.Format;
		}
		else
		{
			RTVDesc.Format = TextureInfo.RTVFormat;
		}

		TextureRef->AddRTV(std::make_unique<MD3D12RenderTargetView>(GetDevice(), RTVDesc, TextureResource));
	}
	else if (CreateFlag & TexCreate_CubeRTV)
	{
		for (size_t i = 0; i < 6; i++)
		{
			D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
			RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.MipSlice = 0;
			RTVDesc.Texture2DArray.PlaneSlice = 0;
			RTVDesc.Texture2DArray.FirstArraySlice = (UINT)i;
			RTVDesc.Texture2DArray.ArraySize = 1;

			if (TextureInfo.RTVFormat == DXGI_FORMAT_UNKNOWN)
			{
				RTVDesc.Format = TextureInfo.Format;
			}
			else
			{
				RTVDesc.Format = TextureInfo.RTVFormat;
			}

			TextureRef->AddRTV(std::make_unique<MD3D12RenderTargetView>(GetDevice(), RTVDesc, TextureResource));
		}
	}

	if (CreateFlag & TexCreate_DSV)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
		DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
		DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;

		if (TextureInfo.DSVFormat == DXGI_FORMAT_UNKNOWN)
		{
			DSVDesc.Format = TextureInfo.Format;
		}
		else
		{
			DSVDesc.Format = TextureInfo.DSVFormat;
		}

		TextureRef->AddDSV(std::make_unique<MD3D12DepthStencilView>(GetDevice(), DSVDesc, TextureResource));
	}
	else if (CreateFlag & TexCreate_CubeDSV)
	{
		for (size_t i = 0; i < 6; i++)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
			DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
			DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			DSVDesc.Texture2DArray.MipSlice = 0;
			DSVDesc.Texture2DArray.FirstArraySlice = (UINT)i;
			DSVDesc.Texture2DArray.ArraySize = 1;
		
			if (TextureInfo.DSVFormat == DXGI_FORMAT_UNKNOWN)
			{
				DSVDesc.Format = TextureInfo.Format;
			}
			else
			{
				DSVDesc.Format = TextureInfo.DSVFormat;
			}

			TextureRef->AddDSV(std::make_unique<MD3D12DepthStencilView>(GetDevice(), DSVDesc, TextureResource));
		}
	}

	if (CreateFlag & TexCreate_UAV)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		UAVDesc.Texture2D.MipSlice = 0;

		if (TextureInfo.UAVFormat == DXGI_FORMAT_UNKNOWN)
		{
			UAVDesc.Format = TextureInfo.Format;
		}
		else
		{
			UAVDesc.Format = TextureInfo.UAVFormat;
		}

		TextureRef->AddUAV(std::make_unique<MD3D12UnorderedAccessView>(GetDevice(), UAVDesc, TextureResource));
	}
}

void MD3D12RHI::UploadTextureData(MD3D12TextureRef Texture, const std::vector<D3D12_SUBRESOURCE_DATA>& InitData)
{
	auto TextureResource = Texture->GetResource();
	D3D12_RESOURCE_DESC TexDesc = TextureResource->D3DResource->GetDesc();

	const UINT NumSubResources = (UINT)InitData.size();
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>Layouts(NumSubResources);
	std::vector<uint32_t>NowRows(NumSubResources);
	std::vector<uint64_t>RowSizeInbytes(NumSubResources);

	uint64_t RequiredSize = 0;
	Device->GetD3DDevice()->GetCopyableFootprints(&TexDesc, 0, NumSubResources, 0, &Layouts[0], &NowRows[0], &RowSizeInbytes[0], &RequiredSize);
	
	MD3D12ResourceLocation UploadResourceLocation;
	auto UploadResourceAllocator = GetDevice()->GetUploadBufferAllocator();
	void* MappedData = UploadResourceAllocator->AllocateUploadResource((uint32_t)RequiredSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, UploadResourceLocation);
	ID3D12Resource* UploadBuffer = UploadResourceLocation.UnderlyingResource->D3DResource.Get();

	for (uint32_t i = 0; i < NumSubResources; i++)
	{
		if (RowSizeInbytes[i] > SIZE_T(-1))
		{
			assert(0);
		}
		D3D12_MEMCPY_DEST DestData = {(BYTE*)MappedData + Layouts[i].Offset, Layouts[i].Footprint.RowPitch, SIZE_T(Layouts[i].Footprint.RowPitch) * SIZE_T(NowRows[i])};
		MemcpySubresource(&DestData, &(InitData[i]), static_cast<SIZE_T>(RowSizeInbytes[i]), NowRows[i], Layouts[i].Footprint.Depth);
	}

	TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COPY_DEST);

	for (UINT i = 0; i < NumSubResources; i++)
	{
		Layouts[i].Offset += UploadResourceLocation.OffsetFromBaseOfResource;

		CD3DX12_TEXTURE_COPY_LOCATION Src;
		Src.pResource = UploadBuffer;
		Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		Src.PlacedFootprint = Layouts[i];

		CD3DX12_TEXTURE_COPY_LOCATION Dst;
		Dst.pResource = TextureResource->D3DResource.Get();
		Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		Dst.SubresourceIndex = i;

		CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
	}

	TransitionResource(TextureResource, D3D12_RESOURCE_STATE_COMMON);
}
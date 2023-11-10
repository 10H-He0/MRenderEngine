#include "Texture.h"
#include "TextureLoader/DDSTextureLoader.h"
#include "TextureLoader/WICTextureLoader.h"
#include "TextureLoader/HDRTextureLoader.h"
#include "Utils/FormatConvert.h"

void MTexture::LoadTextureResourceFromFile(MD3D12RHI* RHI)
{
	std::wstring extention = GetExtention(FilePath);
	if (extention == L"dds")
	{
		LoadDDSTexture(RHI->GetDevice());
	}
	else if (extention == L"jpg" || extention == L"png")
	{
		LoadWICTexture(RHI->GetDevice());
	}
	else if (extention == L"hdr")
	{
		LoadHDRTexture(RHI->GetDevice());
	}
}

std::wstring MTexture::GetExtention(std::wstring path)
{
	if ((path.rfind('.') != std::wstring::npos) && (path.rfind('.') != (path.length() - 1)))
		return path.substr(path.rfind('.') + 1);
	else
		return L"";
}

void MTexture::LoadDDSTexture(MD3D12Device* Device)
{
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile(FilePath.c_str(), TextureResource.TextureInfo, TextureResource.InitData,
		TextureResource.TextureData, bSRGB));
}

void MTexture::LoadWICTexture(MD3D12Device* Device)
{
	D3D12_SUBRESOURCE_DATA InitData;

	DirectX::WIC_LOADER_FLAGS LoadFlags;
	if (bSRGB)
	{
		LoadFlags = DirectX::WIC_LOADER_FORCE_SRGB;
	}
	else
	{
		LoadFlags = DirectX::WIC_LOADER_IGNORE_SRGB;
	}

	ThrowIfFailed(DirectX::CreateWICTextureFromFile(FilePath.c_str(), 0u, D3D12_RESOURCE_FLAG_NONE, LoadFlags,
		TextureResource.TextureInfo, InitData, TextureResource.TextureData));

	TextureResource.InitData.push_back(InitData);
}

void MTexture::LoadHDRTexture(MD3D12Device* Device)
{
	D3D12_SUBRESOURCE_DATA InitData;

	CreateHDRTextureFromFile(TFormatConvert::WStrToStr(FilePath), TextureResource.TextureInfo, InitData, TextureResource.TextureData);

	TextureResource.InitData.push_back(InitData);
}

void MTexture::SetTextureResourceDirectly(const MTextureInfo& ITextureInfo, const std::vector<uint8_t>& ITextureData, const D3D12_SUBRESOURCE_DATA& IInitData)
{
	TextureResource.TextureInfo = ITextureInfo;
	TextureResource.TextureData = ITextureData;

	D3D12_SUBRESOURCE_DATA InitData;
	InitData.pData = TextureResource.TextureData.data();
	InitData.RowPitch = IInitData.RowPitch;
	InitData.SlicePitch = IInitData.SlicePitch;

	TextureResource.InitData.push_back(InitData);
}

void MTexture::CreateTexture(MD3D12RHI* RHI)
{
	auto& TextureInfo = TextureResource.TextureInfo;
	TextureInfo.Type = Type;
	D3DTexture = RHI->CreateTexture(TextureInfo, TexCreate_SRV);

	RHI->UploadTextureData(D3DTexture, TextureResource.InitData);
}

MTexture2D::MTexture2D(const std::string& IName, bool IbSRGB, std::wstring IFilePath)
	:MTexture(IName, ETextureType::TEXTURE_2D, IbSRGB, IFilePath)
{

}

MTexture2D::~MTexture2D()
{

}

MTextureCube::MTextureCube(const std::string& IName, bool IbSRGB, std::wstring IFilePath)
	:MTexture(IName, ETextureType::TEXTURE_CUBE, IbSRGB, IFilePath)
{

}

MTextureCube::~MTextureCube()
{

}

MTexture3D::MTexture3D(const std::string& IName, bool IbSRGB, std::wstring IFilePath)
	:MTexture(IName, ETextureType::TEXTURE_3D, IbSRGB, IFilePath)
{

}

MTexture3D::~MTexture3D()
{

}
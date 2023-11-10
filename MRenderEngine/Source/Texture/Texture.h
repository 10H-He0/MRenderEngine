#pragma once

#include <string>
#include "Texture/TextureInfo.h"
#include "D3D12/D3D12Texture.h"
#include "D3D12/D3D12RHI.h"

struct MTextureResource
{
	MTextureInfo TextureInfo;

	std::vector<uint8_t> TextureData;

	std::vector<D3D12_SUBRESOURCE_DATA> InitData;
};

class MTexture
{
public:
	MTexture(const std::string& IName, ETextureType IType, bool IbSRGB, std::wstring IFilePath)
		:Name(IName),Type(IType),FilePath(IFilePath),bSRGB(IbSRGB)
	{}

	virtual ~MTexture()
	{}

	MTexture(const MTexture& rhs) = delete;

	MTexture& operator=(const MTexture& rhs) = delete;

public:
	void LoadTextureResourceFromFile(MD3D12RHI* RHI);

	void SetTextureResourceDirectly(const MTextureInfo& ITextureInfo, const std::vector<uint8_t>& ITextureData, const D3D12_SUBRESOURCE_DATA& IInitData);

	void CreateTexture(MD3D12RHI* RHI);

	MD3D12TextureRef GetD3DTexture() { return D3DTexture; }

private:
	static std::wstring GetExtention(std::wstring path);

	void LoadDDSTexture(MD3D12Device* Device);

	void LoadWICTexture(MD3D12Device* Device);

	void LoadHDRTexture(MD3D12Device* Device);

private:
	std::string Name;

	ETextureType Type;

	std::wstring FilePath;

	bool bSRGB = true;

	MTextureResource TextureResource;

	MD3D12TextureRef D3DTexture = nullptr;
};

class MTexture2D : public MTexture
{
public:
	MTexture2D(const std::string& IName, bool IbSRGB, std::wstring IFilePath);

	~MTexture2D();
};

class MTextureCube : public MTexture
{
public:
	MTextureCube(const std::string& IName, bool IbSRGB, std::wstring IFilePath);

	~MTextureCube();
};

class MTexture3D : public MTexture
{
public:
	MTexture3D(const std::string& IName, bool IbSRGB, std::wstring IFilePath);

	~MTexture3D();
};
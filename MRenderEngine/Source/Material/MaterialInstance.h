#pragma once

#include "Material.h"
#include "D3D12/D3D12Buffer.h"

class MD3D12RHI;

class MMaterialInstance
{
public:
	MMaterialInstance(MMaterial* IMaterial, const std::string& IName);

public:
	void SetTextureParameter(const std::string& Parameter, const std::string& TextureName);

	void CreateMaterialConstantBuffer(MD3D12RHI* D3D12RHI);

public:
	MMaterial* Material = nullptr;

	std::string Name;

	MMaterialParameters Parameters;

	MD3D12ConstantBufferRef MaterialConstantBuffer = nullptr;
};
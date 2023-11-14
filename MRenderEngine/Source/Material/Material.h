#pragma once

#include <string>
#include <unordered_map>
#include "D3D12/D3D12Utils.h"
#include "Shader/Shader.h"
#include "Math/Math.h"

enum class EShadingMode
{
	DefaultLit,
	UnLit,
};

struct MMaterialParameters
{
public:
	TVector4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	TVector3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	
	float Roughness = 64.0f;
	
	TVector3 EmissiveColor = { 0.0f, 0.0f, 0.0f };

	TMatrix MatTransform = TMatrix::Identity;

	std::unordered_map<std::string, std::string> TextureMap;
};

struct MMaterialRenderState
{
	D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK;

	D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS;
};

class MMaterial
{
public:
	MMaterial(const std::string& IName, const std::string& IShaderName);

	MShader* GetShader(const MShaderDefine& ShaderDefine, MD3D12RHI* D3D12RHI);

public:
	std::string Name;

	EShadingMode ShadingMode = EShadingMode::DefaultLit;

	MMaterialParameters Parameters;

	MMaterialRenderState RenderState;

private:
	std::string ShaderName;

	std::unordered_map<MShaderDefine, std::unique_ptr<MShader>> ShaderMap;
};
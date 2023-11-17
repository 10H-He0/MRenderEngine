#include "Material.h"

MMaterial::MMaterial(const std::string& IName, const std::string& IShaderName)
	:Name(IName),ShaderName(IShaderName)
{}

MShader* MMaterial::GetShader(const MShaderDefine& ShaderDefine, MD3D12RHI* D3D12RHI)
{
	auto Iter = ShaderMap.find(ShaderDefine);

	if (Iter == ShaderMap.end())
	{
		MShaderInfo ShaderInfo;
		ShaderInfo.ShaderName = ShaderName;
		ShaderInfo.FileName = ShaderName;
		ShaderInfo.shaderDefine = ShaderDefine;
		ShaderInfo.bCreateVS = true;
		ShaderInfo.bCreatePS = true;
		std::unique_ptr<MShader> NewShader = std::make_unique<MShader>(ShaderInfo, D3D12RHI);

		ShaderMap.insert({ ShaderDefine, std::move(NewShader) });

		return ShaderMap[ShaderDefine].get();
	}
	else
	{
		Iter->second.get();
	}
}
#include "MaterialInstance.h"
#include "Render/RenderProxy.h"
#include "D3D12/D3D12RHI.h"

MMaterialInstance::MMaterialInstance(MMaterial* IMaterial, const std::string& IName)
	:Material(IMaterial),Name(IName)
{
	Parameters = Material->Parameters;
}

void MMaterialInstance::SetTextureParameter(const std::string& Parmeter, const std::string& TextureName)
{
	auto Iter = Parameters.TextureMap.find(Parmeter);
	if (Iter != Parameters.TextureMap.end())
	{
		Iter->second = TextureName;
	}
}

void MMaterialInstance::CreateMaterialConstantBuffer(MD3D12RHI* D3D12RHI)
{
	MMaterialConstants MaterialConstants;

	MaterialConstants.DiffuseAlbedo = Parameters.DiffuseAlbedo;
	MaterialConstants.EmissiveColor = Parameters.EmissiveColor;
	MaterialConstants.FresnelR0 = Parameters.FresnelR0;
	MaterialConstants.MatTransform = Parameters.MatTransform;
	MaterialConstants.Roughness = Parameters.Roughness;
	MaterialConstants.ShadingModle = static_cast<UINT>(Material->ShadingMode);

	MaterialConstantBuffer = D3D12RHI->CreateConstantBuffer(&MaterialConstants, sizeof(MaterialConstants));
}
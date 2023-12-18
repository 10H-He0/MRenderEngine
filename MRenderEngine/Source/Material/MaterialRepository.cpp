#include "MaterialRepository.h"

MMaterialRepository& MMaterialRepository::Get()
{
	static MMaterialRepository Instance;
	return Instance;
}

void MMaterialRepository::Load()
{
	{
		MMaterial* DefaultMat = CreateMaterial("DefaultMat", "BasePassDefault");

		MMaterialParameters& Parameters = DefaultMat->Parameters;
		Parameters.TextureMap.emplace("BaseColorTexture", "NullTex");
		Parameters.TextureMap.emplace("NormalTexture", "NullTex");
		Parameters.TextureMap.emplace("MetallicTexture", "NullTex");
		Parameters.TextureMap.emplace("RoughnessTexture", "NullTex");

		CreateDefaultMaterialInstance(DefaultMat);

		{
			MMaterialInstance* AssaultRifleMatInst = CreateMaterialInstance(DefaultMat, "AssaultRifleMatInst");

			AssaultRifleMatInst->SetTextureParameter("BaseColorTexture", "AssaultRifle_BaseColor");
			AssaultRifleMatInst->SetTextureParameter("NormalTexture", "AssaultRifle_Normal");
			AssaultRifleMatInst->SetTextureParameter("MetallicTexture", "AssaultRifle_Metallic");
			AssaultRifleMatInst->SetTextureParameter("RoughnessTexture", "AssaultRifle_Roughness");
		}

		{
			MMaterialInstance* CyborgWeaponMatInst = CreateMaterialInstance(DefaultMat, "CyborgWeaponMatInst");

			CyborgWeaponMatInst->SetTextureParameter("BaseColorTexture", "CyborgWeapon_BaseColor");
			CyborgWeaponMatInst->SetTextureParameter("NormalTexture", "CyborgWeapon_Normal");
			CyborgWeaponMatInst->SetTextureParameter("MetallicTexture", "CyborgWeapon_Metallic");
			CyborgWeaponMatInst->SetTextureParameter("RoughnessTexture", "CyborgWeapon_Roughness");
		}

		{
			MMaterialInstance* HelmetMatInst = CreateMaterialInstance(DefaultMat, "HelmetMatInst");
			HelmetMatInst->SetTextureParameter("BaseColorTexture", "Helmet_BaseColor");
			HelmetMatInst->SetTextureParameter("NormalTexture", "Helmet_Normal");
			HelmetMatInst->SetTextureParameter("MetallicTexture", "Helmet_Metallic");
			HelmetMatInst->SetTextureParameter("RoughnessTexture", "Helmet_Roughness");
		}

		{
			MMaterialInstance* RustedIronMatInst = CreateMaterialInstance(DefaultMat, "RustedIronMatInst");
			RustedIronMatInst->SetTextureParameter("BaseColorTexture", "RustedIron_BaseColor");
			RustedIronMatInst->SetTextureParameter("NormalTexture", "RustedIron_Normal");
			RustedIronMatInst->SetTextureParameter("MetallicTexture", "RustedIron_Metallic");
			RustedIronMatInst->SetTextureParameter("RoughnessTexture", "RustedIron_Roughness");
		}

		{
			MMaterialInstance* GrayGraniteMatInst = CreateMaterialInstance(DefaultMat, "GrayGraniteMatInst");
			GrayGraniteMatInst->SetTextureParameter("BaseColorTexture", "GrayGranite_BaseColor");
			GrayGraniteMatInst->SetTextureParameter("NormalTexture", "GrayGranite_Normal");
			GrayGraniteMatInst->SetTextureParameter("MetallicTexture", "GrayGranite_Metallic");
			GrayGraniteMatInst->SetTextureParameter("RoughnessTexture", "GrayGranite_Roughness");
		}

		{
			MMaterialInstance* FloorMatInst = CreateMaterialInstance(DefaultMat, "FloorMatInst");
			FloorMatInst->SetTextureParameter("BaseColorTexture", "Floor_BaseColor");
			FloorMatInst->SetTextureParameter("NormalTexture", "Floor_Normal");
			FloorMatInst->SetTextureParameter("MetallicTexture", "Floor_Metallic");
			FloorMatInst->SetTextureParameter("RoughnessTexture", "Floor_Roughness");
		}

		{
			MMaterialInstance* ColumnMatInst = CreateMaterialInstance(DefaultMat, "ColumnMatInst");
			ColumnMatInst->SetTextureParameter("BaseColorTexture", "Column_BaseColor");
			ColumnMatInst->SetTextureParameter("NormalTexture", "Column_Normal");
			ColumnMatInst->SetTextureParameter("MetallicTexture", "NullTex");
			ColumnMatInst->SetTextureParameter("RoughnessTexture", "Column_Roughness");
		}

		{
			MMaterialInstance* EmissiveMatInst = CreateMaterialInstance(DefaultMat, "EmissiveMatInst");

			MMaterialParameters& Parameters = EmissiveMatInst->Parameters;
			Parameters.EmissiveColor = TVector3(1.0f);
			EmissiveMatInst->SetTextureParameter("BaseColorTexture", "NullTex");
			EmissiveMatInst->SetTextureParameter("NormalTexture", "NullTex");
			EmissiveMatInst->SetTextureParameter("MetallicTexture", "NullTex");
			EmissiveMatInst->SetTextureParameter("RoughnessTexture", "NullTex");
		}
	}

	{
		MMaterial* SkyMat = CreateMaterial("SkyMat", "BasePassSky");

		MMaterialParameters& Parameters = SkyMat->Parameters;
		Parameters.TextureMap.emplace("SkyCubeTexture", "Shiodome_Stairs");

		MMaterialRenderState& RenderState = SkyMat->RenderState;
		RenderState.CullMode = D3D12_CULL_MODE_NONE;
		RenderState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		SkyMat->ShadingMode = EShadingMode::UnLit;

		CreateDefaultMaterialInstance(SkyMat);
	}
}

void MMaterialRepository::UnLoad()
{
	MaterialInstanceMap.clear();

	MaterialMap.clear();
}

MMaterial* MMaterialRepository::CreateMaterial(const std::string& MaterialName, const std::string ShaderName)
{
	MaterialMap.insert({ MaterialName, std::make_unique<MMaterial>(MaterialName, ShaderName) });
	return MaterialMap[MaterialName].get();
}

MMaterialInstance* MMaterialRepository::CreateMaterialInstance(MMaterial* Material, const std::string MaterialInstanceName)
{
	MaterialInstanceMap.insert({ MaterialInstanceName, std::make_unique<MMaterialInstance>(Material, MaterialInstanceName) });
	return MaterialInstanceMap[MaterialInstanceName].get();
}

void MMaterialRepository::CreateDefaultMaterialInstance(MMaterial* Material)
{
	CreateMaterialInstance(Material, Material->Name + "Inst");
}

MMaterialInstance* MMaterialRepository::GetMaterialInstance(const std::string& MaterialInstanceName) const
{
	MMaterialInstance* Res = nullptr;

	auto Iter = MaterialInstanceMap.find(MaterialInstanceName);
	if (Iter != MaterialInstanceMap.end())
	{
		Res = Iter->second.get();
	}

	return Res;
}

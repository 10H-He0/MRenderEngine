#pragma once

#include <unordered_map>
#include <string>
#include "Material.h"
#include "MaterialInstance.h"

class MMaterialRepository
{
public:
	static MMaterialRepository& Get();

	void Load();

	void UnLoad();

	MMaterialInstance* GetMaterialInstance(const std::string& MaterialInstanceName) const;

private:
	MMaterial* CreateMaterial(const std::string& MaterialName, const std::string ShaderName);

	MMaterialInstance* CreateMaterialInstance(MMaterial* Material, const std::string MaterialInstanceName);

	void CreateDefaultMaterialInstance(MMaterial* Material);

public:
	std::unordered_map<std::string, std::unique_ptr<MMaterial>> MaterialMap;

	std::unordered_map<std::string, std::unique_ptr<MMaterialInstance>> MaterialInstanceMap;
};
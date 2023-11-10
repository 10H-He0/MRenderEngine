#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "Texture.h"

class MTextureRepository
{
public:
	static MTextureRepository& Get();

	void Load();

	void UnLoad();

private:
	std::unordered_map<std::string, std::shared_ptr<MTexture>> TextureMap;
};
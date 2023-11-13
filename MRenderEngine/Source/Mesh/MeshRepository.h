#pragma once

#include <string>
#include <unordered_map>
#include "Mesh.h"
#include "FbxLoader.h"

class MMeshRepository
{
public:
	MMeshRepository();

	static MMeshRepository& Get();

	void Load();

	void Unload();

public:
	std::unordered_map<std::string, MMesh> MeshMap;

private:
	std::unique_ptr<TFbxLoader> FbxLoader;
};
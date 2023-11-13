#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Vertex.h"
#include "BoundingBox.h"
#include "Texture/Texture.h"

struct MMeshSDFDescriptor
{
	TVector3 Center;
	float Extention;

	int Resolution;
	int pad1;
	int pad2;
	int pad3;
};

class MMesh
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	MMesh();

	MMesh(MMesh&&) = delete;

	MMesh(const MMesh&) = delete;

	MMesh& operator=(const MMesh&) = delete;

public:
	void CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	void CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	void CreateGrid(float width, float depth, uint32 m, uint32 n);

	void CreateQuad(float x, float y, float w, float h, float depth);

	const std::vector<uint16>& GetIndices16() const;

	std::string GetInPutLayoutName() const;

	void GenerateIndices16();

public:
	void GenerateBoundingBox();

	MBoundingBox GetBoundingBox() { return BoundingBox; }

	void SetSDFTexture(std::unique_ptr<MTexture3D>& ISDFTexture)
	{
		SDFTexture = std::move(ISDFTexture);
	}

	MTexture3D* GetSDFTexture()
	{
		if (SDFTexture)
		{
			return SDFTexture.get();
		}
		else return nullptr;
	}

private:
	void Subdivide();

	MVertex MidPoint(const MVertex& v0, const MVertex& v1);

	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

public:
	std::string MeshName;

	std::vector<MVertex> Vertices;

	std::vector<uint16> Indices16;

	std::vector<uint32> Indices32;

	std::string InputLayoutName;

	MBoundingBox BoundingBox;

	std::unique_ptr<MTexture3D> SDFTexture = nullptr;

	MMeshSDFDescriptor SDFDescriptor;
};
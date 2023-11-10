#pragma once

#include <DirectXCollision.h>
#include <vector>
#include "Ray.h"
#include "Vertex.h"
#include "Math/Transform.h"

class MBoundingBox
{
public:
	void Init(std::vector<TVector3> Points);

	void Init(std::vector<MVertex> Vertices);

	TVector3 GetCenter() const { return (Min + Max) * 0.5; }

	TVector3 GetExtend() const { return (Max - Min) * 0.5; }

	TVector3 GetSize() const { return (Max - Min); }

	int GetWidesetAxis() const;

	float GetMaxWidth() const;

	float GetSurfaceArea() const;

	static MBoundingBox Union(const MBoundingBox& BoxA, const MBoundingBox& BoxB);

	static MBoundingBox Union(const MBoundingBox& Box, const TVector3& Point);

	MBoundingBox Transform(const TTransform& T);

	bool Intersect(const MRay& Ray, float& Dist0, float& Dist1);

	DirectX::BoundingBox GetD3DBoundingBox();

public:
	bool bInit = false;

	TVector3 Min = TVector3(TMath::Infinity);
	TVector3 Max = TVector3(-TMath::Infinity);
};
#include "BoundingBox.h"

void MBoundingBox::Init(std::vector<TVector3> Points)
{
	if (Points.size() > 0)
	{
		for (const TVector3& Point : Points)
		{
			Min = TVector3::Min(Min, Point);
			Max = TVector3::Max(Max, Point);
		}

		bInit = true;
	}
}

void MBoundingBox::Init(std::vector<MVertex> Vertices)
{
	if (Vertices.size() > 0)
	{
		for (const MVertex& Vertex : Vertices)
		{
			TVector3 Point = Vertex.Position;
			
			Min = TVector3::Min(Min, Point);
			Max = TVector3::Max(Max, Point);
		}

		bInit = true;
	}
}

DirectX::BoundingBox MBoundingBox::GetD3DBoundingBox()
{
	DirectX::BoundingBox Box;

	Box.Center = GetCenter();
	Box.Extents = GetExtend();

	return Box;
}

int MBoundingBox::GetWidesetAxis() const
{
	TVector3 V = GetSize();

	if (V.x > V.y && V.x > V.z)
		return 0;
	else if (V.y > V.z)
		return 1;
	else return 2;
}

float MBoundingBox::GetMaxWidth() const
{
	TVector3 V = GetSize();

	if (V.x > V.y && V.x > V.z)
		return V.x;
	else if (V.y > V.z)
		return V.y;
	else return V.z;
}

float MBoundingBox::GetSurfaceArea() const
{
	if (bInit)
	{
		TVector3 V = GetSize();
		return 2.0f * (V.x * V.y + V.x * V.z + V.y * V.z);
	}
	else
	{
		return 0.0f;
	}
}

MBoundingBox MBoundingBox::Union(const MBoundingBox& BoxA, const MBoundingBox& BoxB)
{
	MBoundingBox UnionBox;

	if (BoxA.bInit || BoxB.bInit)
	{
		UnionBox.bInit = true;
		UnionBox.Min = TVector3::Min(BoxA.Min, BoxB.Min);
		UnionBox.Max = TVector3::Max(BoxA.Max, BoxB.Max);
	}

	return UnionBox;
}

MBoundingBox MBoundingBox::Union(const MBoundingBox& Box, const TVector3& Point)
{
	MBoundingBox UnionBox;

	UnionBox.bInit = true;
	UnionBox.Min = TVector3::Min(Box.Min, Point);
	UnionBox.Max = TVector3::Max(Box.Max, Point);

	return UnionBox;
}

MBoundingBox MBoundingBox::Transform(const TTransform& T)
{
	MBoundingBox Box;

	if (bInit)
	{
		Box.bInit = true;

		TMatrix M = T.GetTransformMatrix();

		Box = Union(Box, M.Transform(TVector3(Min.x, Min.y, Min.z)));
		Box = Union(Box, M.Transform(TVector3(Max.x, Min.y, Min.z)));
		Box = Union(Box, M.Transform(TVector3(Min.x, Max.y, Min.z)));
		Box = Union(Box, M.Transform(TVector3(Min.x, Min.y, Max.z)));

		Box = Union(Box, M.Transform(TVector3(Max.x, Max.y, Max.z)));
		Box = Union(Box, M.Transform(TVector3(Min.x, Max.y, Max.z)));
		Box = Union(Box, M.Transform(TVector3(Max.x, Min.y, Max.z)));
		Box = Union(Box, M.Transform(TVector3(Max.x, Max.y, Min.z)));
	}

	return Box;
}

bool MBoundingBox::Intersect(const MRay& Ray, float& Dist0, float& Dist1)
{
	float t0 = 0, t1 = Ray.MaxDist;

	for (int i = 0; i < 3; i++)
	{
		float InvRayDir = 1.0f / Ray.Direction[i];
		float tNear = (Min[i] - Ray.Origin[i]) * InvRayDir;
		float tFar = (Max[i] - Ray.Origin[i]) * InvRayDir;

		if (tNear > tFar) std::swap(tNear, tFar);

		tFar *= 1 + 2 * TMath::gamma(3);
		t0 = tNear > t0 ? tNear: t0;
		t1 = tFar < t1 ? tFar : t1;
		if (t0 > t1) return false;
	}

	Dist0 = t0;
	Dist1 = t1;

	return true;
}
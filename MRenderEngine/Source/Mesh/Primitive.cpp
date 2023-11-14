#include "Primitive.h"
#include "MeshRepository.h"

bool MPrimitive::GetLocalBoundingBox(MBoundingBox& OutBox) const
{
	if (BoundingBox.bInit)
	{
		OutBox = BoundingBox;
		return true;
	}
	else return false;
}

bool MPrimitive::GetWorldBoundingBox(MBoundingBox& OutBox) const
{
	MBoundingBox LocalBox;
	
	if (GetLocalBoundingBox(LocalBox))
	{
		OutBox = LocalBox.Transform(WorldTansform);
		return true;
	}
	else return false;
}

void MLine::GenerateBoundingBox()
{
	std::vector<TVector3> Points;
	Points.push_back(Point1);
	Points.push_back(Point2);
	BoundingBox.Init(Points);
}

void MTriangle::GenerateBoundingBox()
{
	std::vector<TVector3> Points;
	Points.push_back(PointA);
	Points.push_back(PointB);
	Points.push_back(PointC);
	BoundingBox.Init(Points);
}

bool MTriangle::Intersect(const MRay& Ray, float& Dist, bool& bBackFace)
{
	const float EPSILON = 0.000001f;

	TVector3 Dir = Ray.Direction;
	TVector3 Origin = Ray.Origin;

	TVector3 Edge1 = PointB - PointA;
	TVector3 Edge2 = PointC - PointA;

	TVector3 PVec = Dir.Cross(Edge2);

	float Det = Edge1.Dot(PVec);

	if (Det > -EPSILON && Det < EPSILON)
	{
		return false;
	}

	float InvDet = 1.0f / Det;

	TVector3 TVec = Origin - PointA;

	float U = TVec.Dot(PVec) * InvDet;
	if (U < 0.0f || U > 1.0f)
	{
		return false;
	}

	TVector3 QVec = TVec.Cross(Edge1);

	float V = Dir.Dot(QVec) * InvDet;
	if (V < 0.0f || U + V > 1.0f)
	{
		return false;
	}

	float T = Edge2.Dot(QVec) * InvDet;
	if (T < 0.0f)
	{
		return false;
	}

	float TValue = std::abs(T);
	if (TValue > Ray.MaxDist)
	{
		return false;
	}

	Dist = TValue;
	bBackFace = Det < 0.0f ? true : false;

	return true;
}

void MMeshPrimitive::GenerateBoundingBox()
{
	MMesh& Mesh = MMeshRepository::Get().MeshMap.at(MeshName);
	BoundingBox = Mesh.GetBoundingBox();
}
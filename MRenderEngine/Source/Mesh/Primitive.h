#pragma once

#include <string>
#include "Color.h"
#include "BoundingBox.h"
#include "Ray.h"

class MPrimitive
{
public:
	MPrimitive() {}

	virtual ~MPrimitive() {}

	virtual void GenerateBoundingBox() {}

	bool GetLocalBoundingBox(MBoundingBox& OutBox) const;

	bool GetWorldBoundingBox(MBoundingBox& OutBox) const;

	virtual bool Intersect(const MRay& Ray, float& Dist, bool& bBackFace) { return false; }

protected:
	TTransform WorldTansform;

	MBoundingBox BoundingBox;
};

class MPoint : public MPrimitive
{
public:
	MPoint() = default;

	MPoint(const TVector3& IPoint, const MColor& IColor)
		:Point(IPoint),Color(IColor)
	{}

public:
	TVector3 Point;

	MColor Color;
};

class MLine : public MPrimitive
{
public:
	MLine() = default;

	MLine(const TVector3& IPoint1, const TVector3& IPoint2, const MColor& IColor)
		:Point1(IPoint1),Point2(IPoint2),Color(IColor)
	{}

	virtual void GenerateBoundingBox() override;

public:
	TVector3 Point1;
	TVector3 Point2;

	MColor Color;
};

class MTriangle : public MPrimitive
{
public:
	MTriangle() = default;

	MTriangle(const TVector3& IPointA, const TVector3& IPointB, const TVector3& IPointC, const MColor& IColor)
		:PointA(IPointA),PointB(IPointB),PointC(IPointC),Color(IColor)
	{}

	MTriangle(const MTriangle& rhs) = default;

	virtual void GenerateBoundingBox() override;

	virtual bool Intersect(const MRay& Ray, float& Dist, bool& bBackFace) override;

public:
	TVector3 PointA;
	TVector3 PointB;
	TVector3 PointC;

	MColor Color;
};

class MMeshPrimitive : public MPrimitive
{
public:
	MMeshPrimitive(const std::string& IMeshName, const TTransform& IWorldTransform)
		:MeshName(IMeshName)
	{
		WorldTansform = IWorldTransform;
	}

	virtual void GenerateBoundingBox() override;

private:
	std::string MeshName;
};
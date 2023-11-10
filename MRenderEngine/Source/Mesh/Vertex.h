#pragma once

#include "Color.h"
#include "Math/Math.h"

struct MVertex
{
	MVertex() {}

	MVertex(
		const TVector3& p,
		const TVector3& n,
		const TVector3& t,
		const TVector3& uv):
		Position(p),
		Normal(n),
		TangentU(t),
		TexC(uv)
	{}

	MVertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v):
		Position(px, py, pz),
		Normal(nx, ny, nz),
		TangentU(tx, ty, tz),
		TexC(u, v)
	{}

	TVector3 Position;
	TVector3 Normal;
	TVector3 TangentU;
	TVector2 TexC;
};

struct MPrimitiveVertex
{
	MPrimitiveVertex() {}

	MPrimitiveVertex(
		const TVector3& p,
		const MColor& c):
		Position(p),
		Color(c)
	{}

	MPrimitiveVertex(
		float px, float py, float pz,
		float cr, float cg, float cb, float ca):
		Position(px, py, pz),
		Color(cr, cg, cb, ca)
	{}

	TVector3 Position;
	MColor Color;
};

struct MSpriteVertex
{
	MSpriteVertex() {}

	MSpriteVertex(
		const TVector3& p,
		const TVector2& uv):
		Position(p),
		TexC(uv)
	{}

	TVector3 Position;
	TVector2 TexC;
};
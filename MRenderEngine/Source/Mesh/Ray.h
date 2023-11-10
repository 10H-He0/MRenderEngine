#pragma once

#include "Math/Math.h"

class MRay
{
public:
	MRay()
		:MaxDist(TMath::Infinity)
	{}

	MRay(const TVector3& IOrigin, const TVector3& IDirection, float IMaxDist = TMath::Infinity)
		:Origin(IOrigin), Direction(IDirection), MaxDist(IMaxDist)
	{}

public:
	TVector3 Origin;
	
	TVector3 Direction;

	mutable float MaxDist;
};
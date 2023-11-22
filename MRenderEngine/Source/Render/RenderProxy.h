#pragma once

#include <string>
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12View.h"
#include "Math/Math.h"

struct MMaterialConstants
{
public:
	TVector4 DiffuseAlbedo;
	TVector3 FresnelR0;
	float Roughness;

	TMatrix MatTransform = TMatrix::Identity;

	TVector3 EmissiveColor;
	UINT ShadingModle;
};
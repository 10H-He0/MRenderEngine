#include "CameraComponent.h"

MCameraComponent::MCameraComponent()
{
	SetLens(0.25f * TMath::Pi, 1.0f, 0.1f, 100.0f);
}

MCameraComponent::~MCameraComponent()
{

}

void MCameraComponent::SetWorldLocation(const TVector3& Location)
{
	WorldTransform.Location = Location;
	ViewDirty = true;
}

float MCameraComponent::GetNearZ() const
{
	return NearZ;
}

float MCameraComponent::GetFarZ() const
{
	return FarZ;
}

float MCameraComponent::GetAspect() const
{
	return Aspect;
}

float MCameraComponent::GetFovY() const
{
	return FovY;
}

float MCameraComponent::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return float(2.0f * atan(halfWidth / NearZ));
}

float MCameraComponent::GetNearWindowWidth() const
{
	return Aspect * NearWindowHeight;
}

float MCameraComponent::GetNearWindowHeight() const
{
	return NearWindowHeight;
}

float MCameraComponent::GetFarWindowWidth() const
{
	return Aspect * FarWindowHeight;
}

float MCameraComponent::GetFarWindowHeight() const
{
	return FarWindowHeight;
}

void MCameraComponent::SetLens(float fovY, float aspect, float nearZ, float farZ)
{
	FovY = fovY;
	Aspect = aspect;
	NearZ = nearZ;
	FarZ = farZ;

	NearWindowHeight = 2.0f * NearZ * tanf(0.5f * FovY);
	FarWindowHeight = 2.0f * FarZ * tanf(0.5f * FovY);

	Proj = TMatrix::CreatePerspectiveFieldOfView(fovY, aspect, nearZ, farZ);
}

void MCameraComponent::LookAt(const TVector3& pos, const TVector3& target, const TVector3& up)
{
	TVector3 L = target - pos;
	L.Normalize();
	TVector3 R = up.Cross(L);
	R.Normalize();
	TVector3 U = L.Cross(R);

	WorldTransform.Location = pos;
	Look = L;
	Right = R;
	Up = U;

	ViewDirty = true;
}

TMatrix MCameraComponent::GetView() const
{
	assert(!ViewDirty);
	return View;
}

TMatrix MCameraComponent::GetProj() const
{
	return Proj;
}

void MCameraComponent::MoveRight(float Dist)
{
	WorldTransform.Location += Right * Dist;

	ViewDirty = true;
}

void MCameraComponent::MoveForward(float Dist)
{
	WorldTransform.Location += Look * Dist;

	ViewDirty = true;
}

void MCameraComponent::MoveUp(float Dist)
{
	WorldTransform.Location += Up * Dist;

	ViewDirty = true;
}

void MCameraComponent::Pitch(float Degrees)
{
	float Radians = TMath::DegreesToRadians(Degrees);

	TMatrix R = TMatrix::CreateFromAxisAngle(Right, Radians);

	Up = R.TransformNormal(Up);
	Look = R.TransformNormal(Look);

	ViewDirty = true;
}

void MCameraComponent::RotateY(float Degrees)
{
	float Radians = TMath::DegreesToRadians(Degrees);

	TMatrix R = TMatrix::CreateRotationY(Radians);

	Right = R.TransformNormal(Right);
	Up = R.TransformNormal(Up);
	Look = R.TransformNormal(Look);

	ViewDirty = true;
}

void MCameraComponent::UpdateViewMatrix()
{
	if (ViewDirty)
	{
		// Keep camera's axes orthogonal to each other and of unit length.
		Look.Normalize();
		Up = Look.Cross(Right);
		Up.Normalize();

		// Up, Look already ortho-normal, so no need to normalize cross product.
		Right = Up.Cross(Look);

		// Fill in the view matrix entries.
		float x = -WorldTransform.Location.Dot(Right);
		float y = -WorldTransform.Location.Dot(Up);
		float z = -WorldTransform.Location.Dot(Look);

		View(0, 0) = Right.x;
		View(1, 0) = Right.y;
		View(2, 0) = Right.z;
		View(3, 0) = x;

		View(0, 1) = Up.x;
		View(1, 1) = Up.y;
		View(2, 1) = Up.z;
		View(3, 1) = y;

		View(0, 2) = Look.x;
		View(1, 2) = Look.y;
		View(2, 2) = Look.z;
		View(3, 2) = z;

		View(0, 3) = 0.0f;
		View(1, 3) = 0.0f;
		View(2, 3) = 0.0f;
		View(3, 3) = 1.0f;

		ViewDirty = false;
	}
}

void MCameraComponent::SetPrevViewProj(const TMatrix& VP)
{
	PrevViewProj = VP;
}

TMatrix MCameraComponent::GetPrevViewProj() const
{
	return PrevViewProj;
}
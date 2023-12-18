#pragma once

#include "Component.h"

class MCameraComponent : public MComponent
{
public:
	MCameraComponent();

	~MCameraComponent();

	virtual void SetWorldLocation(const TVector3& Location) override;

	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	void SetLens(float fovY, float aspect, float nearZ, float farZ);

	void LookAt(const TVector3& pos, const TVector3& target, const TVector3& up);

	TMatrix GetView() const;
	TMatrix GetProj() const;

	void MoveRight(float Dist);
	void MoveForward(float Dist);
	void MoveUp(float Dist);

	void Pitch(float Degrees);
	void RotateY(float Degrees);

	void UpdateViewMatrix();

	void SetPrevViewProj(const TMatrix& VP);

	TMatrix GetPrevViewProj() const;

private:
	TVector3 Right = { 1.0f, 0.0f, 0.0f };
	TVector3 Up = { 0.0f, 1.0f, 0.0f };
	TVector3 Look = { 0.0f, 0.0f, 1.0f };

	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float Aspect = 0.0f;
	float FovY = 0.0f;
	float NearWindowHeight = 0.0f;
	float FarWindowHeight = 0.0f;

	bool ViewDirty = true;

	TMatrix View = TMatrix::Identity;
	TMatrix Proj = TMatrix::Identity;

	TMatrix PrevViewProj = TMatrix::Identity;
};
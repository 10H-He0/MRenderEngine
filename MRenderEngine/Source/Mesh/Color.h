#pragma once


struct MColor
{
public:
	MColor() = default;

	MColor(float Value)
		:R(Value),G(Value),B(Value),A(Value)
	{

	}

	MColor(float IR, float IG, float IB, float IA = 1.0f)
		:R(IR),G(IG),B(IB),A(IA)
	{

	}

public:
	static const MColor Black;
	static const MColor White;
	static const MColor Red;
	static const MColor Green;
	static const MColor Blue;
	static const MColor Yellow;
	static const MColor Cyan;
	static const MColor Magenta;

public:
	float R = 0;
	float G = 0;
	float B = 0;
	float A = 0;
};
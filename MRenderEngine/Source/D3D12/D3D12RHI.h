#pragma once

#include "D3D12Device.h"
#include "Texture/TextureInfo.h"
#include "Math/Math.h"

class MD3D12RHI
{
public:
	MD3D12RHI();

	~MD3D12RHI();

public:
	MD3D12Device* GetDevice() { return Device.get(); }

	IDXGIFactory4* GetDXGIFactory();

private:
	std::unique_ptr<MD3D12Device> Device = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory4>DXGIFactory = nullptr;
};
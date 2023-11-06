#pragma once

#include "D3D12/D3D12Utils.h"
#include "InputLayout.h"
#include "Shader/Shader.h"
#include <unordered_map>

class MD3D12RHI;

struct MGraphicsPSODescriptor
{
	bool operator==(const MGraphicsPSODescriptor& rhs)const
	{
		return rhs.InputLayoutName == InputLayoutName
			&& rhs.Shader == Shader
			&& rhs.PrimitiveTopologyType == PrimitiveTopologyType
			&& rhs.RasterizerDesc.CullMode == RasterizerDesc.CullMode
			&& rhs.DepthStencilDesc.DepthFunc == DepthStencilDesc.DepthFunc;
	}

public:
	std::string InputLayoutName;
	MShader* Shader = nullptr;
	DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	bool _4xMSAAState = false;
	UINT _4xMSAAQuality = 0;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D12_RASTERIZER_DESC RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	UINT NumRenderTargets = 1;
};

namespace std
{
	template<>
	struct hash<MGraphicsPSODescriptor>
	{
		std::size_t operator()(const MGraphicsPSODescriptor& rhs)const
		{
			using std::hash;
			using std::string;

			return (hash<string>()(rhs.InputLayoutName) ^ (hash<void*>()(rhs.Shader) << 1));
		}
	};
}

class MGraphicsPSOManager
{
public:
	MGraphicsPSOManager(MD3D12RHI* ID3D12RHI, MInputLayoutManager* IInputLayoutManager);

	void TryCreatePSO(const MGraphicsPSODescriptor& Descriptor);

	ID3D12PipelineState* GetPSO(const MGraphicsPSODescriptor& Descriptor)const;

private:
	void CreatePSO(const MGraphicsPSODescriptor& Descriptor);

private:
	MD3D12RHI* D3D12RHI = nullptr;

	MInputLayoutManager* InputLayoutManager = nullptr;

	std::unordered_map<MGraphicsPSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOMap;
};

struct MComputePSODescriptor
{
	bool operator==(const MComputePSODescriptor& rhs)const
	{
		return rhs.Shader == Shader
			&& rhs.Flag == Flag;
	}

public:
	MShader* Shader = nullptr;
	D3D12_PIPELINE_STATE_FLAGS Flag = D3D12_PIPELINE_STATE_FLAG_NONE;
};

namespace std
{
	template<>
	struct hash<MComputePSODescriptor>
	{
		std::size_t operator()(const MComputePSODescriptor& rhs)const
		{
			using std::hash;
			using std::string;

			return (hash<void*>()(rhs.Shader) ^ (hash<int>()(rhs.Flag) << 1));
		}
	};
}

class MComputePSOManager
{
public:
	MComputePSOManager(MD3D12RHI* ID3D12RHI);

	void TryCreatePSO(const MComputePSODescriptor& Descriptor);

	ID3D12PipelineState* GetPSO(const MComputePSODescriptor& Descriptor)const;

private:
	void CreatePSO(const MComputePSODescriptor& Descriptor);

private:
	MD3D12RHI* D3D12RHI = nullptr;

	std::unordered_map<MComputePSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOMap;
};
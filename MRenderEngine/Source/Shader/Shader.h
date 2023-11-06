#pragma once

#include <unordered_map>
#include <wrl/client.h>
#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12RHI.h"

using Microsoft::WRL::ComPtr;

enum class EShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
};

struct MShaderDefine
{
public:
	void GetD3DShaderMacro(std::vector<D3D_SHADER_MACRO>& OutMacros) const;

	bool operator == (const MShaderDefine& rhs) const;

	void SetDefine(const std::string& Name, const std::string& Definition);

public:
	std::unordered_map<std::string, std::string> DefineMap;
};

namespace std
{
	template<>
	struct hash<MShaderDefine>
	{
		std::size_t operator()(const MShaderDefine& Define) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			size_t HashValue = 0;
			for (const auto& Pair : Define.DefineMap)
			{
				HashValue ^= (hash<string>()(Pair.first) << 1);
				HashValue ^= (hash<string>()(Pair.second) << 1);
			}

			return HashValue;
		}
	};
}

struct MShaderParameter
{
	std::string Name;
	EShaderType ShaderType;
	UINT BindPoint;
	UINT RegisterSpace;
};

struct MShaderCBVParameter : MShaderParameter
{
	MD3D12ConstantBufferRef ConstantBufferRef;
};

struct MShaderSRVParameter : MShaderParameter
{
	UINT BindCount;

	std::vector<MD3D12ShaderResourceView*> SRVList;
};

struct MShaderUAVParameter : MShaderParameter
{
	UINT BindCount;

	std::vector<MD3D12UnorderedAccessView*> UAVList;
};

struct MShaderSamplerParameter : MShaderParameter
{

};

struct MShaderInfo
{
	std::string ShaderName;

	std::string FileName;

	MShaderDefine shaderDefine;

	bool bCreateVS = false;

	std::string VSEntryPoint = "VS";

	bool bCreatePS = false;

	std::string PSEntryPoint = "PS";

	bool bCreateCS = false;

	std::string CSEntryPoint = "CS";
};

class MShader
{
public:
	MShader(const MShaderInfo& IShaderInfo, MD3D12RHI* IRHI);

	void Initialize();

	bool SetParameter(std::string ParamName, MD3D12ConstantBufferRef ConstantBufferRef);

	bool SetParameter(std::string ParamName, MD3D12ShaderResourceView* SRV);

	bool SetParameter(std::string ParamName, const std::vector<MD3D12ShaderResourceView*>& SRVList);

	bool SetParameter(std::string ParamName, MD3D12UnorderedAccessView* UAV);

	bool SetParameter(std::string ParamName, const std::vector<MD3D12UnorderedAccessView*>& UAVList);

	void BindParams();

private:
	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& FileName, const D3D_SHADER_MACRO* Defines, const std::string& EntryPoint, const std::string& Target);

	void GetShaderParameters(ComPtr<ID3DBlob> PassBlob, EShaderType ShaderType);

	D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderType ShaderType);

	std::vector<CD3DX12_STATIC_SAMPLER_DESC> CreateStaticSamplers();

	void CreateRootSignatures();

	void CheckBindings();

	void ClearBindings();

public:
	MShaderInfo ShaderInfo;

	std::vector<MShaderCBVParameter> CBVParams;

	std::vector<MShaderSRVParameter> SRVParams;

	std::vector<MShaderUAVParameter> UAVParams;

	std::vector<MShaderSamplerParameter> SamplerParams;

	int CBVSignatureBaseBindSlot = -1;

	int SRVSignatureBindSlot = -1;

	UINT SRVCount = 0;

	int UAVSignatureBindSlot = -1;

	UINT UAVCount = 0;

	int SamplerSignatureBindPoint = -1;

	std::unordered_map<std::string, ComPtr<ID3DBlob>>ShaderPass;

	ComPtr<ID3D12RootSignature> RootSignature;

private:
	MD3D12RHI* D3D12RHI = nullptr;
};


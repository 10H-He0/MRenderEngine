#include "PSO.h"
#include "Shader/Shader.h"
#include "D3D12/D3D12RHI.h"

MGraphicsPSOManager::MGraphicsPSOManager(MD3D12RHI* ID3D12RHI, MInputLayoutManager* IInputLayoutManager)
	:D3D12RHI(ID3D12RHI), InputLayoutManager(IInputLayoutManager)
{

}

void MGraphicsPSOManager::TryCreatePSO(const MGraphicsPSODescriptor& Descriptor)
{
	if (PSOMap.find(Descriptor) == PSOMap.end())
	{
		CreatePSO(Descriptor);
	}
}

void MGraphicsPSOManager::CreatePSO(const MGraphicsPSODescriptor& Descriptor)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
	InputLayoutManager->GetInputLayout(Descriptor.InputLayoutName, InputLayout);
	PSODesc.InputLayout = { InputLayout.data(), (UINT)InputLayout.size() };

	MShader* Shader = Descriptor.Shader;
	auto RootSignature = Shader->RootSignature;
	PSODesc.pRootSignature = RootSignature.Get();
	PSODesc.VS = CD3DX12_SHADER_BYTECODE(Shader->ShaderPass.at("VS")->GetBufferPointer(), Shader->ShaderPass.at("VS")->GetBufferSize());
	PSODesc.PS = CD3DX12_SHADER_BYTECODE(Shader->ShaderPass.at("PS")->GetBufferPointer(), Shader->ShaderPass.at("PS")->GetBufferSize());

	PSODesc.RasterizerState = Descriptor.RasterizerDesc;
	PSODesc.BlendState = Descriptor.BlendDesc;
	PSODesc.DepthStencilState = Descriptor.DepthStencilDesc;
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.PrimitiveTopologyType = Descriptor.PrimitiveTopologyType;
	PSODesc.NumRenderTargets = Descriptor.NumRenderTargets;
	for (int i = 0; i < 8; i++)
	{
		PSODesc.RTVFormats[i] = Descriptor.RTVFormats[i];
	}
	PSODesc.SampleDesc.Count = Descriptor._4xMSAAState ? 4 : 1;
	PSODesc.SampleDesc.Quality = Descriptor._4xMSAAQuality ? (Descriptor._4xMSAAQuality - 1) : 0;
	PSODesc.DSVFormat = Descriptor.DepthStencilFormat;

	ComPtr<ID3D12PipelineState> PSO;
	auto D3DDevice = D3D12RHI->GetDevice()->GetD3DDevice();
	ThrowIfFailed(D3DDevice->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&PSO)));
	PSOMap.insert({ Descriptor, PSO });
}

ID3D12PipelineState* MGraphicsPSOManager::GetPSO(const MGraphicsPSODescriptor& Descriptor)const
{
	auto It = PSOMap.find(Descriptor);

	if (It == PSOMap.end())
	{
		assert(0);
		return nullptr;
	}
	else
	{
		return It->second.Get();
	}
}

MComputePSOManager::MComputePSOManager(MD3D12RHI* ID3D12RHI)
	:D3D12RHI(ID3D12RHI)
{

}

void MComputePSOManager::TryCreatePSO(const MComputePSODescriptor& Descriptor)
{
	if (PSOMap.find(Descriptor) == PSOMap.end())
	{
		CreatePSO(Descriptor);
	}
}

void MComputePSOManager::CreatePSO(const MComputePSODescriptor& Descriptor)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC PSODesc = {};

	MShader* Shader = Descriptor.Shader;
	PSODesc.pRootSignature = Shader->RootSignature.Get();
	PSODesc.CS = CD3DX12_SHADER_BYTECODE(Shader->ShaderPass.at("CS")->GetBufferPointer(), Shader->ShaderPass.at("CS")->GetBufferSize());
	PSODesc.Flags = Descriptor.Flag;

	ComPtr<ID3D12PipelineState>PSO;
	auto D3DDevice = D3D12RHI->GetDevice()->GetD3DDevice();
	ThrowIfFailed(D3DDevice->CreateComputePipelineState(&PSODesc, IID_PPV_ARGS(&PSO)));
	PSOMap.insert({ Descriptor, PSO });
}

ID3D12PipelineState* MComputePSOManager::GetPSO(const MComputePSODescriptor& Descriptor)const
{
	auto It = PSOMap.find(Descriptor);

	if (It == PSOMap.end())
	{
		assert(0);
		return nullptr;
	}
	else
	{
		return It->second.Get();
	}
}
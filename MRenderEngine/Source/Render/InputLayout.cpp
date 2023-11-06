#include "InputLayout.h"

void MInputLayoutManager::AddInputLayout(const std::string& Name, const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout)
{
	InputLayoutMap.insert({ Name, InputLayout });
}

void MInputLayoutManager::GetInputLayout(const std::string Name, std::vector<D3D12_INPUT_ELEMENT_DESC>& OutInputLayout)const
{
	auto It = InputLayoutMap.find(Name);

	if (It == InputLayoutMap.end())
	{
		assert(0);
	}
	else
	{
		OutInputLayout = It->second;
	}
}
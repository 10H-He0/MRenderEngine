#include "MaterialInstance.h"
#include "Render/RenderProxy.h"
#include "D3D12/D3D12RHI.h"

MMaterialInstance::MMaterialInstance(MMaterial* IMaterial, const std::string& IName)
	:Material(IMaterial),Name(IName)
{
	Parameters = Material->Parameters;
}

void MMaterialInstance::SetTextureParameter(const std::string& Parmeter, const std::string& TextureName)
{

}
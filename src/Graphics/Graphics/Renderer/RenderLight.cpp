#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"

namespace luna::render
{


void DirectionLight::Init()
{
	mParamBuffer = std::make_shared<ShaderParamsBuffer>(sRenderModule->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash()));
}


}
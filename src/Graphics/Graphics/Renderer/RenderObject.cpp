#include "Graphics/Renderer/RenderObject.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/Asset/MaterialTemplate.h"
namespace luna::graphics
{
	RenderObject::RenderObject(RenderScene* ownerScene) : mOwnerScene(ownerScene)
	{
		mWorldMat.setIdentity();
	}

	PARAM_ID(RoWorldMatrixBuffer);
	void RenderObjectGpuSceneDataBuffer::SetMaterialParameter(MaterialInstance* matInstance)
	{
		matInstance->SetShaderInput(ParamID_RoWorldMatrixBuffer, mRoDataBufferView);
	}
}
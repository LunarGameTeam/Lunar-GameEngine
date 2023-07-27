#include "Graphics/Renderer/RenderMesh.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/RenderModule.h"

namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);

void RenderObjectDrawData::DrawRenderObjects(MeshRenderPass pass, const std::unordered_map<luna::graphics::ShaderParamID, luna::graphics::RHIView*>& shaderBindingParam, RHIView* temp)
{
	for (int32_t visibleRoIndex = 0; visibleRoIndex < mVisibleROs[pass].size(); ++visibleRoIndex)
	{
		RenderObject* drawRenderObject = mVisibleROs[pass][visibleRoIndex];
		MaterialInstance* matInstance = nullptr;
		matInstance = mOverrideMaterialInst[pass];
		if (!matInstance)
			matInstance = drawRenderObject->mMaterial;
		RenderMeshBase* renderMeshData = drawRenderObject->mMeshIndex;
		RHIResource* instancingBuffer = mScene->mROIDInstancingBuffer->mRes;
		if (temp == nullptr)
			temp = mView->mViewBuffer->mView;
		if (temp)
		{
			matInstance->SetShaderInput(ParamID_SceneBuffer, mScene->mSceneParamsBuffer->mView);
			matInstance->SetShaderInput(ParamID_ViewBuffer, temp);
		}
		for (auto& eachShaderParam : shaderBindingParam)
		{
			matInstance->SetShaderInput(eachShaderParam.first, eachShaderParam.second);
		}
		sRenderModule->mRenderContext->DrawMeshInstanced(renderMeshData, matInstance, nullptr, instancingBuffer, drawRenderObject->mID, 1);		
	}
}

void RenderObjectDrawData::PerViewUpdate(RenderView* renderView)
{
	auto scene = renderView->mOwnerScene;
	mView = renderView;
	mScene = scene;

	for(size_t eachPassIndex =0; eachPassIndex < MeshRenderPass::AllNum; ++eachPassIndex)
	{
		for(auto it : mScene->GetDirtyRos())
		{
			if(mFilters[eachPassIndex])
			{
				if(mFilters[eachPassIndex](it))
					mVisibleROs[eachPassIndex].push_back(it);
			}
			else
			{
				//TODO Remove
			}
		}
	}
	mScene->GetDirtyRos().clear();
}
}

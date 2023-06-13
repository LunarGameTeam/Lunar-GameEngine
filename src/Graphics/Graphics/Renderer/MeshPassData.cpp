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
namespace luna::render
{
PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);

void RenderMeshBase::Init(SubMesh* meshData)
{
	if (meshData->mVertexData.size() == 0 || meshData->mIndexData.size() == 0)
		return;
	RHIBufferDesc desc;
	desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;
	mVertexSize = meshData->mVertexData.size();
	mVeretexLayout = meshData->GetVertexLayout();
	if (meshData->mType == SubMeshType::SubMeshSkined)
	{
		SubMeshSkeletal* meshSkeletalData = static_cast<SubMeshSkeletal*>(meshData);
		LArray<SkinRenderVertex> combineData;
		for (int32_t vertId = 0; vertId < mVertexSize; ++vertId)
		{
			combineData.emplace_back();
			combineData.back().mBaseValue = meshSkeletalData->mVertexData[vertId];
			combineData.back().mSkinValue = meshSkeletalData->mSkinData[vertId];
		}
		desc.mSize = sizeof(SkinRenderVertex) * mVertexSize;
		mVB = sRenderModule->mRenderContext->CreateBuffer(desc, combineData.data());
	}
	else
	{
		desc.mSize = sizeof(BaseVertex) * mVertexSize;
		mVB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mVertexData.data());
	}
	mIndexSize = meshData->mIndexData.size();
	desc.mSize = sizeof(uint32_t) * mIndexSize;
	desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
	mIB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mIndexData.data());
}

size_t PerPassDrawCmds::AddCommand(
	RenderObject* renderObject,
	MaterialInstance* materialInstance
)
{
	auto ro_command_exist = mRoCommandIndex.find(renderObject->mID);
	size_t command_id = -1;
	if (ro_command_exist == mRoCommandIndex.end())
	{
		if (mEmptyId.empty())
		{
			command_id = mAllCommands.size();
		}
		else
		{
			command_id = mEmptyId.front();
			mEmptyId.pop();
		}
		MeshDrawCommand newCommand;
		mAllCommands.insert({ command_id ,newCommand });
	}
	else
	{
		command_id = ro_command_exist->second;
	}
	mAllCommands[command_id].mMaterialInstance = materialInstance;
	mAllCommands[command_id].mRenderObjectId = renderObject->mID;

	//mAllCommands[command_id].mCanBatchHash = materialAsset->GetInstanceID();
	return command_id;
}

void MeshPassDrawData::DrawMeshs(MeshRenderPass pass, const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*>& shaderBindingParam, RHIView* temp)
{
	for (int32_t visibleRoIndex = 0; visibleRoIndex < allVisibleCommandsRef[pass].size(); ++visibleRoIndex)
	{
		luna::render::RenderObject* drawRenderObject = mScene->GetRenderObjects().find(allVisibleCommandsRef[pass][visibleRoIndex]->mRenderObjectId)->second;
		int32_t mesh_id = drawRenderObject->mMeshIndex;
		RenderMeshBase* renderMeshData = mScene->GetData<AssetSceneData>()->GetMeshData(mesh_id);
		RHIResource* instancingBuffer = mScene->mROIDInstancingBuffer->mRes;
		if (temp == nullptr)
			temp = mView->mViewBuffer->mView;

		if (temp)
		{
			allVisibleCommandsRef[pass][visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_SceneBuffer, mScene->mSceneParamsBuffer->mView);
			allVisibleCommandsRef[pass][visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_ViewBuffer, temp);
		}

		for (auto& eachShaderParam : shaderBindingParam)
		{
			allVisibleCommandsRef[pass][visibleRoIndex]->mMaterialInstance->SetShaderInput(eachShaderParam.first, eachShaderParam.second);
		}
		if(mOverrideMaterialInst[pass])
		{

			sRenderModule->mRenderContext->DrawMeshInstanced(renderMeshData, mOverrideMaterialInst[pass], nullptr, instancingBuffer, drawRenderObject->mID, 1);
		}
		else
		{
			sRenderModule->mRenderContext->DrawMeshInstanced(renderMeshData, allVisibleCommandsRef[pass][visibleRoIndex]->mMaterialInstance, nullptr, instancingBuffer, drawRenderObject->mID, 1);
		}
		
	}
}

void MeshPassDrawData::PerViewUpdate(RenderView* renderView)
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
					mAllMeshDrawCommands[eachPassIndex].AddCommand(it, it->mMaterial);
			}
			else
			{
				//TODO Remove
			}
		}
	}

	for(size_t eachPassIndex =0; eachPassIndex < MeshRenderPass::AllNum; ++eachPassIndex)
	{
		allVisibleCommandsRef[eachPassIndex].clear();
		for(auto& eachCommand : mAllMeshDrawCommands[eachPassIndex].GetCommands())
		{
			allVisibleCommandsRef[eachPassIndex].push_back(&eachCommand.second);
		}
	}

	mScene->GetDirtyRos().clear();
}
}

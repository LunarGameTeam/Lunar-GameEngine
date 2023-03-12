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
#include "Graphics/Asset/MaterialTemplate.h"
namespace luna::render
{
	PARAM_ID(SceneBuffer);
	PARAM_ID(ViewBuffer);
	void RenderMeshBase::Init(SubMesh* meshData)
	{
		if (meshData->mVertexData.size() == 0 || meshData->mIndexData.size() == 0)
			return;
		RHIBufferDesc desc;
		mVertexSize = meshData->mVertexData.size();
		desc.mSize = sizeof(BaseVertex) * mVertexSize;
		desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;

		mVB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mVertexData.data());
		mIndexSize = meshData->mIndexData.size();
		desc.mSize = sizeof(uint32_t) * mIndexSize;
		desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
		mIB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mIndexData.data());

		mVeretexLayout = BaseVertex::GetVertexLayout();
	}

	size_t MeshRenderCommandsPacket::AddCommand(
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
			MeshRenderCommand newCommand;
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

	void MeshRenderCommandsPassData::AddCommand(const MeshRenderCommand* data)
	{
		allVisibleCommandsRef.push_back(data);
	}
	
	void MeshRenderCommandsPassData::ClearCommand()
	{
		allVisibleCommandsRef.clear();
	}

	void MeshRenderCommandsPassData::DrawAllCommands(const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*>& shaderBindingParam)
	{
		for (int32_t visibleRoIndex = 0; visibleRoIndex < allVisibleCommandsRef.size(); ++visibleRoIndex)
		{
			luna::render::RenderObject* drawRenderObject = mScene->GetRenderObjects().find(allVisibleCommandsRef[visibleRoIndex]->mRenderObjectId)->second;
			int32_t mesh_id = drawRenderObject->mMeshIndex;
			RenderMeshBase* renderMeshData = mScene->mSceneDataGpu.GetMeshData(mesh_id);
			RHIResource* instancingBuffer = mScene->mROIDInstancingBuffer->mRes;

			allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_SceneBuffer, mScene->mSceneParamsBuffer->mView);
			allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_ViewBuffer, mView->mViewBuffer->mView);
			for (auto& eachShaderParam : shaderBindingParam)
			{
				allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(eachShaderParam.first, eachShaderParam.second);
			}
			sRenderModule->mRenderContext->DrawMeshInstanced(renderMeshData, allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance, nullptr, instancingBuffer, drawRenderObject->mID,1);
		}
	}

	void MeshRenderCommandsPassData::DrawAllCommands(RHIView* sceneViewParamBuffer, const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*>& shaderBindingParam)
	{
		for (int32_t visibleRoIndex = 0; visibleRoIndex < allVisibleCommandsRef.size(); ++visibleRoIndex)
		{
			luna::render::RenderObject* drawRenderObject = mScene->GetRenderObjects().find(allVisibleCommandsRef[visibleRoIndex]->mRenderObjectId)->second;
			int32_t mesh_id = drawRenderObject->mMeshIndex;
			RenderMeshBase* renderMeshData = mScene->mSceneDataGpu.GetMeshData(mesh_id);
			RHIResource* instancingBuffer = mScene->mROIDInstancingBuffer->mRes;

			allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_SceneBuffer, mScene->mSceneParamsBuffer->mView);
			allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(ParamID_ViewBuffer, sceneViewParamBuffer);
			for (auto& eachShaderParam : shaderBindingParam)
			{
				allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance->SetShaderInput(eachShaderParam.first, eachShaderParam.second);
			}
			sRenderModule->mRenderContext->DrawMeshInstanced(renderMeshData, allVisibleCommandsRef[visibleRoIndex]->mMaterialInstance, nullptr, instancingBuffer, drawRenderObject->mID, 1);
		}
	}
}
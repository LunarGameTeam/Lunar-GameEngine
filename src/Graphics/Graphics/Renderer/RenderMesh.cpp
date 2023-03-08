#include "Graphics/Renderer/RenderMesh.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"
namespace luna::render
{
	void RenderMeshBase::Init(SubMesh* meshData)
	{
		if (meshData->mVertexData.size() == 0 || meshData->mIndexData.size() == 0)
			return;
		RHIBufferDesc desc;
		desc.mSize = sizeof(BaseVertex) * meshData->mVertexData.size();
		desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;

		mVB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mVertexData.data());

		desc.mSize = sizeof(uint32_t) * meshData->mIndexData.size();
		desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
		mIB = sRenderModule->mRenderContext->CreateBuffer(desc, meshData->mIndexData.data());

		mVeretexLayout = BaseVertex::GetVertexLayout();
	}

	size_t MeshRenderCommandsPacket::AddCommand(
		RenderObject* renderObject,
		MaterialTemplateAsset* materialAsset
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
			newCommand.mMaterialInstanceIndex = command_id;
			mAllCommands.insert({ command_id ,newCommand });
		}
		else
		{
			command_id = ro_command_exist->second;
		}
		mAllCommands[command_id].mMaterialTemplate = materialAsset;
		mAllCommands[command_id].mRenderObjectId = renderObject->mID;

		mAllCommands[command_id].mCanBatchHash = materialAsset->GetInstanceID();
		return command_id;
	}

	size_t MeshRenderCommandsPacket::AddCommand(
		RenderObject* renderObject,
		const LString& materialAsset
	)
	{
		if (materialAsset == "")
		{
			return AddCommand(renderObject, nullptr);
		}
		return AddCommand(renderObject, sAssetModule->LoadAsset<MaterialTemplateAsset>(materialAsset));
	}

	void MeshRenderCommandsPassData::AddCommand(const MeshRenderCommand* data)
	{
		allVisibleCommandsRef.push_back(data);
	}
	
	void MeshRenderCommandsPassData::ClearCommand()
	{
		allVisibleCommandsRef.clear();
	}

	void MeshRenderCommandsPassData::DrawAllCommands(PackedParams* sceneAndViewParam)
	{
		LUnorderedMap<int32_t, LArray<int32_t>> batchList;
		for (int32_t visibleRoIndex = 0; visibleRoIndex < allVisibleCommandsRef.size(); ++visibleRoIndex)
		{
			batchList[allVisibleCommandsRef[visibleRoIndex]->mCanBatchHash].push_back(visibleRoIndex);
		}
		for (auto& eachBatch : batchList)
		{
			int32_t batchBegin = eachBatch.second[0];
			auto template_node = mScene->GetRenderObjects().find(allVisibleCommandsRef[batchBegin]->mRenderObjectId);
			int32_t mesh_id = template_node->second->mMeshIndex;
			//同一个shader，暂时认为vertexlayout一致
			RHIVertexLayout vertex_layout = mScene->mSceneDataGpu.GetMeshData(mesh_id)->GetVertexLayout();
			RHIPipelineStatePtr pipeline = sRenderModule->mRenderContext->CreatePipeline(template_node->second->mMaterial,&vertex_layout);
			auto bindingset = sRenderModule->mRenderContext->GetBindingSet(pipeline, params);
			sRenderModule->mRenderContext->mGraphicCmd->SetPipelineState(pipeline);
			//sRenderModule->mRenderContext->mGraphicCmd->BindDesriptorSetExt(bindingset);
			//接下来把相同材质的drawcommand继续按照模型是否相同进行instancing
			std::unordered_map<int32_t,LArray<RenderObject*>> meshDivideCommand;
			for (int32_t index_command = 0; index_command < eachBatch.second.size(); ++index_command)
			{
				int32_t batchDealId = eachBatch.second[index_command];
				auto roDealData = mScene->GetRenderObjects().find(allVisibleCommandsRef[batchDealId]->mRenderObjectId);
				meshDivideCommand[eachBatch.second[roDealData->second->mMeshIndex]].push_back(roDealData->second);
			}
			for (auto eachMeshBatch : meshDivideCommand)
			{
				eachMeshBatch.second.mater
			}
			
			//RHIVertexLayout layout = mesh->GetVertexLayout();
			//if (instanceMessage)
			//	layout.AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageInstanceMessage, 4, 1, VertexElementInstanceType::PerInstance);

			//auto pipeline = GetPipeline(mat, &layout);
			//auto bindingset = GetBindingSet(pipeline, params);

			//mGraphicCmd->SetPipelineState(pipeline);
			//mGraphicCmd->BindDesriptorSetExt(bindingset);

			//size_t vertexCount = mesh->mVertexData.size();

			//RHIResource* vb = mesh->mVB;
			//size_t indexCount = mesh->mIndexData.size();
			//RHIResource* ib = mesh->mIB;

			//LArray<RHIVertexBufferDesc> descs;
			//RHIVertexBufferDesc& vbDesc = descs.emplace_back();
			//vbDesc.mOffset = 0;
			//vbDesc.mBufferSize = vertexCount * mesh->GetStridePerVertex();
			//vbDesc.mVertexStride = mesh->GetStridePerVertex();
			//vbDesc.mVertexRes = mesh->mVB;

			//if (instanceMessage != nullptr)
			//{
			//	RHIVertexBufferDesc& vbInstancingDesc = descs.emplace_back();
			//	vbInstancingDesc.mOffset = 0;
			//	vbInstancingDesc.mBufferSize = instanceMessage->GetMemoryRequirements().size;
			//	vbInstancingDesc.mVertexStride = layout.GetSize()[1];
			//	vbInstancingDesc.mVertexRes = instanceMessage;
			//}

			//mGraphicCmd->SetVertexBuffer(descs, 0);
			//mGraphicCmd->SetIndexBuffer(ib);
			//switch (mat->mMaterialTemplate->GetPrimitiveType())
			//{
			//case RHIPrimitiveTopologyType::Triangle:
			//	mGraphicCmd->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
			//	break;
			//case RHIPrimitiveTopologyType::Line:
			//	mGraphicCmd->SetDrawPrimitiveTopology(RHIPrimitiveTopology::LineList);
			//	break;
			//default:
			//	break;
			//}
			//mGraphicCmd->DrawIndexedInstanced((uint32_t)indexCount, instancingSize, 0, 0, startInstanceIdx);
		}
		int a = 0;
	}
}
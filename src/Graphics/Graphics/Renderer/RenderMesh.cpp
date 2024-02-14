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
	void MeshRendererDataGenerateCommand(graphics::RenderScene* curScene, RenderMeshBridgeData& renderData, const LArray<SubMesh*>& initSubmesh)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRoDrawData = curScene->RequireData<RenderObjectDrawData>();
			for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderMesh.size(); ++subMeshIndex)
			{
				curScene->DestroyRenderObject(renderData.mRenderObjects[subMeshIndex]);
			}
			renderData.mRenderMesh.clear();
			renderData.mRenderObjects.clear();
			for (int32_t subMeshIndex = 0; subMeshIndex < initSubmesh.size(); ++subMeshIndex)
			{
				graphics::RenderObject* newObject = curScene->CreateRenderObject();
				RenderMeshBase* meshData = newObject->RequireData<RenderMeshBase>();
				meshData->mMeshData = initSubmesh[subMeshIndex]->GetRenderMeshData();
				renderData.mRenderMesh.push_back(meshData);
				renderData.mRenderObjects.push_back(newObject);
				renderData.mRoDrawData->AddDirtyRo(newObject);
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_GENERATE, commandFunc);
	}

	void MeshRendererMaterialGenerateCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		size_t subIndex,
		MaterialTemplateAsset* matAsset
	)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			renderData.mRenderMesh[subIndex]->mMaterial = dynamic_cast<MaterialInstance*>(matAsset->CreateInstance());
			renderData.mRenderMesh[subIndex]->mMaterial->Ready();
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void MeshRendererUpdateCastShadowCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool castShadow
	)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderMesh.size(); ++subMeshIndex)
			{
				renderData.mRenderMesh[subMeshIndex]->mCastShadow = castShadow;
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void MeshRendererUpdateReceiveShadowCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool receiveShadow
	)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderMesh.size(); ++subMeshIndex)
			{
				renderData.mRenderMesh[subMeshIndex]->mReceiveLight = receiveShadow;
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void MeshRendererUpdateReceiveLightCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool receiveLight
	)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderMesh.size(); ++subMeshIndex)
			{
				renderData.mRenderMesh[subMeshIndex]->mReceiveLight = receiveLight;
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}

	void MeshRendererUpdateReceiveLightCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		const LMatrix4f& mRoTransform
	)
	{
		std::function<void(void)> commandFunc = [=, &renderData]()->void {
			RenderObjectDrawData* RoDrawData = curScene->RequireData<RenderObjectDrawData>();
			for (int32_t subMeshIndex = 0; subMeshIndex < renderData.mRenderObjects.size(); ++subMeshIndex)
			{
				renderData.mRenderObjects[subMeshIndex]->UpdateWorldMatrix(mRoTransform);
				RoDrawData->AddDirtyRo(renderData.mRenderObjects[subMeshIndex]);
			}
		};
		curScene->GetRenderDataUpdater()->AddCommand(RenderDataUpdateCommandType::RENDER_DATA_UPDATE, commandFunc);
	}
}

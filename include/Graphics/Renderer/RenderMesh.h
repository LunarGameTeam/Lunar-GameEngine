#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/Renderer/MaterialInstance.h"


namespace luna::graphics
{
	class RenderAssetDataMesh;
	class RenderObjectDrawData;
	class SkeletonSkinData;

	class RenderMeshBase : public RenderData
	{
	public:
		//几何体数据
		RenderAssetDataMesh* mMeshData;
		//渲染参数
		bool              mCastShadow = true;
		bool              mReceiveLight = true;
		bool              mReceiveShadow = true;
		TPPtr<MaterialInstance> mMaterial;
		RenderMeshBase() :mMaterial(nullptr) 
		{
		}
	};

	struct RenderMeshBridgeData
	{
		RenderObjectDrawData* mRoDrawData;
		LArray<graphics::RenderObject*> mRenderObjects;
		LArray<RenderMeshBase*> mRenderMesh;
		LArray<SkeletonSkinData*> mSkinData;
	};

	void RENDER_API MeshRendererDataGenerateCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		const LArray<SubMesh*>& initSubmesh
	);

	void RENDER_API MeshRendererMaterialGenerateCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		size_t subIndex,
		MaterialTemplateAsset* matAsset
	);

	void RENDER_API MeshRendererUpdateCastShadowCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool castShadow
	);

	void RENDER_API MeshRendererUpdateReceiveShadowCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool receiveShadow
	);

	void RENDER_API MeshRendererUpdateReceiveLightCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		bool receiveLight
	);

	void RENDER_API MeshRendererUpdateReceiveLightCommand(
		graphics::RenderScene* curScene,
		RenderMeshBridgeData& renderData,
		const LMatrix4f& mRoTransform
	);

}

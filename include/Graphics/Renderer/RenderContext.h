#pragma once

#include "Core/CoreMin.h"
#include "Core/Foundation/MemoryHash.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIRenderPass.h"
#include "Graphics/RHI/RHICmdList.h"

#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RenderAssetManager/RenderAssetManager.h"
#include "Graphics/Renderer/MaterialInstance.h"
namespace luna::graphics
{

class RENDER_API RenderResourceContext final : NoCopy
{
	RenderAssetDataManager mRenderAssetManager;
public:
	RenderResourceContext() = default;
	virtual ~RenderResourceContext() = default;
	void Init();
	RenderAssetDataManager* GetAssetManager() { return &mRenderAssetManager; };
public:

	RHICBufferDesc GetDefaultShaderConstantBufferDesc(ShaderParamID name);

	SharedPtr<ShaderAsset> mDefaultShaderPbr;
	SharedPtr<LShaderInstance> mDefaultShaderVertexPbrInstance;
	SharedPtr<LShaderInstance> mDefaultShaderFragmentPbrInstance;
};

struct MeshDrawCommandHashKey
{
	RenderAssetDataMesh* mRenderMeshs = nullptr;
	MaterialInstanceGraphBase* mMtl = nullptr;
	bool operator==(const MeshDrawCommandHashKey& key2) const {
		if (mRenderMeshs->mID != key2.mRenderMeshs->mID)
		{
			return false;
		}
		if (mMtl != key2.mMtl)
		{
			return false;
		}
		return true;
	}
};

struct MeshDrawCommandBatch
{
	MeshDrawCommandHashKey mDrawParameter;
	size_t mRoOffset = 0;
	size_t mDrawCount = 0;
};

class RENDER_API RenderDrawContext
{
	RenderAssetDataMesh* mFullScreenRenderMesh = nullptr;
public:
	void Init();

	void BindDrawCommandPassDesc(RHICmdList* cmdList,const RenderPassDesc&);

	void EndRenderPass(RHICmdList* cmdList);

	
	void DrawMesh(RHICmdList* cmdList, graphics::RenderAssetDataMesh* mesh, graphics::MaterialInstanceGraphBase* mat);

	void DrawFullScreen(RHICmdList* cmdList, graphics::MaterialInstanceGraphBase* mat);

	void DrawMeshInstanced(
		RHICmdList* cmdList,
		RenderAssetDataMesh* mesh,
		MaterialInstanceGraphBase* mat,
		int32_t instancingSize = 1);

	void DrawMeshBatch(RHICmdList* cmdList, const MeshDrawCommandBatch& meshDrawCommand);

	void Dispatch(RHICmdList* cmdList, MaterialInstanceComputeBase* mat, LVector4i dispatchSize);
};

RENDER_API extern graphics::RenderResourceContext* sRenderResourceContext;

RENDER_API extern graphics::RenderDrawContext* sRenderDrawContext;

void  RENDER_API GenerateGlobelRenderResourceContext();

void  RENDER_API GenerateGlobelEncoderHelper();


}
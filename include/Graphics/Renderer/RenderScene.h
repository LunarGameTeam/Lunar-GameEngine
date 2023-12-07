#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/Renderer/SkeletonSkin.h"
#include "Graphics/Renderer/RenderObject.h"
#include <functional>


namespace luna::graphics
{

class AmbientAtmosphereData : public RenderData
{
	//环境光
	LVector4f           mAmbientColor = LVector4f(0.05, 0.05, 0.05, 1.0);
	//Skybox
	MaterialInstance*   mSkyboxMaterial = nullptr;
};

class RenderSceneDebugData : public RenderData
{
	RenderMeshBase  mDebugMeshLineData;
	RenderMeshBase  mDebugMeshData;
};

struct GpuSceneUploadComputeCommand
{
	LUnorderedMap<ShaderParamID, RHIView*> mStorageBufferInput;
	LUnorderedMap<ShaderParamID, RHIView*> mStorageBufferOutput;
	MaterialInstanceComputeBase* mComputeMaterial;
	LVector4i mDispatchSize;
};

struct GpuSceneUploadCopyCommand
{
	RHIResource* mUniformBufferInput;
	RHIResource* mStorageBufferOutput;
	size_t mSrcOffset;
	size_t mDstOffset;
	size_t mCopyLength;
};

class RenderSceneStagingMemory
{
	size_t mPerStageSize;
	LArray<RHIMemoryPtr> mMemoryData;
	LArray<size_t> mMemoryDataSize;
	size_t mMemoryOffset;
	//stage buffer池
	LArray<RHIResourcePtr> mStageBufferPool;
	LArray<RHIViewPtr> mStageBufferViewPool;
	size_t mPoolUsedSize;
public:
	RenderSceneStagingMemory(size_t perStageSize = 0x800000);

	RHIView* AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize);

	RHIView* AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize, std::function<void(void*)> memoryFunc);

	RHIView* AllocUniformStageBuffer(ShaderCBuffer* cbufferData);

	void Clear()
	{
		ClearMemory();
	};
private:
	void AddNewMemory(size_t memorySize);

	bool BindMemory(RHIResource* targetResource);

	void ClearMemory();
};

class RenderSceneUploadBufferPool
{
	size_t mFrameIndex;
	LArray<RenderSceneStagingMemory> mMemoryArray;
public:
	RenderSceneUploadBufferPool();

	RHIView* AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize);

	RHIView* AllocStructStageBuffer(size_t curBufferSize, RHIViewType useType, size_t strideSize, std::function<void(void*)> memoryFunc);

	RHIView* AllocUniformStageBuffer(ShaderCBuffer* cbufferData);

	void Present();
};

class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();

	virtual ~RenderScene();

	RenderObject* CreateRenderObject();

	void DestroyRenderObject(RenderObject* ro);

	void GetRenderObjects(LArray<RenderObject*>& valueOut) const;

	RenderView* CreateRenderView();

	void DestroyRenderView(RenderView* renderView);

	void GetAllView(LArray<RenderView*>& valueOut) const;

	GpuSceneUploadComputeCommand* AddComputeCommand();

	GpuSceneUploadCopyCommand* AddCopyCommand();

	void AddCbufferCopyCommand(ShaderCBuffer* cbufferDataIn, RHIResource* bufferOutput);

	RenderSceneUploadBufferPool* GetStageBufferPool();

	void ExcuteCopy();
	//void Debug();
private:
	//bool                  mBufferDirty = true;
	//bool                  mInit        = false;

	LHoldIdArray<RenderObject> mRenderObjects;

	LArray<GpuSceneUploadComputeCommand> mAllComputeCommand;

	LArray<GpuSceneUploadCopyCommand> mAllCopyCommand;

	LHoldIdArray<RenderView> mViews;

	RenderSceneUploadBufferPool mStageBufferPool;
};

}

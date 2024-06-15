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

enum class RenderDataUpdateCommandType
{
	RENDER_DATA_GENERATE,
	RENDER_DATA_UPDATE
};
struct RenderDataUpdateCommand
{
	RenderDataUpdateCommandType mType;
	std::function<void(void)> mExcuteFunc;
};

class RenderDataUpdateCommandPool
{
	LArray<RenderDataUpdateCommand> commandPool;
public:

	void AddCommand(RenderDataUpdateCommandType commandType,const std::function<void(void)>& func);

	void ExcuteCommand();
};

class RENDER_API RenderDataUpdateCommandAllocator
{
	size_t mGameLocation = 0;

	size_t mRenderLocation = 2;

	LArray<RenderDataUpdateCommandPool> mPools;
public:
	RenderDataUpdateCommandAllocator();

	void AddCommand(RenderDataUpdateCommandType commandType, const std::function<void(void)> &func);

	void ExcuteCommand();

	void FinishRecord();
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

	RenderDataUpdateCommandAllocator* GetRenderDataUpdater() { return &mAllRenderDataUpdateCommand; };

	void AddCbufferCopyCommand(ShaderCBuffer* cbufferDataIn, RhiUniformBufferPack* bufferOutput);

	RenderSceneUploadBufferPool* GetStageBufferPool();

	void ExcuteCopy(RHICmdList* cmdList);

	bool mRenderable = true;
private:

	LHoldIdArray<RenderObject> mRenderObjects;

	LHoldIdArray<RenderView> mViews;
private:

	LArray<GpuSceneUploadComputeCommand> mAllComputeCommand;

	LArray<GpuSceneUploadCopyCommand> mAllCopyCommand;

	RenderDataUpdateCommandAllocator mAllRenderDataUpdateCommand;

	RenderSceneUploadBufferPool mStageBufferPool;
};

}

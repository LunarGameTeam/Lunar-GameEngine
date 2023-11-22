#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/RenderModule.h"
namespace luna::graphics
{
	class RenderViewGpuDataBuffer : public RenderData
	{

	};

	struct GpuSceneUploadComputeCommand
	{
		LUnorderedMap<LString, RHIResourcePtr> mStorageBufferInput;
		LUnorderedMap<LString, RHIResourcePtr> mStorageBufferOutput;
		MaterialInstance* mComputeMaterial;
		LVector4i mDispatchSize;
	};

	struct GpuSceneUploadCopyCommand
	{
		RHIResourcePtr mUniformBufferInput;
		RHIResourcePtr mStorageBufferOutput;
		size_t mSrcOffset;
		size_t mDstOffset;
		size_t mCopyLength;
	};

	class GpuRenderScene : public RenderDataContainer
	{
		LArray<GpuSceneUploadComputeCommand> mAllComputeCommand;

		LArray<GpuSceneUploadCopyCommand> mAllCopyCommand;
	public:
		GpuSceneUploadComputeCommand* AddComputeCommand();

		GpuSceneUploadCopyCommand* AddCopyCommand();

		void ExcuteCopy();
	};

}
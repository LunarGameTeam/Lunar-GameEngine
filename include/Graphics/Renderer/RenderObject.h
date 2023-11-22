#pragma once
#include "Graphics/Renderer/RenderData.h"
#include "Core/Foundation/Container.h"
#include "Core/Math/Math.h"
#include "Graphics/RHI/RHITypes.h"
namespace luna::graphics
{
	struct RenderObjectGpuSceneDataBuffer : public RenderData
	{
		size_t                   mMaxRoObjectSize;

		RHIResourcePtr           mRoDataBuffer;

		RHIViewPtr               mRoDataBufferView;

		void SetMaterialParameter(MaterialInstance* matInstance);
	};

	struct RenderObjectRenderSceneDataBuffer : public RenderData
	{
		size_t                   mCurRoObjectSize;

		size_t                   mMaxRoObjectSize;

		RHIResourcePtr           mRoDataBuffer;

		RHIViewPtr               mRoDataBufferView;
	};

	class RENDER_API RenderObject : public RenderDataContainer, public HoldIdItem
	{
		LMatrix4f    mWorldMat;
		RenderScene* mOwnerScene = nullptr;
	public:
		RenderObject(RenderScene* ownerScene);

		void UpdateWorldMatrix(const LMatrix4f& worldMat) { mWorldMat = worldMat; }

		const LMatrix4f& GetWorldMatrix() { return mWorldMat; }
	};

}

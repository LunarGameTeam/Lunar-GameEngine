#pragma once
#include "Graphics/Renderer/RenderData.h"
#include "Core/Foundation/Container.h"
#include "Core/Math/Math.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Asset/MaterialTemplate.h"

namespace luna::graphics
{

struct RenderObjectDrawBatch;


class RENDER_API RenderObject : public RenderDataContainer, public HoldIdItem
	{
		LMatrix4f    mWorldMat;
		RenderScene* mOwnerScene = nullptr;
	public:
		RenderObject();

		void SetScene(RenderScene* ownerScene) { mOwnerScene = ownerScene; };

		void UpdateWorldMatrix(const LMatrix4f& worldMat) { mWorldMat = worldMat; }

		const LMatrix4f& GetWorldMatrix() { return mWorldMat; }
	};

	class RENDER_API RenderObjectDrawData : public RenderData
	{
		RHIResourcePtr mRoMessagePtr;
		RHIViewPtr mRoMessageView;
		LArray<RenderObject*> dirtyRo;
		LSharedPtr<MaterialComputeAsset> mMaterial;
		LSharedPtr<MaterialInstanceComputeBase> mMaterialInstance;

	public:
		std::vector<LSharedPtr<RenderObjectDrawBatch>> mDrawBatch;

		RenderObjectDrawData();

		void AddDirtyRo(RenderObject* roData);

		void SetMaterialParameter(MaterialInstanceBase* matInstance);

		void PerSceneUpdate(RenderScene* renderScene) override;
	private:
		void GenerateRoMatrixBuffer(void* pointer);

		void GenerateRoIndexBuffer(void* pointer);
	};
}

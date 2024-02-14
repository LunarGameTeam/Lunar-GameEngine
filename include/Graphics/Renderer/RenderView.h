#pragma once
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Graphics/Renderer/RenderData.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/RenderObject.h"
#include "Graphics/FrameGraph/FrameGraph.h"

#include <functional>



namespace luna::graphics
{


enum class RenderViewType
{
	SceneView,
	ShadowMapView
};

struct CameraIntrinsicsParameter
{
	int32_t	       mRtWidth = 0;
	int32_t        mRtHeight = 0;
	float          mFovY = 0.0f;
	float          mFar = 0.0f;
	float          mNear = 0.0f;
};

struct CameraExtrinsicsParameter
{
	LVector3f      mPosition;
	LMatrix4f      mViewMatrix;
};

class RENDER_API RenderViewParameterData : public RenderData
{
	bool mIntrinsicsDirty = false;

	bool mExtrinsicsDirty = false;

	LMatrix4f mProjMatrix;

	LSharedPtr<graphics::ShaderCBuffer> mViewCbuffer = nullptr;

	RHIResourcePtr  mViewParamRes;

	RHIViewPtr      mViewParamCbufferView;

	RHICBufferDesc  mCbufferDesc;
public:
	CameraIntrinsicsParameter mIntrinsicsParameter;

	CameraExtrinsicsParameter mExtrinsicsParameter;

	void Init();

	const RHICBufferDesc& GetParamDesc() { return mCbufferDesc; };

	void SetMaterialParameter(MaterialInstanceBase* matInstance);

	RHIResource* GetResource() { return mViewParamRes; };

	void MarkIntrinsicsDirty() { mIntrinsicsDirty = true; }

	void MarkExtrinsicsDirty() { mExtrinsicsDirty = true; }

	void PerViewUpdate(RenderView* renderView);
};

class RENDER_API RenderView : public RenderDataContainer, public HoldIdItem
{
public:
	RenderView();

	~RenderView() = default;
	
public:
	void Culling(RenderScene* scene);

public:
	RenderTarget* GetRenderTarget() const { return mRT.Get(); }

	LArray<RenderObject*>& GetViewVisibleROs() { return mViewVisibleROs; }

	void SetRenderTarget(RenderTarget* val) { mRT = val; }

	RenderViewType mViewType   = RenderViewType::SceneView;

	RenderScene*   mOwnerScene = nullptr;
private:

	LArray<RenderObject*> mViewVisibleROs;

	TPPtr<RenderTarget> mRT;

	bool                mDirty = false;
};

struct ViewRenderBridgeData
{
	
	graphics::RenderView* mView = nullptr;
	RenderViewParameterData* mViewParamData;
};

void RENDER_API RenderViewDataGenerateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData);

void RENDER_API RenderViewDataRenderTargetUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, graphics::RenderTarget* renderTarget);

void RENDER_API RenderViewDataTargetWidthUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, int32_t width);

void RENDER_API RenderViewDataTargetHeightUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, int32_t height);

void RENDER_API RenderViewDataFovUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float fovY);

void RENDER_API RenderViewDataNearPlaneUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float nearValue);

void RENDER_API RenderViewDataFarPlaneUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, float farValue);

void RENDER_API RenderViewDataPositionUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, const LVector3f& position);

void RENDER_API RenderViewDataViewMatrixUpdateCommand(graphics::RenderScene* curScene, ViewRenderBridgeData& renderData, const LMatrix4f& vieMat);

}

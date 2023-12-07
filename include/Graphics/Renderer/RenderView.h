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
class RenderViewParameterData : public RenderData
{
	RHIResourcePtr  mViewParamRes;

	RHIViewPtr      mViewParamCbufferView;

	RHICBufferDesc  mCbufferDesc;
public:
	void Init();

	const RHICBufferDesc& GetParamDesc() { return mCbufferDesc; };

	void SetMaterialParameter(MaterialInstanceBase* matInstance);

	RHIResource* GetResource() { return mViewParamRes; };
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


}

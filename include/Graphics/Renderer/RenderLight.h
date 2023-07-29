#pragma once

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RenderTypes.h"
#include "Core/Math/Math.h"

#include <memory>

#include "RenderData.h"


namespace luna::graphics
{

struct RENDER_API Light : public RenderData
{
	LVector4f    mColor      = LVector4f(1, 1, 1, 1);
	bool         mCastShadow = false;

	float        mIntensity  = 1.0;
	bool         mInit       = false;
	bool         mDirty      = true;
	RenderScene* mOwnerScene = nullptr;
};

struct RENDER_API PointLight : Light
{
	LVector3f              mPosition;
	float                  mFov    = (float)std::numbers::pi / 2.0f;
	float                  mAspect = 1;
	float                  mNear   = 0.1;
	float                  mFar    = 50;
	LArray<LMatrix4f>      mViewMatrix;
	LMatrix4f              mProjMatrix;
	LArray<ShaderCBuffer*> mParamBuffer;

	void PerViewUpdate(RenderView* renderScene) override;

	void Update(RenderView* view);
};

struct RENDER_API DirectionLight : Light
{

	LVector3f mDirection;
	LMatrix4f mViewMatrix;

	void Update(RenderView* view);
	void PerViewUpdate(RenderView* renderScene) override;

	ShaderCBuffer* mParamBuffer;
};

}

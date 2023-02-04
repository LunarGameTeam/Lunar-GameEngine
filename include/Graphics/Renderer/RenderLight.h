#pragma once

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RenderTypes.h"
#include "Core/Math/Math.h"

#include <memory>


namespace luna::render
{

struct RENDER_API Light
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
	LVector3f mPosition;
	float     mFov    = (float)std::numbers::pi / 2.0f;
	float     mAspect = 1;
	float     mNear   = 0.1;
	float     mFar    = 50;
	LArray<LMatrix4f> mViewMatrix;
	LMatrix4f mProjMatrix;
	LArray<ShaderParamsBuffer*>			mParamBuffer;
	void Update(RenderView* view);
};

struct RENDER_API DirectionLight : Light
{

	LVector3f      mDirection;


	void Update(RenderView* view);

	ShaderParamsBuffer* mParamBuffer;
};

}
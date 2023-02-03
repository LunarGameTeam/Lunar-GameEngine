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
};

struct RENDER_API DirectionLight : Light
{

	LVector3f      mDirection;
	LMatrix4f      mViewMatrix;
	LMatrix4f      mProjMatrix;


	void Init();
	void Update(RenderView* view);

	std::shared_ptr<ShaderParamsBuffer> mParamBuffer;
};

}
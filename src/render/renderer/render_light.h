#pragma once
#include "core/foundation/container.h"
#include "core/foundation/string.h"
#include <functional>


namespace luna::render
{
	struct Light
	{
		LVector4f mColor = LVector4f(1, 1, 1, 1);
		bool      mCastShadow;
		float     mIndensity;
		LVector3f mDirection;
		LMatrix4f mViewMatrix;
		LMatrix4f mProjMatrix;

		const LMatrix4f &GetViewMatrix()const
		{
			return mViewMatrix;
		}
		const LMatrix4f& GetProjectionMatrix() const
		{
			return mProjMatrix;
		}
	};
}

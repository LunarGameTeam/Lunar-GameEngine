#pragma once
#include "core/misc/container.h"
#include "core/misc/string.h"
#include <functional>


namespace luna::render
{
	struct Light
	{
		LVector3f mColor;
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

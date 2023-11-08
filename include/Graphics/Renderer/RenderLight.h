#pragma once

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RenderTypes.h"
#include "Core/Math/Math.h"

#include <memory>

#include "RenderData.h"


namespace luna::graphics
{
	enum class PointBasedLightType
	{
		POINT_BASED_LIGHT_POINT = 0,
		POINT_BASEDLIGHT_DIRECTION,
		POINT_BASEDLIGHT_SPOT
	};
struct RENDER_API PointBasedLight : public RenderData
{
	size_t              mIndex;
	PointBasedLightType mType = PointBasedLightType::POINT_BASED_LIGHT_POINT;
	LVector4f           mColor      = LVector4f(1, 1, 1, 1);
	float               mIntensity  = 1.0;
	//direction
	LVector3f           mDirection;
	//spot


	RenderScene* mOwnerScene = nullptr;
};

//struct RENDER_API PointLight : PointBasedLight
//{
//	LVector3f              mPosition;
//	float                  mFov    = (float)std::numbers::pi / 2.0f;
//	float                  mAspect = 1;
//	float                  mNear   = 0.1;
//	float                  mFar    = 50;
//	LArray<LMatrix4f>      mViewMatrix;
//	LMatrix4f              mProjMatrix;
//	LArray<ShaderCBuffer*> mParamBuffer;
//
//	void PerViewUpdate(RenderView* renderScene) override;
//	
//};
//
//struct RENDER_API DirectionLight : PointBasedLight
//{
//
//	LVector3f mDirection;
//	LMatrix4f mViewMatrix;
//	
//	void PerViewUpdate(RenderView* renderScene) override;
//
//	ShaderCBuffer* mParamBuffer;
//};


class PointBasedRenderLightData : RenderData
{
	LUnorderedMap<size_t,LSharedPtr<PointBasedLight>> mAllLights;
	LQueue<size_t> mFreeIndex;
	ShaderCBuffer* mLightBufferGlobelMessage;
	LSharedPtr<FGResource> mLightDataBuffer;
	RHIResourcePtr mExistLightDataBuffer;
public:
	PointBasedRenderLightData();
	PointBasedLight* CreatePointBasedLight();
	void DestroyLight(size_t lightIndex);
	void PerSceneUpdate(RenderScene* renderScene) override;
};

PointBasedRenderLightData::PointBasedRenderLightData()
{
	size_t elementSize = 3 * sizeof(LVector4f);
	RHIBufferDesc desc;
	desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
	desc.mSize = elementSize * 128;
	mExistLightDataBuffer = sRenderModule->GetRenderContext()->CreateBuffer(RHIHeapType::Upload, desc);
	mLightDataBuffer->BindExternalResource(mExistLightDataBuffer.get());
}

PointBasedLight* PointBasedRenderLightData::CreatePointBasedLight()
{
	LSharedPtr<PointBasedLight> newLight = MakeShared<PointBasedLight>();
	size_t curIndex = mAllLights.size();
	if (!mFreeIndex.empty())
	{
		curIndex = mFreeIndex.front();
		mFreeIndex.pop();
	}
	mAllLights.insert({ curIndex ,newLight });
	return newLight.get();
}

void PointBasedRenderLightData::DestroyLight(size_t lightIndex)
{
	auto itor = mAllLights.find(lightIndex);
	if(itor == mAllLights.end())
	{
		return;
	}
	mAllLights.erase(lightIndex);
	mFreeIndex.push(lightIndex);
}

void PointBasedRenderLightData::PerSceneUpdate(RenderScene* renderScene)
{
	//这里先全量刷新，后面看能不能懒更新
	LVector4f* lightBufferData = (LVector4f*)mExistLightDataBuffer->Map();
	size_t mPointLightSize = 0;
	size_t mDirectionLightSize = 0;
	size_t mSpotLightSize = 0;
	for (auto eachLight : mAllLights)
	{
		switch (eachLight.second->mType)
		{
		case PointBasedLightType::POINT_BASED_LIGHT_POINT:
		{
			mPointLightSize += 1;
			break;
		}
		case PointBasedLightType::POINT_BASEDLIGHT_DIRECTION:
		{
			mDirectionLightSize += 1;
			break;
		}
		case PointBasedLightType::POINT_BASEDLIGHT_SPOT:
		{
			mSpotLightSize += 1;
			break;
		}
		default:
			break;
		}
	}
	size_t mPointLightCopy = 0;
	size_t mDirectionLightCopy = 0;
	size_t mSpotLightCopy = 0;
	for (auto eachLight : mAllLights)
	{
		LVector4f* currentPointer = nullptr;
		switch (eachLight.second->mType)
		{
		case PointBasedLightType::POINT_BASED_LIGHT_POINT:
		{
			currentPointer = lightBufferData + 3 * mPointLightCopy;
			mPointLightCopy += 1;
			break;
		}
		case PointBasedLightType::POINT_BASEDLIGHT_DIRECTION:
		{
			currentPointer = lightBufferData + 3 * (mPointLightSize + mDirectionLightCopy);
			currentPointer[2] = eachLight.second->mDirection;
			currentPointer[2].w() = 0;
			mDirectionLightCopy += 1;
			break;
		}
		case PointBasedLightType::POINT_BASEDLIGHT_SPOT:
		{
			currentPointer = lightBufferData + 3 * (mPointLightSize + mDirectionLightSize + mSpotLightCopy);
			mSpotLightCopy += 1;
			break;
		}
		default:
			break;
		}
		currentPointer[0] = eachLight.second->mColor;
		currentPointer[1].x() = eachLight.second->mIntensity;
	}
	mExistLightDataBuffer->Unmap();
	mLightBufferGlobelMessage->Set("PointLightNum", mPointLightSize);
	mLightBufferGlobelMessage->Set("DirectionLightNum", mDirectionLightSize);
	mLightBufferGlobelMessage->Set("SpotLightNum", mSpotLightSize);
	mLightBufferGlobelMessage->Commit();

}

}

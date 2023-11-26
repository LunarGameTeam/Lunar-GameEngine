#pragma once

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RenderTypes.h"
#include "Core/Math/Math.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/Asset/MaterialTemplate.h"
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

struct RENDER_API PointBasedLight
{
	size_t              mIndex;

	PointBasedLightType mType = PointBasedLightType::POINT_BASED_LIGHT_POINT;

	LVector4f           mColor      = LVector4f(1, 1, 1, 1);

	float               mIntensity  = 1.0;

	LVector4f           mRangeParam = LVector4f(0, 0, 0, 0);
	//position
	LVector3f           mPosition;
	//direction
	LVector3f           mDirection;
	//spot
	
	RenderScene* mOwnerScene = nullptr;
};

class PointBasedRenderLightData : RenderData
{
	//这个数据是render scene的光源组件，持有dirty的光源信息
	size_t mMaxLightNum;

	LHoldIdArray<PointBasedLight> mAllLights;

	RHIResourcePtr mLightParameterBuffer;

	RHIViewPtr     mLightParameterBufferView;

	RHIResourcePtr mExistLightDataBuffer;

	RHIViewPtr     mExistLightDataBufferView;

	LArray<PointBasedLight*> mDirtyList;

	LSharedPtr<ShaderCBuffer> mLightBufferGlobelMessage = nullptr;

	bool LightNumDirty = true;

	LArray<uint32_t> mPointLightIndex;

	LArray<uint32_t> mDirectionLightIndex;

	LArray<uint32_t> mSpotLightIndex;
public:
	PointBasedRenderLightData();

	PointBasedLight* CreatePointBasedLight();

	void DestroyLight(PointBasedLight* light);

	void SetMaterialParameter(MaterialInstance* matInstance);

	void PerSceneUpdate(RenderScene* renderScene);

	void MarkLightDirty(PointBasedLight* light) { mDirtyList.push_back(light); };
};
}

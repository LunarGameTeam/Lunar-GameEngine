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

struct RenderLightGpuDataBuffer : public RenderData
{
	//这个数据是GPUscene的光源组件，持有所有的光源信息
	LSharedPtr<ShaderCBuffer> mLightParameter;

	size_t mMaxLightNum;

	RHIResourcePtr mExistLightDataBuffer;

	RHIViewPtr mExistLightDataBufferView;

	void SetMaterialParameter(MaterialInstance* matInstance);
};

class PointBasedRenderLightData : RenderData
{
	//这个数据是render scene的光源组件，持有dirty的光源信息
	size_t mMaxLightNum;

	size_t mCurDirtyLightNum;

	LHoldIdArray<PointBasedLight> mAllLights;

	LSharedPtr<ShaderCBuffer> mLightBufferGlobelMessage;

	RHIResourcePtr mExistLightDataBuffer;

	RHIViewPtr     mExistLightDataBufferView;
public:
	PointBasedRenderLightData();

	PointBasedLight* CreatePointBasedLight();

	void DestroyLight(PointBasedLight* light);

	void PerSceneUpdate(RenderScene* renderScene) override;
};
}

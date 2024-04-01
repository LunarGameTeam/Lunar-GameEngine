#pragma once

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RenderTypes.h"
#include "Core/Math/Math.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
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

struct RENDER_API PointBasedLight : public luna::HoldIdItem
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

class RENDER_API PointBasedRenderLightData : public RenderData
{
	//这个数据是render scene的光源组件，持有dirty的光源信息
	size_t mMaxLightNum;

	LHoldIdArray<PointBasedLight> mAllLights;

	LSharedPtr<RhiUniformBufferPack> mLightParameterBuffer;

	RHIViewPtr     mLightParameterBufferView;

	RHIResourcePtr mExistLightDataBuffer;

	RHIViewPtr     mExistLightDataBufferView;

	LUnorderedSet<PointBasedLight*> mDirtyList;

	LSharedPtr<ShaderCBuffer> mLightBufferGlobelMessage = nullptr;

	bool LightNumDirty = true;

	LArray<uint32_t> mPointLightIndex;

	LArray<uint32_t> mDirectionLightIndex;

	LArray<uint32_t> mSpotLightIndex;

	LSharedPtr<MaterialComputeAsset> mMaterial;

	LSharedPtr<MaterialInstanceComputeBase> mMaterialInstance;

	bool LightDataInitSucceed = false;
public:
	PointBasedRenderLightData();

	PointBasedLight* CreatePointBasedLight();

	void DestroyLight(PointBasedLight* light);

	void SetMaterialParameter(MaterialInstance* matInstance);

	void PerSceneUpdate(RenderScene* renderScene) override;

	void UpdateLightNumParameter(RenderScene* renderScene);

	void UpdateDirtyLightData(RenderScene* renderScene);

	void MarkLightDirty(PointBasedLight* light);
private:
	void GenerateDirtyLightDataBuffer(void* pointer);

	void GenerateDirtyLightIndexBuffer(void* pointer);
};

struct LightRenderBridgeData
{
	graphics::RenderView* mView = nullptr;
	graphics::PointBasedRenderLightData* mRenderData = nullptr;
	graphics::PointBasedLight* mRenderLight = nullptr;
};

void RENDER_API RenderLightDataGenerateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, PointBasedLightType type);

void RENDER_API GenerateRenderLightColorUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector4f& colorIn);

void RENDER_API GenerateRenderLightIntensityUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const float intensity);

void RENDER_API GenerateRenderLightPositionUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector3f& position);

void RENDER_API GenerateRenderLightDirectionUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector3f& dir);

void RENDER_API GenerateRenderLightRangeParameterUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const LVector4f& rangeParamter);

void RENDER_API GenerateRenderCastShadowUpdateCommand(graphics::RenderScene* curScene, LightRenderBridgeData& renderData, const bool castShadow);


}

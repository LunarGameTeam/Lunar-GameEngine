#include "Game/Light.h"
#include "Core/Object/Transform.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Game/GameModule.h"
namespace luna
{

	void GameLightRenderDataUpdater::ClearData(graphics::GameRenderBridgeData* curData)
	{
		GameRenderBridgeDataLight* realPointer = static_cast<GameRenderBridgeDataLight*>(curData);
		realPointer->mDirty = false;
	};

	void GameLightRenderDataUpdater::UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene)
	{
		GameRenderBridgeDataLight* realPointer = static_cast<GameRenderBridgeDataLight*>(curData);
		if (!realPointer->mDirty)
		{
			return;
		}
		if (mRenderLight == nullptr)
		{
			graphics::PointBasedRenderLightData* lightData = curScene->RequireData<graphics::PointBasedRenderLightData>();
			mRenderLight = lightData->CreatePointBasedLight();
		}
		mRenderLight->mColor = realPointer->mColor;
		mRenderLight->mIntensity = realPointer->mIntensity;
		mRenderLight->mType = graphics::PointBasedLightType::POINT_BASED_LIGHT_POINT;

		LightUpdateRenderThreadImpl(realPointer, mRenderLight, curScene);
	}

	GameLightRenderDataUpdater::~GameLightRenderDataUpdater()
	{
		if (mRenderLight)
		{
			mRenderLight->mOwnerScene->RequireData<graphics::PointBasedRenderLightData>()->DestroyLight(mRenderLight->mIndex);
		}
	};


	GameDirLightRenderDataUpdater::~GameDirLightRenderDataUpdater()
	{
		if (mShadowView != nullptr)
		{
			mShadowView->mOwnerScene->DestroyRenderView(mShadowView);
		}
	}

	void GameDirLightRenderDataUpdater::LightUpdateRenderThreadImpl(GameRenderBridgeDataLight* curData, graphics::PointBasedLight* mRenderLight, graphics::RenderScene* curScene)
	{
		GameRenderBridgeDataDirLight* realPointer = static_cast<GameRenderBridgeDataDirLight*>(curData);
		mRenderLight->mDirection = realPointer->mDirection;
		//这里需要更新下当前的光源阴影信息
		if (mShadowView == nullptr && realPointer->mCastShadow == true)
		{
			mShadowView = curScene->CreateRenderView();
			mShadowView->mViewType = graphics::RenderViewType::ShadowMapView;
		}
		if (mShadowView != nullptr && realPointer->mCastShadow == false)
		{
			curScene->DestroyRenderView(mShadowView);
		}
	}

	RegisterTypeEmbedd_Imp(LightComponent)
	{
		cls->Ctor<LightComponent>();
		cls->Binding<Self>();

		cls->BindingProperty<&LightComponent::mColor>("color")
			.Setter<&LightComponent::SetColor>()
			.Serialize();
		cls->BindingProperty<&Self::mIntensity>("intensity")
			.Setter<&LightComponent::SetIndensity>()
			.Serialize();
		cls->BindingProperty<&Self::mCastShadow>("cast_shadow")
			.Setter<&LightComponent::SetCastShadow>()
			.Serialize();
		BindingModule::Get("luna")->AddType(cls);
	};

	RegisterTypeEmbedd_Imp(DirectionLightComponent)
	{
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);
		cls->Ctor<DirectionLightComponent>();
	};

	RegisterTypeEmbedd_Imp(PointLightComponent)
	{
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);
		cls->Ctor<PointLightComponent>();
	};


	LightComponent::~LightComponent()
	{

	}

	void LightComponent::OnCreate()
	{
		Component::OnCreate();

	}

	void LightComponent::SetColor(const LVector4f& val)
	{
		mNeedUpdate = true;
		mColor = val;

	}

	void LightComponent::SetIndensity(float val)
	{
		mNeedUpdate = true;
		mIntensity = val;
	}

	void LightComponent::SetCastShadow(bool val)
	{
		mNeedUpdate = true;
		mCastShadow = val;
	}

	void LightComponent::UpdateLightBridgeDataBase(GameRenderBridgeDataLight* curData)
	{
		if (mNeedUpdate)
		{
			curData->mDirty = true;
			curData->mCastShadow = mCastShadow;
			curData->mColor = mColor;
			curData->mIntensity = mIntensity;
			mNeedUpdate = false;
		}
	}


	void PointLightComponent::OnCreate()
	{
		LightComponent::OnCreate();
		mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&PointLightComponent::OnTransformDirty, this));
		OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
	}

	void PointLightComponent::OnTransformDirty(Transform* transform)
	{
		mNeedUpdate = true;
		//GetScene()->GetRenderScene()->SetSceneBufferDirty();
		//mLight->mDirty = true;
		//graphics::PointLight* light = (graphics::PointLight*)mLight;
		//light->mPosition = mTransform->GetPosition();

	}

	LSharedPtr<graphics::GameRenderDataUpdater> PointLightComponent::OnTickImpl(graphics::GameRenderBridgeData* curRenderData)
	{
		if (mNeedUpdate)
		{
			GameRenderBridgeDataLight* realPointer = static_cast<GameRenderBridgeDataLight*>(curRenderData);
			UpdateLightBridgeDataBase(realPointer);
		}
	}


	void DirectionLightComponent::OnTransformDirty(Transform* transform)
	{
		mNeedUpdate = true;
		//graphics::DirectionLight* light = (graphics::DirectionLight*)mLight;
		//light->mDirection = GetDirection();

	}

	void DirectionLightComponent::OnCreate()
	{
		LightComponent::OnCreate();
		mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&DirectionLightComponent::OnTransformDirty, this));
		OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
	}

	const LVector3f DirectionLightComponent::GetCSMSplit()const
	{
		return mCSMSplit;
	}

	void DirectionLightComponent::SetCSMSplits(const LVector3f& val)
	{
		mNeedUpdate = true;
		mCSMSplit = val;
	}

	LSharedPtr<graphics::GameRenderDataUpdater> DirectionLightComponent::OnTickImpl(graphics::GameRenderBridgeData* curRenderData)
	{
		if (mNeedUpdate)
		{
			GameRenderBridgeDataDirLight* realPointer = static_cast<GameRenderBridgeDataDirLight*>(curRenderData);
			UpdateLightBridgeDataBase(realPointer);
			realPointer->mDirection = GetDirection();
			realPointer->mCSMSplit = mCSMSplit;
		}
	}



}
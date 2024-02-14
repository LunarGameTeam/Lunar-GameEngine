#include "Game/Light.h"
#include "Core/Object/Transform.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Game/GameModule.h"
namespace luna
{
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
		Super::OnCreate();

	}

	void LightComponent::SetColor(const LVector4f& val)
	{
		mColor = val;
		if (GetScene())
		{
			luna::graphics::GenerateRenderLightColorUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mColor);
		}
	}

	void LightComponent::SetIndensity(float val)
	{
		mIntensity = val;
		if (GetScene())
		{
			luna::graphics::GenerateRenderLightIntensityUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mIntensity);
		}
	}

	void LightComponent::SetCastShadow(bool val)
	{
		mCastShadow = val;
		if (GetScene())
		{
			luna::graphics::GenerateRenderCastShadowUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mCastShadow);
		}
	}

	void PointLightComponent::OnCreate()
	{
		LightComponent::OnCreate();
		luna::graphics::RenderLightDataGenerateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, graphics::PointBasedLightType::POINT_BASED_LIGHT_POINT);
		mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&PointLightComponent::OnTransformDirty, this));
		OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
	}

	void PointLightComponent::OnTransformDirty(Transform* transform)
	{
		mPosition = mTransform->GetPosition();
		luna::graphics::GenerateRenderLightPositionUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mPosition);
	}


	void DirectionLightComponent::OnTransformDirty(Transform* transform)
	{
		luna::graphics::GenerateRenderLightDirectionUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, GetDirection());
	}

	void DirectionLightComponent::OnCreate()
	{
		LightComponent::OnCreate();
		luna::graphics::RenderLightDataGenerateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, graphics::PointBasedLightType::POINT_BASEDLIGHT_DIRECTION);
		mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&DirectionLightComponent::OnTransformDirty, this));
		OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
	}

	const LVector3f DirectionLightComponent::GetCSMSplit()const
	{
		return mCSMSplit;
	}

	void DirectionLightComponent::SetCSMSplits(const LVector3f& val)
	{
		mCSMSplit = val;
	}
}
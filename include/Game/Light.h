#pragma once

#include "Core/Object/Transform.h"
#include "Core/Object/Component.h"

#include "Graphics/Renderer/RenderScene.h"

#include "Game/GameConfig.h"


namespace luna
{

class GAME_API LightComponent : public Component
{
	RegisterTypeEmbedd(LightComponent, Component)
public:	 
	void OnCreate() override;

	const LVector3f& GetColor()
	{
		return mColor;
	}
	const float GetIntensity()const
	{
		return mIdentisity;
	}

	void OnTransformDirty(Transform* transform);
protected:
	ActionHandle            mTransformDirtyAction;
	LVector3f               mColor      = LVector3f(0.8f, 0.8f, 0.8f);
	bool                    mCastShadow = false;
	float                   mIdentisity = 1.0f;
	render::DirectionLight* mLight = nullptr;
};

class GAME_API PointLightComponent : public LightComponent
{
	RegisterTypeEmbedd(PointLightComponent, LightComponent)
public:
	void OnCreate() override;

	LVector3f GetPosition() const
	{
		return mTransform->GetPosition();
	}
};

class GAME_API DirectionLightComponent : public LightComponent
{
	RegisterTypeEmbedd(DirectionLightComponent, LightComponent)
public:	
	void OnCreate() override;

	const LMatrix4f& GetProjectionMatrix(int idx)const
	{
		return mProj[idx];
	}
	const LVector3f GetDirection() const
	{
		return mTransform->ForwardDirection();
	}

	const LMatrix4f GetWorldMatrix()const
	{
		return mTransform->GetLocalToWorldMatrix();
	}
	const LMatrix4f GetViewMatrix()const
	{
		return mViewMatrix[0];
	}
	const LMatrix4f GetViewMatrix(int idx)const
	{
		return mViewMatrix[idx];
	}
	const LQuaternion GetRotation()const
	{
		return mTransform->GetRotation();
	}

	void OnTick(float delta_time) override;

	void SetProjectionMatrix(const LMatrix4f& val, int csm_index);
	void SetViewMatrix(const LMatrix4f& val, int csm_index);


	const LVector3f GetCSMSplit() const;
	void SetCSMSplits(const LVector3f& val);

private:
	LMatrix4f mViewMatrix[4];
	LMatrix4f mProj[4];
	LVector3f mCSMSplit = LVector3f(0.2f, 0.5f, 0.7f);
	float zFar = 100, zNear = 0.01f;
	float mAspect = 1024.f / 768.f;


};

}
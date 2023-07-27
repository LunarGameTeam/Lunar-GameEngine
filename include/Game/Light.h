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
	virtual ~LightComponent();

	void OnCreate() override;

	const LVector4f& GetColor()
	{
		return mColor;
	}
	void SetColor(const LVector4f& val);

	void SetIndensity(float val);

	float GetIntensity() const
	{
		return mIntensity;
	}

	void SetCastShadow(bool val);
protected:
	ActionHandle   mTransformDirtyAction;
	LVector4f      mColor      = LVector4f(0.8f, 0.8f, 0.8f, 1.0f);
	bool           mCastShadow = false;
	float          mIntensity = 1.0f;
	graphics::Light* mLight      = nullptr;
};

class GAME_API PointLightComponent : public LightComponent
{
	RegisterTypeEmbedd(PointLightComponent, LightComponent)
public:
	void OnCreate() override;
	void OnTransformDirty(Transform* transform);

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
	void OnTransformDirty(Transform* transform);

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
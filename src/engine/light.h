#pragma once

#include "core/core_library.h"
#include "core/object/transform.h"
#include "core/object/component.h"

#include "render/renderer/render_scene.h"

#include "engine/engine_config.h"


namespace luna
{

class ENGINE_API LightComponent : public Component
{
	RegisterTypeEmbedd(LightComponent, Component)
public:
	void OnCreate() override;
	GET_SET_VALUE(float, mIdentisity, Indensity);
	GET_SET_VALUE(bool, mCastShadow, CastShadow);

	void OnTransformDirty(Transform* transform);
protected:
	ActionHandle         mTransformDirtyAction;
	render::RenderLight* mLight = nullptr;
	LVector3f            mColor = LVector3f(0.8f, 0.8f, 0.8f);
	bool                 mCastShadow = false;
	float                mIdentisity;
	
};

class ENGINE_API PointLightComponent : public LightComponent
{
	RegisterTypeEmbedd(PointLightComponent, LightComponent)
public:
	const LVector3f& GetColor()
	{
		return mColor;
	}
	void OnCreate() override;
	float GetIntensity() ;
	LVector3f GetPosition();
};

class ENGINE_API DirectionLightComponent : public LightComponent
{
	RegisterTypeEmbedd(DirectionLightComponent, LightComponent)
public:
	void OnCreate() override;
	const float GetIntensity()const;
	const LMatrix4f &GetProjectionMatrix(int csm_index)const ;
	const LVector3f GetDirection()const;
	const LMatrix4f GetWorldMatrix()const;
	const LMatrix4f GetViewMatrix()const;
	const LMatrix4f GetViewMatrix(int csm_index)const;
	const LVector3f GetColor()const;
	const LQuaternion GetRotation()const;

	void SetProjectionMatrix(const LMatrix4f& val, int csm_index);
	void SetViewMatrix(const LMatrix4f& val, int csm_index);


	const LVector3f GetCSMSplit()const;


	void SetCSMSplits(const LVector3f& val);

private:
	LMatrix4f mViewMatrix[4];
	LMatrix4f mProj[4];
	LVector3f mCSMSplit = LVector3f(0.2f, 0.5f, 0.7f);
	float zFar = 100, zNear = 0.01f;
	float mAspect = 1024.f / 768.f;


};

}
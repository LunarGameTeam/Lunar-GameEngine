#pragma once

#include "game/game_config.h"

#include "core/core_library.h"
#include "core/object/transform.h"
#include "core/object/component.h"

#include <numbers>
#include "render/renderer/render_view.h"

namespace luna
{

class GAME_API CameraComponent : public Component
{
	RegisterTypeEmbedd(CameraComponent, Component)
public:
	virtual ~CameraComponent();
	void OnCreate() override;
	
	void OnTick(float delta_time) override;


	const LMatrix4f GetWorldMatrix()const;
	const LMatrix4f& GetViewMatrix()const;
	const LMatrix4f& GetProjectionMatrix()const;
	const LFrustum GetFrustum()const;
	const LVector3f& GetPosition()const;
	const float GetFar()const;
	const float GetNear()const;
	void SetFar(float val);
	void SetNear(float val);

	Transform* GetTransform()
	{
		return mTransform;
	}
	
	GET_SET_VALUE(LVector3f, mDirection, FlyDirection);
	GET_SET_VALUE(float, mSpeed, Speed);

	render::RenderTarget* GetRenderViewTarget();
	void SetRenderViewTarget(render::RenderTarget* target);

	void SetAspectRatio(float val);

private:
	render::RenderView* mRenderView;
	Transform* mTransform;

	float mFar = 200.f, mNear = 0.1f;
	mutable LMatrix4f mViewMat = LMatrix4f::Identity();
	mutable LMatrix4f mProjMat = LMatrix4f::Identity();

	float mAspect = 1024.f / 768.f;
	float mFovY = (float)std::numbers::pi / 3.0f;
	LVector3f mDirection = LVector3f(0, 0, 0);
	float mSpeed = 0.0f;
};

}
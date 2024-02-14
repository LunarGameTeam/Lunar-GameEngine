#pragma once

#include "Game/GameConfig.h"

#include "Core/Object/Transform.h"
#include "Core/Object/Component.h"

#include <numbers>
#include "Graphics/Renderer/RenderView.h"
#include "Core/Object/System.h"

namespace luna
{
class GAME_API CameraSystem : public System
{
	RegisterTypeEmbedd(CameraSystem, System)
public:
	CameraSystem();
	void OnTick(float deltaTime) override;

};

class GAME_API CameraComponent : public Component
{
	RegisterTypeEmbedd(CameraComponent, Component)
public:
	CameraComponent();
	virtual ~CameraComponent();
	void OnCreate() override;

	void OnActivate() override;
	void OnDeactivate() override;
	
	//void OnTick(float delta_time) override;

	LVector3f ViewportToWorldPosition(const LVector2f& viewport);
	LVector2f WorldPositionToViewport(const LVector3f& worldpos);

	const LMatrix4f GetWorldMatrix()const;
	const LMatrix4f& GetViewMatrix()const;
	const LMatrix4f& GetProjectionMatrix()const;
	const LFrustum GetFrustum()const;
	LVector3f GetPosition() const;
	float GetFar()const;
	float GetNear()const;
	void SetFar(float val);
	void SetNear(float val);

	Transform* GetTransform()
	{
		return mTransform;
	}
	
	GET_SET_VALUE(LVector3f, mDirection, FlyDirection);
	GET_SET_VALUE(float, mSpeed, Speed);

	graphics::RenderTarget* GetRenderViewTarget();
	void SetRenderViewTarget(graphics::RenderTarget* target);

	void SetAspectRatio(float val);

private:
	ActionHandle   mTransformDirtyAction;
	ActionHandle   mRenderTargetDirtyAction;

	float mFar = 1000.f, mNear = 0.1f;
	mutable LMatrix4f mViewMat = LMatrix4f::Identity();
	mutable LMatrix4f mProjMat = LMatrix4f::Identity();

	float mAspect = 1024.f / 768.f;
	float mFovY = (float)std::numbers::pi / 3.0f;
	LVector3f mDirection = LVector3f(0, 0, 0);
	float mSpeed = 0.0f;

	TPPtr<graphics::RenderTarget> mTarget;

	luna::graphics::ViewRenderBridgeData mVirtualRenderData;
private:
	void OnTransformDirty(Transform* transform);
	void OnRenderTargetDirty(graphics::RenderTarget* renderTarget);
};

}
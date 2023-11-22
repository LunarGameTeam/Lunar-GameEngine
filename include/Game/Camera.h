#pragma once

#include "Game/GameConfig.h"

#include "Core/Object/Transform.h"
#include "Core/Object/Component.h"

#include <numbers>
#include "Graphics/Renderer/RenderView.h"
#include "Core/Object/System.h"
#include "Game/RenderComponent.h"

namespace luna
{
class GAME_API CameraSystem : public System
{
	RegisterTypeEmbedd(CameraSystem, System)
public:
	CameraSystem();
	void OnTick(float deltaTime) override;

};

struct CameraIntrinsicsParameter
{
	int32_t	       mRtWidth = 0;
	int32_t        mRtHeight = 0;
	float          mFovY = 0.0f;
	float          mFar = 0.0f;
	float          mNear = 0.0f;
};

struct CameraExtrinsicsParameter
{
	LVector3f      mPosition;
	LMatrix4f      mViewMatrix;
};

struct GameRenderBridgeDataCamera :public graphics::GameRenderBridgeData
{
	//相机内参数
	bool mIntrinsicsDirty = false;
	CameraIntrinsicsParameter mIntrinsicsParameter;
	//相机外参数
	bool mExtrinsicsDirty = false;
	CameraExtrinsicsParameter mExtrinsicsParameter;
};

class GameCameraRenderDataUpdater :public graphics::GameRenderDataUpdater
{
	graphics::RenderView* mRenderView = nullptr;
public:
	virtual ~GameCameraRenderDataUpdater();
private:
	void UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene) override;

	void ClearData(graphics::GameRenderBridgeData* curData) override;

	virtual LSharedPtr<graphics::GameRenderBridgeData> GenarateData() override { return MakeShared<GameRenderBridgeDataCamera>(); };
};

class GAME_API CameraComponent : public graphics::RendererComponent
{
	RegisterTypeEmbedd(CameraComponent, Component)
public:
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

	float mFar = 1000.f, mNear = 0.1f;
	mutable LMatrix4f mViewMat = LMatrix4f::Identity();
	mutable LMatrix4f mProjMat = LMatrix4f::Identity();

	float mAspect = 1024.f / 768.f;
	float mFovY = (float)std::numbers::pi / 3.0f;
	LVector3f mDirection = LVector3f(0, 0, 0);
	float mSpeed = 0.0f;

	graphics::RenderTarget* mTarget;
private:
	bool mNeedUpdateIntrinsics = false;
	bool mNeedUpdateExtrinsics = false;
	void OnTransformDirty(Transform* transform);
	//render相关
	LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameCameraRenderDataUpdater>(); }

	LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData) override;
};

}
#pragma once

#include "Core/Object/Transform.h"
#include "Core/Object/Component.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderLight.h"

#include "Game/GameConfig.h"
#include "Game/RenderComponent.h"

namespace luna
{
	//点光源
	struct GameRenderBridgeDataLight :public graphics::GameRenderBridgeData
	{
		bool           mDirty = false;
		LVector4f      mColor = LVector4f(0.0f, 0.0f, 0.0f, 1.0f);
		LVector3f      mPosition = LVector3f(0.0f, 0.0f, 0.0f, 1.0f);
		bool           mCastShadow = false;
		float          mIntensity = 1.0f;
	};

	class GameLightRenderDataUpdater :public graphics::GameRenderDataUpdater
	{
		graphics::PointBasedLight*           mRenderLight = nullptr;
	public:
		virtual ~GameLightRenderDataUpdater();
	private:
		void UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene) override;

		void ClearData(graphics::GameRenderBridgeData* curData) override;

		virtual LSharedPtr<graphics::GameRenderBridgeData> GenarateData() override {return MakeShared<GameRenderBridgeDataLight>();};

		virtual void LightUpdateRenderThreadImpl(GameRenderBridgeDataLight* curData, graphics::PointBasedLight* mRenderLight, graphics::RenderScene* curScene) {};
	};


	//方向光
	struct GameRenderBridgeDataDirLight :public GameRenderBridgeDataLight
	{
		LVector3f      mDirection = LVector4f(0.0f, -1.0f, 0.0f, 1.0f);
		LVector3f      mCSMSplit = LVector3f(0.2f, 0.5f, 0.7f);
	};

	class GameDirLightRenderDataUpdater :public GameLightRenderDataUpdater
	{
		graphics::RenderView* mShadowView = nullptr;
	public:
		~GameDirLightRenderDataUpdater();
	private:
		LSharedPtr<graphics::GameRenderBridgeData> GenarateData() override { return MakeShared<GameRenderBridgeDataDirLight>(); };

		void LightUpdateRenderThreadImpl(GameRenderBridgeDataLight* curData, graphics::PointBasedLight* mRenderLight, graphics::RenderScene* curScene) override;
	};


	class GAME_API LightComponent : public graphics::RendererComponent
	{
		RegisterTypeEmbedd(LightComponent, graphics::RendererComponent)
	public:
		virtual ~LightComponent();

		void OnCreate() override;

		const LVector4f& GetColor()
		{
			return mColor;
		}

		LVector3f GetPosition() const
		{
			return mTransform->GetPosition();
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
		LVector4f      mColor = LVector4f(0.8f, 0.8f, 0.8f, 1.0f);
		bool           mCastShadow = false;
		float          mIntensity = 1.0f;

		bool           mNeedUpdate = false;

		void UpdateLightBridgeDataBase(GameRenderBridgeDataLight* curData);
		//graphics::Light* mLight      = nullptr;
	};
	
	class GAME_API PointLightComponent : public LightComponent
	{
		RegisterTypeEmbedd(PointLightComponent, LightComponent)
	public:
		void OnCreate() override;
		void OnTransformDirty(Transform* transform);

	private:
		//render相关
		LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameLightRenderDataUpdater>(); }

		LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData) override;
	};
	
	class GAME_API DirectionLightComponent : public LightComponent
	{
		RegisterTypeEmbedd(DirectionLightComponent, LightComponent)
	public:
		void OnCreate() override;
		void OnTransformDirty(Transform* transform);

		//const LMatrix4f& GetProjectionMatrix(int idx)const
		//{
		//	return mProj[idx];
		//}
		const LVector3f GetDirection() const
		{
			return mTransform->ForwardDirection();
		}

		const LMatrix4f GetWorldMatrix()const
		{
			return mTransform->GetLocalToWorldMatrix();
		}
		//const LMatrix4f GetViewMatrix()const
		//{
		//	return mViewMatrix[0];
		//}
		//const LMatrix4f GetViewMatrix(int idx)const
		//{
		//	return mViewMatrix[idx];
		//}
		const LQuaternion GetRotation()const
		{
			return mTransform->GetRotation();
		}

		//void SetProjectionMatrix(const LMatrix4f& val, int csm_index);
		//void SetViewMatrix(const LMatrix4f& val, int csm_index);


		const LVector3f GetCSMSplit() const;
		void SetCSMSplits(const LVector3f& val);

	private:
		//LMatrix4f mViewMatrix[4];
		//LMatrix4f mProj[4];
		LVector3f mCSMSplit = LVector3f(0.2f, 0.5f, 0.7f);
		//float zFar = 100, zNear = 0.01f;
		//float mAspect = 1024.f / 768.f;
	private:
		//render相关
		LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameDirLightRenderDataUpdater>(); }

		LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData) override;


	};

	



}
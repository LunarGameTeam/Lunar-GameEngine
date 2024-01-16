#include "Game/RenderComponent.h"
#include "Graphics/RenderModule.h"
#include "Game/Scene.h"

namespace luna::graphics
{
	bool GameRenderDataUpdater::Create()
	{
		for (int32_t frameIndex = 0; frameIndex < 3; ++frameIndex)
		{
			LSharedPtr<GameRenderBridgeData> newData = GenarateData();
			mRenderBridgeData.push_back(newData);
		}
		mGameLocation = 0;
		mRenderLocation = 2;
		return true;
	}

	GameRenderBridgeData* GameRenderDataUpdater::GetGameThreadBridgeData()
	{
		return mRenderBridgeData[mGameLocation].get();
	}

	void GameRenderDataUpdater::OnGameDataRecordFinish()
	{
		mGameLocation = (mGameLocation + 1) % 3;
	}

	void GameRenderDataUpdater::UpdateRenderThread(RenderScene* curScene)
	{
		UpdateRenderThreadImpl(mRenderBridgeData[mRenderLocation].get(), curScene);
		ClearData(mRenderBridgeData[mRenderLocation].get());
		mRenderLocation = (mRenderLocation + 1) % 3;
	}

	RegisterTypeEmbedd_Imp(RendererComponent)
	{
		cls->Binding<RendererComponent>();

		BindingModule::Get("luna")->AddType(cls);
	}

	RendererComponent::RendererComponent()
	{
		mIsRenderComponent = true;
		mNeedTick = true;
	}

	RendererComponent::~RendererComponent()
	{

	}

	void RendererComponent::OnCreate()
	{
		Component::OnCreate();
		mRenderDataUpdater = GenarateRenderUpdater();
		mRenderDataUpdater->Create();
	}

	void RendererComponent::OnRenderTick(RenderScene* curScene)
	{
		mRenderDataUpdater->UpdateRenderThread(curScene);
	}

	void RendererComponent::OnTick(float delta_time)
	{
		GameRenderBridgeData* curRenderData = mRenderDataUpdater->GetGameThreadBridgeData();
		OnTickImpl(curRenderData);
		mRenderDataUpdater->OnGameDataRecordFinish();
	}


}

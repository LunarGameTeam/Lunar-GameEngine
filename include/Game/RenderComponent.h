#pragma once

#include "Game/GameConfig.h"

#include "Core/Object/Component.h"

#include "Graphics/Renderer/RenderScene.h"

#include <atomic>


namespace luna::graphics
{
	struct GameRenderBridgeData
	{
		/*
		  渲染辅助数据，这个数据在game线程进行填写。但是会在render线程进行使用
		  该数据可能会同时被game和render线程使用，因此尽量不要存储指针类的数据，除非能保证这个指针指向的数据不会每帧变化
		*/
	};
	/*
	  这个类是为了沟通game层和render层，父类会解决数据的跨线程访问的问题。
	  子类只需要补充实现如何使用game层制作好的GameRenderBridgeData去更新renderscene里的renderdata
	  UpdateRenderThreadImpl是render层调用和更新的
	*/
	class GAME_API GameRenderDataUpdater
	{
		int mRenderLocation;

		int mGameLocation;

		LArray<LSharedPtr<GameRenderBridgeData>> mRenderBridgeData;
		/*
		这里可以持有renderdata里面的数据的指针，方便制作刷新这些数据的command，因为renderdata的数据永远只有一份。
		除此之外，这里不要持有任何rhi的数据，如果你制作command需要借助一些辅助的staging buffer，请在UpdateRenderThreadImpl的时候，使用RenderScene上面
		的RenderSceneUploadBufferPool临时去申请cbuffer或者structure buffer即可。
		*/
	public:
		virtual ~GameRenderDataUpdater() {};
		
		bool Create();

		GameRenderBridgeData* GetGameThreadBridgeData();

		void OnGameDataRecordFinish();

		void UpdateRenderThread(RenderScene* curScene);

	private:
		virtual LSharedPtr<GameRenderBridgeData> GenarateData() = 0;

		virtual void UpdateRenderThreadImpl(GameRenderBridgeData* curData,RenderScene* curScene) = 0;

		//这里需要提供一个清空渲染辅助数据的函数，用于每次更新完render data之后重置辅助信息
		virtual void ClearData(GameRenderBridgeData* curData) = 0;
	};

	/*
	  这个类是用于拓展渲染相关的组件
	  如果要制作一个渲染相关的子类组件，需要先制作对应的GameRenderDataUpdater，然后补充组件如何填充GameRenderBridgeData的流程
	*/
	class GAME_API RendererComponent : public Component
	{
		RegisterTypeEmbedd(RendererComponent, Component)

		LSharedPtr<GameRenderDataUpdater> mRenderDataUpdater;

	public:
		void OnTick(float delta_time) override;

		virtual ~RendererComponent();

	private:
		virtual LSharedPtr<GameRenderDataUpdater> GenarateRenderUpdater() = 0;

		virtual void OnTickImpl(GameRenderBridgeData* curRenderData) = 0;
	};

}

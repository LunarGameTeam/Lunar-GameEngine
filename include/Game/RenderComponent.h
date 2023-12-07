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
		  ��Ⱦ�������ݣ����������game�߳̽�����д�����ǻ���render�߳̽���ʹ��
		  �����ݿ��ܻ�ͬʱ��game��render�߳�ʹ�ã���˾�����Ҫ�洢ָ��������ݣ������ܱ�֤���ָ��ָ������ݲ���ÿ֡�仯
		*/
	};
	/*
	  �������Ϊ�˹�ͨgame���render�㣬����������ݵĿ��̷߳��ʵ����⡣
	  ����ֻ��Ҫ����ʵ�����ʹ��game�������õ�GameRenderBridgeDataȥ����renderscene���renderdata
	  UpdateRenderThreadImpl��render����ú͸��µ�
	*/
	class GAME_API GameRenderDataUpdater
	{
		int mRenderLocation;

		int mGameLocation;

		LArray<LSharedPtr<GameRenderBridgeData>> mRenderBridgeData;
		/*
		������Գ���renderdata��������ݵ�ָ�룬��������ˢ����Щ���ݵ�command����Ϊrenderdata��������Զֻ��һ�ݡ�
		����֮�⣬���ﲻҪ�����κ�rhi�����ݣ����������command��Ҫ����һЩ������staging buffer������UpdateRenderThreadImpl��ʱ��ʹ��RenderScene����
		��RenderSceneUploadBufferPool��ʱȥ����cbuffer����structure buffer���ɡ�
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

		//������Ҫ�ṩһ�������Ⱦ�������ݵĺ���������ÿ�θ�����render data֮�����ø�����Ϣ
		virtual void ClearData(GameRenderBridgeData* curData) = 0;
	};

	/*
	  �������������չ��Ⱦ��ص����
	  ���Ҫ����һ����Ⱦ��ص������������Ҫ��������Ӧ��GameRenderDataUpdater��Ȼ�󲹳����������GameRenderBridgeData������
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

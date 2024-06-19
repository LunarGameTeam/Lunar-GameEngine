/*!
 * \file render_subsystem.h
 * \date 2020/08/09 10:39
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief RenderSubSystem，渲染子模块，执行了渲染流程
 *
 * TODO: long description
 *
 * \note
*/
#pragma once


#include "Core/Framework/Module.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Event/EventModule.h"


#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/ImguiRenderer.h"
#include "Graphics/RHI/RHITypes.h"


namespace luna::graphics
{



class RENDER_API RenderModule : public LModule
{
	RegisterTypeEmbedd(RenderModule, LModule)
public:
	RenderModule();
public:
	RenderScene* AddScene();
	void RemoveScene(RenderScene*);

	RHISwapChainPtr& GetSwapChain()
	{
		return mMainSwapchain;
	}

	inline RHIRenderQueue* GetCmdQueueCore()
	{
		return mGraphicQueue;
	}

	ImguiRenderer* GetGuiRenderer() { return mGuiRenderer.get(); };
public:
	
	bool OnLoad() override;	
	bool OnInit() override;
	bool OnShutdown() override;

	void Tick(float deltaTime) override;
	void RenderTick(float delta_time) override;
protected:
	RHICmdList* PrepareRender();

	void OnMainWindowResize(LWindow& window, WindowEvent& event);
public:
	LSharedPtr<Texture2D>              mDefaultWhiteTexture;
	LSharedPtr<Texture2D>              mDefaultNormalTexture;
private:
	LSharedPtr<ImguiRenderer> mGuiRenderer;
	FrameGraphBuilder* mFrameGraphBuilder;

	RHISinglePoolSingleCmdListPtr mGraphicCmd;

	RHIRenderQueuePtr    mGraphicQueue;

	uint64_t             mFrameFenceValue = 0;
	RHIFencePtr          mFrameFence;


	LArray<RenderScene*>      mRenderScenes;

	graphics::RHISwapchainDesc         mSwapchainDesc;

	RHISwapChainPtr                    mMainSwapchain;

	graphics::RHIDevicePtr             mRhiDevice;
	bool                               mNeedResizeSwapchain = false;
};

}
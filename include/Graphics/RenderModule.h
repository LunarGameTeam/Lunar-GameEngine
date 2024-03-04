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

#include "Graphics/Renderer/ImGuiTexture.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/Renderer/SceneRenderer.h"
#include "Graphics/Renderer/ImguiRenderer.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RenderAssetManager/RenderAssetManager.h"

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

	template <typename T>
	T* GetDevice()
	{
		return static_cast<T*>(mRenderContext->mDevice.get());
	};

	RHIDevice* GetRHIDevice()
	{
		return mRenderContext->mDevice.get();
	}

	RenderResourceGenerateHelper* GetRenderContext()
	{
		return mRenderContext;
	}

	RenderCommandGenerateHelper* GetRenderCommandHelper()
	{
		return mRenderCommandEncoder;
	}

	inline RHIRenderQueue* GetCmdQueueCore()
	{
		return mRenderContext->mGraphicQueue;
	}

	RenderDeviceType GetDeviceType() { return mRenderContext->mDeviceType; }

	RenderAssetDataManager* GetAssetManager() { return &mRenderAssetManager; };

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
	RenderResourceGenerateHelper*      mRenderContext;
	RenderCommandGenerateHelper*       mRenderCommandEncoder;
	LSharedPtr<Texture2D>              mDefaultWhiteTexture;
	LSharedPtr<Texture2D>              mDefaultNormalTexture;
private:
	LSharedPtr<ImguiRenderer> mGuiRenderer;

	LSharedPtr<SceneRenderer> mRenderer;

	RHISinglePoolSingleCmdListPtr mGraphicCmd;

	uint64_t             mFrameFenceValue = 0;
	RHIFencePtr          mFrameFence;


	LArray<RenderScene*>      mRenderScenes;

	graphics::RHISwapchainDesc         mSwapchainDesc;

	RHISwapChainPtr                    mMainSwapchain;

	bool                               mNeedResizeSwapchain = false;

	RenderAssetDataManager             mRenderAssetManager;
};

}
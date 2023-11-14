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

#include "Graphics/RHI/RHITypes.h"


namespace luna::graphics
{

class RENDER_API RenderModule : public LModule
{
	RegisterTypeEmbedd(RenderModule, LModule)
public:
	RenderModule();
	void OnIMGUI() override;
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
		return mRenderContext->mDevice;
	}

	RenderContext* GetRenderContext()
	{
		return mRenderContext;
	}

	inline RHIRenderQueue* GetCmdQueueCore()
	{
		return mRenderContext->mGraphicQueue;
	}

	RenderDeviceType GetDeviceType() { return mRenderContext->mDeviceType; }

public:
	
	bool OnLoad() override;	
	bool OnInit() override;
	void SetupIMGUI();
	bool OnShutdown() override;

	void Tick(float deltaTime) override;
	void RenderTick(float delta_time) override;
	void UpdateFrameBuffer();
protected:
	void Render();
	void RenderIMGUI();

	void OnMainWindowResize(LWindow& window, WindowEvent& event);
public:
	RenderContext*      mRenderContext;
	TPPtr<RenderTarget> mMainRT;
	RenderMeshBase      mFullscreenMesh;
	ImguiTexture* GetImguiTexture(RHIResource* key);
	ImguiTexture* AddImguiTexture(RHIResource* res);
	bool          IsImuiTexture(RHIResource* key);
	LSharedPtr<Texture2D>              mDefaultWhiteTexture;
	LSharedPtr<Texture2D>              mDefaultNormalTexture;
private:
	SceneRenderer mRenderer;
	LArray<RenderScene*>               mRenderScenes;
	LMap<RHIResourcePtr, ImguiTexture> mImguiTextures;

	graphics::RHIRenderPassPtr           mIMGUIRenderPass;
	graphics::RHIFrameBufferPtr          mFrameBuffer[2];
	graphics::RHISwapchainDesc           mSwapchainDesc;

	RHISwapChainPtr                    mMainSwapchain;

	bool                               mNeedResizeSwapchain = false;

	std::atomic_bool                   mLogicUpdated = false;
};
}
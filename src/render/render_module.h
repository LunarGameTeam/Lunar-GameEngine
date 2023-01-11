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


#include "core/core_library.h"
#include "render/render_config.h"

#include "render/renderer/render_scene.h"
#include "render/renderer/render_pass.h"

#include "render/asset/mesh_asset.h"

#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_memory.h"
#include "render/renderer/render_device.h"
#include "render/renderer/render_scene.h"
#include "render/rhi/rhi_types.h"

#include <vulkan//vulkan.h>
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"
#include "renderer/imgui_texture.h"
#include "core/event/event_module.h"

namespace luna::render
{

class RenderDevice;


class RENDER_API RenderModule : public LModule
{
	RegisterTypeEmbedd(RenderModule, LModule)
public:
	RenderModule();


	void OnIMGUI() override;

	//Renderer
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
		return static_cast<T*>(mRenderDevice->mDevice);
	};

	RHIDevice* GetRHIDevice()
	{
		return mRenderDevice->mDevice;
	}

	RenderDevice* GetRenderDevice()
	{
		return mRenderDevice;
	}

	inline RHIRenderQueue* GetCmdQueueCore()
	{
		return mRenderDevice->mGraphicQueue;
	}

	RenderDeviceType GetDeviceType() { return mRenderDevice->mDeviceType; }
	LSharedPtr<ShaderAsset> mDefaultShader;


	RenderScene* GetRenderSceneAt(int32_t idx)
	{
		if (idx < mRenderScenes.size())
			return mRenderScenes[idx];
		return nullptr;
	}

	size_t GetRenderSeneSize() { return mRenderScenes.size(); }

public:
	bool OnLoad() override;	
	bool OnInit() override;
	void SetupIMGUI();
	bool OnShutdown() override;

	void OnFrameBegin(float delta_time) override;
	void Tick(float delta_time) override;

	void OnFrameEnd(float deltaTime) override;
	void UpdateFrameBuffer();
protected:
	void Render();
	void RenderIMGUI();

	void OnMainWindowResize(LWindow& window, WindowEvent& event);
public:

	RenderDevice* mRenderDevice;
	TPPtr<RenderTarget>     mMainRT;

	ImguiTexture* GetImguiTexture(RHIResource* key) { return &mImguiTextures[key]; }
	ImguiTexture* AddImguiTexture(RHIResource* res);
	bool IsImuiTexture(RHIResource* key) { return mImguiTextures.find(key) != mImguiTextures.end(); }
private:
	LSharedPtr<Texture2D>       mDefaultWhiteTexture;

	LArray<RenderScene*>        mRenderScenes;
	//LMap<LWindow*, RHISwapChainPtr> mSwapchains;

	LMap<RHIResourcePtr, ImguiTexture> mImguiTextures;

	render::RHIRenderPassPtr  mRenderPass;
	render::RHIFrameBufferPtr mFrameBuffer[2];
	render::RHISwapchainDesc  mSwapchainDesc;

	RHISwapChainPtr                    mMainSwapchain;
	//framegraph
	FrameGraphBuilder*                 mFrameGraph          = nullptr;

	bool                               mNeedResizeSwapchain = false;
};
}
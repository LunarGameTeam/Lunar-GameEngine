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
#include "render/pch.h"

#include "render/renderer/render_obj.h"
#include "render/renderer/render_pass.h"

#include "render/asset/mesh_asset.h"

#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_memory.h"
#include "render/renderer/render_device.h"
#include "render/renderer/render_scene.h"

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

	RHISwapChainPtr& GetMainSwapchain()
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
public:
	bool OnInit() override;
	bool OnShutdown() override;

	void OnFrameBegin(float delta_time) override;
	void Tick(float delta_time) override;
	void OnFrameEnd(float deltaTime) override;

	RenderScene* GetRenderScene(int32_t scene_id) { return mRenderScenes[scene_id]; }
	int32_t GetRenderSeneSize() { return (int32_t)mRenderScenes.Size(); }
public:
	RenderDevice* mRenderDevice;
	TSubPtr<RenderTarget>     mMainRT;
	
private:
	LSharedPtr<Texture2D> mDefaultWhiteTexture;

	RenderDeviceType          mDeviceType = RenderDeviceType::DirectX12;

	TSubPtrArray<RenderScene> mRenderScenes;	//主交换链

	RHISwapChainPtr           mMainSwapchain;
	//framegraph
	FrameGraphBuilder* mFrameGraph = nullptr;
};
}

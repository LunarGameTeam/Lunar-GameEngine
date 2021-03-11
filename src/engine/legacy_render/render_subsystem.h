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

#include "core/core_module.h"
#include "private_render.h"

#include "legacy_render/interface/i_shader.h"
#include "legacy_render/interface/i_camera.h"
#include "legacy_render/asset/mesh.h"
#include "legacy_render/component/renderer.h"
#include "legacy_render/d3d11/d3d11_device.h"
#include "legacy_render/d3d11/node.h"

namespace luna
{
namespace legacy_render
{


class LEGACY_RENDER_API RenderSubusystem : public SubSystem, public IDeviceNotify
{
public:
	RenderSubusystem();;

	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	virtual void OnDeviceLost();
	virtual void OnDeviceRestored();

	DeviceResources *GetDevice()
	{
		return m_deviceResources.get();
	}

	GET_SET_VALUE(ICamera*, m_main_camera, Camera);

	void RegisterRenderer(RendererComponent * renderer);
	void UnRegisterRenderer(RendererComponent *renderer);

private:
	ICamera *m_main_camera;
	LMap<RendererComponent *, RenderNode*> m_renderers;
	LVector<LSharedPtr<DX11RenderNode>> m_nodes;

	void Clear();

	std::unique_ptr<DeviceResources>	m_deviceResources;
};

}
}
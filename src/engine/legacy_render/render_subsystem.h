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

#include <directxmath.h>

#include "core/core_module.h"
#include "private_render.h"

#include "legacy_render/interface/i_shader.h"
#include "legacy_render/interface/i_camera.h"
#include "legacy_render/d3d11/Mesh.h"
#include "legacy_render/d3d11/d3d11_device.h"

namespace luna
{
namespace legacy_render
{


class LEGACY_RENDER_API RenderSubusystem : public SubSystem, public DX::IDeviceNotify
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

	DX::DeviceResources *GetDevice()
	{
		return m_deviceResources.get();
	}

	GET_SET_VALUE(ICamera*, m_main_camera, Camera);

private:
	IShader *shader;
	ICamera *m_main_camera;
	LSharedPtr<Mesh> model;

	void Clear();
	// DirectXTK objects.
	std::unique_ptr<DX::DeviceResources>	m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_batchInputLayout;
};

}
}
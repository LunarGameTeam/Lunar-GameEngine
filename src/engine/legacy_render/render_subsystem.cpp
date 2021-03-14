#include "render_subsystem.h"

#include "core/asset/asset_subsystem.h"
#include "core/object/transform.h"
#include "core/object/entity.h"

#include "window/window_subsystem.h"

#include "legacy_render/component/renderer.h"

#include "interface/i_camera.h"

#include "d3d11/d3d11_device.h"
#include "d3d11/shader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace luna
{
namespace legacy_render
{

LEGACY_RENDER_API RenderSubusystem *g_render_sys = nullptr;

RenderSubusystem::RenderSubusystem()
{
	m_need_tick = true;
	g_render_sys = this;
}

bool RenderSubusystem::OnPreInit()
{
	return true;
}

bool RenderSubusystem::OnPostInit()
{
	return true;
}

bool RenderSubusystem::OnInit()
{
	static AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();

	m_deviceResources = std::make_unique<DeviceResources>(
		DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 2,
		D3D_FEATURE_LEVEL_9_1);
	m_deviceResources->RegisterDeviceNotify(this);
	LWindow *main_window = gEngine->GetSubsystem<luna::WindowSubsystem>()->GetMainWindow();
	auto window = main_window->GetWindow();
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;
	m_deviceResources->SetWindow(hwnd, main_window->GetWindowWidth(), main_window->GetWindowHeight());
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();
	bool ret = true;
	return true;
}

bool RenderSubusystem::OnShutdown()
{
	return true;
}

void RenderSubusystem::Tick(float delta_time)
{
	for (auto it : m_renderers)
	{
		if (it.first->GetRendererDirty())
		{
			auto &render_node = *it.second.first;
			auto &dx_node = *it.second.second;
			it.first->PopulateRenderNode(*it.second.first);
			
			dx_node.mesh->Update(render_node.mesh.get());
			dx_node.texture->Update(render_node.material->GetTexture2D().get());
			dx_node.shader = render_node.material->GetShader();
			//dx_node.world_matrix = &(it.first->GetEntity()->GetComponent<Transform>()->GetCachedMatrix());
			it.first->SetRendererDirty(false);
		}
	}
	Clear();
	auto context = m_deviceResources->GetD3DDeviceContext();
	
	if (m_main_camera)
	{
		for (DX11RenderNode *dx_node : m_nodes)
		{
			dx_node->shader->Bind();
			auto *device = GetDevice()->GetD3DDevice();
			auto *context = GetDevice()->GetD3DDeviceContext();
			unsigned int stride;
			unsigned int offset;


			// Set vertex buffer stride and offset.
			stride = sizeof(BaseVertex);
			offset = 0;
			
			// Set the vertex buffer to active in the input assembler so it can be rendered.
			context->IASetVertexBuffers(0, 1, &dx_node->mesh->vertex_buffer, &stride, &offset);
			// Set the index buffer to active in the input assembler so it can be rendered.
			context->IASetIndexBuffer(dx_node->mesh->index_buffer, DXGI_FORMAT_R32_UINT, 0);
			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			dx_node->shader->SetWVPMatrix(*dx_node->world_matrix,m_main_camera->GetViewMatrix(), m_main_camera->GetProjectionMatrix());
			context->DrawIndexed(dx_node->mesh->num, 0, (UINT)0);
			
		}
// 		model->Bind();
// 		model->Draw();
	}
	// Show the new frame.
	m_deviceResources->Present();
}

void RenderSubusystem::OnDeviceLost()
{

}

void RenderSubusystem::OnDeviceRestored()
{

}

void RenderSubusystem::RegisterRenderer(RendererComponent *renderer)
{
	if (m_renderers.find(renderer) == m_renderers.end())
	{
		m_renderers[renderer] = std::make_pair<RenderNode *, DX11RenderNode *>(new RenderNode(), new DX11RenderNode());
		m_nodes.push_back(m_renderers[renderer].second);
	}
}

void RenderSubusystem::UnRegisterRenderer(RendererComponent *renderer)
{
	if (m_renderers.find(renderer) != m_renderers.end())
	{
		delete m_renderers[renderer].first;
		delete m_renderers[renderer].second;
		m_renderers.erase(renderer);
	}
}

void RenderSubusystem::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

}
}



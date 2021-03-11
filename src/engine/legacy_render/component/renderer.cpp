#include "renderer.h"
#include "legacy_render/render_subsystem.h"
namespace luna
{
namespace legacy_render
{

void RendererComponent::OnCreate()
{
	g_render_sys->RegisterRenderer(this);
}

void RendererComponent::OnDestroy()
{
	g_render_sys->UnRegisterRenderer(this);
}

void RendererComponent::OnEnable()
{
}

void RendererComponent::OnDisable()
{
}

void RendererComponent::OnTick(float delta_time)
{
}

}
}
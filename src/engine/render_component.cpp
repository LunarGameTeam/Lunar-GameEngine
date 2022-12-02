#include "render_component.h"
#include "render/render_module.h"
#include "engine/scene.h"

namespace luna::render
{

RegisterTypeEmbedd_Imp(RendererComponent)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);

	cls->Property<&RendererComponent::m_receive_shadow>("receive_shadow")
		.Setter<&RendererComponent::SetReceiveLight>();
	cls->Property<&RendererComponent::m_cast_shadow>("cast_shadow").
		Setter<&RendererComponent::SetCastShadow>();
}

void RendererComponent::OnCreate()
{
	Component::OnCreate();
}

void RendererComponent::OnDestroy()
{
	Component::OnDestroy();
}

void RendererComponent::OnActivate()
{
	Component::OnActivate();
}

void RendererComponent::OnDeactivate()
{
	Component::OnDeactivate();
}

void RendererComponent::OnTick(float delta_time)
{
}

}

#include "Game/RenderComponent.h"
#include "Graphics/RenderModule.h"
#include "Game/Scene.h"

namespace luna::graphics
{

RegisterTypeEmbedd_Imp(RendererComponent)
{
	cls->Binding<RendererComponent>();

	cls->BindingProperty<&RendererComponent::m_receive_shadow>("receive_shadow")
		.Setter<&RendererComponent::SetReceiveLight>()
		.Serialize();

	cls->BindingProperty<&RendererComponent::m_cast_shadow>("cast_shadow")
		.Setter<&RendererComponent::SetCastShadow>()
		.Serialize();

	BindingModule::Get("luna")->AddType(cls);
}

RendererComponent::~RendererComponent()
{

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

#pragma once

#include "Game/GameConfig.h"

#include "Core/Object/Component.h"

#include "Graphics/Renderer/RenderScene.h"




namespace luna::render
{
class GAME_API RendererComponent : public Component
{
	RegisterTypeEmbedd(RendererComponent, Component)
public:
	virtual ~RendererComponent();

	GET_SET_VALUE(bool, m_dirty, RendererDirty);
	GET_SET_VALUE_DIRTY(bool, m_cast_shadow, CastShadow, m_dirty);
	GET_SET_VALUE_DIRTY(bool, m_receive_light, ReceiveLight, m_dirty);

public:
	void OnCreate() override;
	void OnDestroy() override;
	void OnActivate() override;
	void OnDeactivate() override;
	void OnTick(float delta_time) override;

protected:
	bool m_dirty;

private:
	bool m_cast_shadow = true;
	bool m_receive_shadow = true;
	bool m_receive_light = true;
};
}

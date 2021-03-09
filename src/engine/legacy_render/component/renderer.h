#pragma once

#include "legacy_render/private_render.h"
#include "core/core_module.h"
#include "core/object/component.h"


namespace luna
{
namespace legacy_render
{

class LEGACY_RENDER_API RendererComponent : public Component
{
public:
	void OnCreate() override;
	void OnDestroy() override;
	void OnEnable() override;
	void OnDisable() override;

public:
	virtual void Bind() = 0;
	void OnTick(float delta_time) override;

};

}
}
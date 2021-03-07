#pragma once

#include "core/core_module.h"
#include "world/component.h"

#include "legacy_render/interface/i_camera.h"

namespace luna
{

class WORLD_API CameraComponent : public Component, public ICamera
{
public:
	void SetAsMainCamera();
};

}
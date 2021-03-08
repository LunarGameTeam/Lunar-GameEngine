#pragma once

#include "core/math/math.h"
#include <Eigen/Geometry>
#include "window/luna_window.h"
#include "core/event/event_subsystem.h"

namespace luna
{

class ICamera
{
public:
	virtual LMatrix4f &GetViewMatrix() = 0;
	virtual LMatrix4f &GetProjectionMatrix() = 0;

};

}

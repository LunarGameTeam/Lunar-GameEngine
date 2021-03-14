#pragma once

#include "core/memory/ptr.h"
#include "legacy_render/private_render.h"
#include "legacy_render/asset/mesh.h"
#include "legacy_render/asset/material.h"
#include "legacy_render/asset/texture.h"

namespace luna
{
namespace legacy_render
{

struct LEGACY_RENDER_API RenderNode
{
	LSharedPtr<Mesh> mesh;
	LSharedPtr<Material> material;
};

}
}

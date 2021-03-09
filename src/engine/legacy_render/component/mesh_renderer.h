#pragma once

#include "legacy_render/private_render.h"
#include "legacy_render/component/renderer.h"

namespace luna
{

class LEGACY_RENDER_API MeshRenderer : public RendererComponent
{

};

class LEGACY_RENDER_API StaticMeshRenderer : public MeshRenderer
{

};

class LEGACY_RENDER_API SkeletonMeshRenderer : public MeshRenderer
{

};


}
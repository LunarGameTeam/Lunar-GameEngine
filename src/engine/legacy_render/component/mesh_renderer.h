#pragma once

#include "legacy_render/private_render.h"
#include "legacy_render/component/renderer.h"
#include "legacy_render/asset/mesh.h"
#include "legacy_render/asset/texture.h"
#include "legacy_render/asset/material.h"

namespace luna
{
namespace legacy_render
{

class LEGACY_RENDER_API MeshRenderer : public RendererComponent
{
public:

private:
	LSharedPtr<Mesh> m_mesh;
	LSharedPtr<Material> m_material;
};

class LEGACY_RENDER_API StaticMeshRenderer : public MeshRenderer
{

};

class LEGACY_RENDER_API SkeletonMeshRenderer : public MeshRenderer
{

};

}


}
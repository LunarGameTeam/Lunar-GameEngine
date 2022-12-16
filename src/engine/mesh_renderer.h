#pragma once

#include "render/render_config.h"
#include "render/asset/mesh_asset.h"
#include "render/asset/texture_asset.h"

#include "engine/render_component.h"


namespace luna::render
{

class ENGINE_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		m_mesh(this),
		m_material(this)
	{
	}

	bool PopulateRenderNode(RenderObject& render_nodes) override;

	void SetMesh(MeshAsset* mesh)
	{
		m_mesh.SetPtr(mesh);
		m_dirty = true;
	}

	void SetMaterial(MaterialInstance* mat)
	{
		m_material.SetPtr(mat);
		m_dirty = true;
	}

	void OnCreate() override;


	void OnActivate() override;

private:
	TSubPtr<MeshAsset> m_mesh;
	TSubPtr<MaterialInstance> m_material;
};


class RENDER_API StaticMeshRenderer : public MeshRenderer
{
};


class RENDER_API SkeletonMeshRenderer : public MeshRenderer
{
};

}

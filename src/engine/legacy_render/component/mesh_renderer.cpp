#include "mesh_renderer.h"
namespace luna
{
namespace legacy_render
{

void MeshRenderer::PopulateRenderNode(RenderNode &render_nodes)
{
	render_nodes.material = m_material;
	render_nodes.mesh = m_mesh;
}


}
}
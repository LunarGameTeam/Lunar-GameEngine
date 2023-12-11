#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"

namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);
class OpaquePassGenerator : public FrameGraphMeshPassGenerator
{
public:
	OpaquePassGenerator();

	void AddPassNode(FrameGraphBuilder* builder, RenderView* view) override;
private:
	bool CheckRenderObject(const RenderObject* curRo) const override;

	MaterialInstanceGraphBase* SetMaterialByRenderObject(const RenderObject* curRo) const override;
};

}


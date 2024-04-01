#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/Asset/MeshAsset.h"
namespace luna::graphics
{
class SceneOverLayPassGenerator : public FrameGraphPassGeneratorPerView
{
	RenderAssetDataMesh* mDebugMeshLineData = nullptr;

	RenderAssetDataMesh* mDebugMeshData = nullptr;

	SharedPtr<MaterialGraphAsset> mOverlayMtlAsset = nullptr;

	MaterialInstanceGraphBase* mOverlayMtlInstance = nullptr;
public:
	SceneOverLayPassGenerator();

	void AddPassNode(FrameGraphBuilder* builder, RenderView* view) override;
private:
	bool CheckRenderObject(const RenderObject* curRo) const override;
};
}


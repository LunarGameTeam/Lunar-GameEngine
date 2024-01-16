#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/Asset/MeshAsset.h"
namespace luna::graphics
{
class SkyBoxPassGenerator : public FrameGraphPassGeneratorPerView
{
	SharedPtr<MeshAsset> mSkyBoxMeshAsset = nullptr;

	RenderAssetDataMesh* mSkyboxRenderMesh = nullptr;

	SharedPtr<MaterialGraphAsset> mSkyBoxMtlAsset = nullptr;

	MaterialInstanceGraphBase* mSkyBoxDefaultMtlInstance = nullptr;
public:
	SkyBoxPassGenerator();

	void AddPassNode(FrameGraphBuilder* builder, RenderView* view) override;
private:
	bool CheckRenderObject(const RenderObject* curRo) const override;
};

}


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

	const size_t GetRenderQueueSize() const override { return 1; };
private:
	bool CheckRenderObject(const RenderObject* curRo) const override;
};

}


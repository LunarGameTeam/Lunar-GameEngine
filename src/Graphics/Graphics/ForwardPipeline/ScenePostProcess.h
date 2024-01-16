#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/Asset/MeshAsset.h"
namespace luna::graphics
{
	class PostProcessPassGenerator : public FrameGraphPassGeneratorPerView
	{
		SharedPtr<MaterialGraphAsset> mGammaCorrectionMtlAsset = nullptr;

		MaterialInstanceGraphBase* mGammaCorrectionMtlInstance = nullptr;
	public:
		PostProcessPassGenerator();

		void AddPassNode(FrameGraphBuilder* builder, RenderView* view) override;
	private:
		bool CheckRenderObject(const RenderObject* curRo) const override;
	};
}
#pragma once
#include "GltfImporter/GltfImporterCommon.h"
namespace luna::lgltf
{
	class LGltfImporterMesh : public LGltfImporterBase
	{
	public:
		LGltfImporterMesh() {};
	private:
		void ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene) override;
	};
}
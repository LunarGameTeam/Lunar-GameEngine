#pragma once
#include "gltf_importer_common.h"
#include "../gltf_loader/gltf_mesh_loader.h"
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
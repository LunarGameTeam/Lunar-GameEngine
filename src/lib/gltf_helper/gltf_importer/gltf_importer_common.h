#pragma once
#include "../gltf_loader/gltf_loader_util.h"
#include "../../import_scene/import_scene.h"
namespace luna::lgltf
{
	class LGltfImporterBase
	{
	public:
		LGltfImporterBase() {};
		void ParsingData(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene) = 0;
	};
}
#pragma once
#include "gltf_importer_common.h"
namespace luna::lgltf
{
	void LGltfImporterBase::ParsingData(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene)
	{
		ParsingDataImpl(gltfDataInput, gltfNodeInput,outputScene);
	}
}
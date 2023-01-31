#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfImporterCommon.h"
namespace luna::asset
{

class ASSET_IMPORT_API LGltfImporterMesh : public LGltfImporterBase
{
public:
	LGltfImporterMesh() {};
private:
	void ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, LImportScene& outputScene) override;
};

}
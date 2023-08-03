#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfImporterCommon.h"
#include "AssetImport/GltfParser/GltfLoader/GltfSceneLoader.h"
namespace luna::asset
{
	class ASSET_IMPORT_API LGltfSceneImportHelper
	{
		LUnorderedMap<asset::LImportNodeDataType, std::shared_ptr<LGltfImporterBase>> mImporters;
	public:
		LGltfSceneImportHelper();
		void ParseScene(const LGltfSceneData* gltfDataInput, asset::LImportScene& outputScene);
	private:
		void ParseSceneData(
			asset::LImportNodeDataType type,
			const LGltfDataBase* gltfDataInput,
			const LArray<size_t>& gltfNodesInput,
			LGltfImportContext &dataContext,
			asset::LImportScene& outputScene
		);
		asset::LImportNodeDataType GetTypeByGltfType(LGltfDataType inType);
	};

}


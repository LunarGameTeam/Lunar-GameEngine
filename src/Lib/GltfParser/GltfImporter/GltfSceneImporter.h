#pragma once
#include "GltfImporter/GltfImporterCommon.h"
#include "GltfLoader/GltfSceneLoader.h"
namespace luna::lgltf
{
	class LGltfSceneImportHelper
	{
		LUnorderedMap<resimport::LImportNodeDataType, std::shared_ptr<LGltfImporterBase>> mImporters;
	public:
		LGltfSceneImportHelper();
		void ParseScene(const LGltfSceneData* gltfDataInput, resimport::LImportScene& outputScene);
	private:
		void ParseSceneData(
			resimport::LImportNodeDataType type,
			const LGltfDataBase* gltfDataInput,
			const LGltfNodeBase& gltfNodeInput,
			resimport::LImportScene& outputScene
		);
		resimport::LImportNodeDataType GetTypeByGltfType(LGltfDataType inType);
	};

	static std::shared_ptr<LGltfSceneImportHelper> instanceGltfSceneImport = std::make_shared<LGltfSceneImportHelper>();
}


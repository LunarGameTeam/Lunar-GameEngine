#pragma once
#include "AssetImport/ObjParser/ObjImporter/ObjImporterCommon.h"
#include "AssetImport/ObjParser/ObjLoader/ObjSceneLoader.h"
namespace luna::asset
{
	class LObjSceneImportHelper
	{
		LUnorderedMap<asset::LImportNodeDataType, std::shared_ptr<LObjImporterBase>> mImporters;
	public:
		LObjSceneImportHelper();
		void ParseScene(const LObjSceneData* objDataInput, asset::LImportScene& outputScene);
	private:
		void ParseSceneData(
			asset::LImportNodeDataType type,
			const LObjDataBase* objDataInput,
			asset::LImportScene& outputScene
		);
		asset::LImportNodeDataType GetTypeByObjType(LObjDataType inType);
	};

	static std::shared_ptr<LObjSceneImportHelper> instanceObjSceneImport = std::make_shared<LObjSceneImportHelper>();
}


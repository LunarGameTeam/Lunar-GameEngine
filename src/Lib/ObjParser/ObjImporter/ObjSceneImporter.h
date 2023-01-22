#pragma once
#include "ObjImporter/ObjImporterCommon.h"
#include "ObjLoader/ObjSceneLoader.h"
namespace luna::lobj
{
	class LObjSceneImportHelper
	{
		LUnorderedMap<resimport::LImportNodeDataType, std::shared_ptr<LObjImporterBase>> mImporters;
	public:
		LObjSceneImportHelper();
		void ParseScene(const LObjSceneData* objDataInput, resimport::LImportScene& outputScene);
	private:
		void ParseSceneData(
			resimport::LImportNodeDataType type,
			const LObjDataBase* objDataInput,
			resimport::LImportScene& outputScene
		);
		resimport::LImportNodeDataType GetTypeByObjType(LObjDataType inType);
	};

	static std::shared_ptr<LObjSceneImportHelper> instanceObjSceneImport = std::make_shared<LObjSceneImportHelper>();
}


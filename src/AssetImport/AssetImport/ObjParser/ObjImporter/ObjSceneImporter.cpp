#include "AssetImport/ObjParser/ObjImporter/ObjSceneImporter.h"
#include "AssetImport/ObjParser/ObjImporter/ObjMeshImporter.h"
#include "AssetImport/ObjParser/ObjImporter/ObjMaterialImporter.h"
namespace luna::asset
{
	LObjSceneImportHelper::LObjSceneImportHelper()
	{
		mImporters.insert(std::pair<
			asset::LImportNodeDataType, 
			std::shared_ptr<LObjImporterBase>
		>
			(
				asset::LImportNodeDataType::ImportDataMesh,
				std::make_shared<LObjImporterMesh>()
			)
		);
	}

	void LObjSceneImportHelper::ParseScene(const LObjSceneData* objDataInput, asset::LImportScene& outputScene)
	{
		outputScene.SetAxisAndUnit(asset::ImportAxisYupLeftHand,asset::ImportUnitMeter);
		for (auto &eachData : objDataInput->mDatas)
		{
			LObjDataType dataType = eachData->GetType();
			ParseSceneData(GetTypeByObjType(dataType), eachData.get(), outputScene);
		}
	}

	void LObjSceneImportHelper::ParseSceneData(
		asset::LImportNodeDataType type,
		const LObjDataBase* objDataInput,
		asset::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(objDataInput, outputScene);
	}

	asset::LImportNodeDataType LObjSceneImportHelper::GetTypeByObjType(LObjDataType inType)
	{
		switch (inType)
		{
		case luna::asset::ObjMeshData:
			return asset::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::asset::ObjMaterialData:
			return asset::LImportNodeDataType::ImportDataMaterial;
			break;
		default:
			assert(0);
			break;
		}
		return asset::LImportNodeDataType::ImportDataMesh;
	}

}
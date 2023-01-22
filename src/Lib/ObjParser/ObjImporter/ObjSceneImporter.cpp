#include "ObjImporter/ObjSceneImporter.h"
#include "ObjImporter/ObjMeshImporter.h"
#include "ObjImporter/ObjMaterialImporter.h"
namespace luna::lobj
{
	LObjSceneImportHelper::LObjSceneImportHelper()
	{
		mImporters.insert(std::pair<
			resimport::LImportNodeDataType, 
			std::shared_ptr<LObjImporterBase>
		>
			(
				resimport::LImportNodeDataType::ImportDataMesh,
				std::make_shared<LObjImporterMesh>()
			)
		);
	}

	void LObjSceneImportHelper::ParseScene(const LObjSceneData* objDataInput, resimport::LImportScene& outputScene)
	{
		outputScene.SetAxisAndUnit(resimport::ImportAxisYupLeftHand,resimport::ImportUnitMeter);
		for (auto &eachData : objDataInput->mDatas)
		{
			LObjDataType dataType = eachData->GetType();
			ParseSceneData(GetTypeByObjType(dataType), eachData.get(), outputScene);
		}
	}

	void LObjSceneImportHelper::ParseSceneData(
		resimport::LImportNodeDataType type,
		const LObjDataBase* objDataInput,
		resimport::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(objDataInput, outputScene);
	}

	resimport::LImportNodeDataType LObjSceneImportHelper::GetTypeByObjType(LObjDataType inType)
	{
		switch (inType)
		{
		case luna::lobj::ObjMeshData:
			return resimport::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::lobj::ObjMaterialData:
			return resimport::LImportNodeDataType::ImportDataMaterial;
			break;
		default:
			assert(0);
			break;
		}
		return resimport::LImportNodeDataType::ImportDataMesh;
	}

}
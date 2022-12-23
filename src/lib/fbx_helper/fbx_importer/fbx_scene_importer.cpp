#include "fbx_scene_importer.h"
#include "fbx_mesh_importer.h"
#include "fbx_skeleton_importer.h"
#include "fbx_animation_importer.h"
#include "fbx_material_importer.h"
#include "fbx_camera_importer.h"
namespace luna::lfbx
{
	LFbxSceneImportHelper::LFbxSceneImportHelper()
	{
		mImporters.insert(std::pair<
			ImportData::LImportNodeDataType, 
			std::shared_ptr<LFbxImporterBase>
		>
			(
				ImportData::LImportNodeDataType::ImportDataMesh,
				std::make_shared<LFbxImporterMesh>()
			)
		);
	}

	void LFbxSceneImportHelper::ParseScene(const LFbxSceneData* fbxDataInput, ImportData::LImportScene& outputScene)
	{
		for (auto &eachData : fbxDataInput->mDatas)
		{
			LFbxDataType dataType = eachData->GetType();
			ParseSceneData(GetTypeByFbxType(dataType), eachData.get(), outputScene);
		}
	}

	void LFbxSceneImportHelper::ParseSceneData(ImportData::LImportNodeDataType type, const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(fbxDataInput, outputScene);
	}

	ImportData::LImportNodeDataType LFbxSceneImportHelper::GetTypeByFbxType(LFbxDataType inType)
	{
		switch (inType)
		{
		case luna::lfbx::FbxMeshData:
			return ImportData::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::lfbx::FbxMaterialData:
			return ImportData::LImportNodeDataType::ImportDataMaterial;
			break;
		case luna::lfbx::FbxCameraData:
			return ImportData::LImportNodeDataType::ImportDataCamera;
			break;
		default:
			assert(0);
			break;
		}
		return ImportData::LImportNodeDataType::ImportDataMesh;
	}

}
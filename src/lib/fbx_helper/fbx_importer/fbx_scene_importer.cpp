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
		ParseSceneAxisAndUnit(fbxDataInput->mAxis, fbxDataInput->mUnit, outputScene);
		for (auto& eachData : fbxDataInput->mNodes)
		{
			ImportData::LImportSceneNode node_value;
			node_value.mIndex = eachData.mIndex;
			node_value.mName = eachData.mName;
			node_value.mParent = eachData.mParent;
			FbxVector3ToVector3(eachData.mLocalTranslation,node_value.mTranslation);
			FbxVector3ToVector3(eachData.mLocalScaling, node_value.mScal);
			FbxEulaToQuaternion(eachData.mLocalRotation, node_value.mRotation);
			node_value.mChild = eachData.mChild;
			for (auto &eachNodeDataRef : eachData.mNodeData)
			{
				ImportData::LImportNodeDataType newDataType;
				switch (eachNodeDataRef.first)
				{
				case FbxMeshData:
					newDataType = ImportData::LImportNodeDataType::ImportDataMesh;
					break;
				default:
					assert(0);
					break;
				};
				node_value.mNodeData.insert(std::pair<ImportData::LImportNodeDataType, size_t>(newDataType, eachNodeDataRef.second));
			}
			outputScene.AddNodeData(node_value);
		}
		for (auto &eachData : fbxDataInput->mDatas)
		{
			LFbxDataType dataType = eachData->GetType();
			size_t nodeIdex = eachData->GetNodeIndex();
			ParseSceneData(GetTypeByFbxType(dataType), eachData.get(), fbxDataInput->mNodes[nodeIdex], outputScene);
		}
	}

	void LFbxSceneImportHelper::ParseSceneAxisAndUnit(
		fbxsdk::FbxAxisSystem axis,
		fbxsdk::FbxSystemUnit unit,
		ImportData::LImportScene& outputScene
	)
	{
		ImportData::LImportAxisType axisType;
		ImportData::LImportUnitType unitType;
		if (axis == fbxsdk::FbxAxisSystem::eMayaZUp)
		{
			axisType = ImportData::LImportAxisType::ImportAxisZupRightHand;
		}
		else if (axis == fbxsdk::FbxAxisSystem::eMayaYUp)
		{
			axisType = ImportData::LImportAxisType::ImportAxisYupRightHand;
		}
		else
		{
			assert(false);
		}
		if (unit == fbxsdk::FbxSystemUnit::cm)
		{
			unitType = ImportData::LImportUnitType::ImportUnitCenterMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::dm)
		{
			unitType = ImportData::LImportUnitType::ImportUnitDeciMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::m)
		{
			unitType = ImportData::LImportUnitType::ImportUnitMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::Inch)
		{
			unitType = ImportData::LImportUnitType::ImportUnitInch;
		}
		else
		{
			assert(false);
		}
		outputScene.SetAxisAndUnit(axisType, unitType);
	}

	void LFbxSceneImportHelper::ParseSceneData(
		ImportData::LImportNodeDataType type,
		const LFbxDataBase* fbxDataInput,
		const LFbxNodeBase &fbxNodeInput,
		ImportData::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(fbxDataInput, fbxNodeInput, outputScene);
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
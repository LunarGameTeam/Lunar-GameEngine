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
			resimport::LImportNodeDataType, 
			std::shared_ptr<LFbxImporterBase>
		>
			(
				resimport::LImportNodeDataType::ImportDataMesh,
				std::make_shared<LFbxImporterMesh>()
			)
		);
	}

	void LFbxSceneImportHelper::ParseScene(const LFbxSceneData* fbxDataInput, resimport::LImportScene& outputScene)
	{
		ParseSceneAxisAndUnit(fbxDataInput->mAxis, fbxDataInput->mUnit, outputScene);
		for (auto& eachData : fbxDataInput->mNodes)
		{
			resimport::LImportSceneNode node_value;
			node_value.mIndex = eachData.mIndex;
			node_value.mName = eachData.mName;
			node_value.mParent = eachData.mParent;
			FbxVector3ToVector3(eachData.mLocalTranslation,node_value.mTranslation);
			FbxVector3ToVector3(eachData.mLocalScaling, node_value.mScal);
			FbxEulaToQuaternion(eachData.mLocalRotation, node_value.mRotation);
			node_value.mChild = eachData.mChild;
			for (auto &eachNodeDataRef : eachData.mNodeData)
			{
				resimport::LImportNodeDataType newDataType;
				switch (eachNodeDataRef.first)
				{
				case FbxMeshData:
					newDataType = resimport::LImportNodeDataType::ImportDataMesh;
					break;
				default:
					assert(0);
					break;
				};
				node_value.mNodeData.insert(std::pair<resimport::LImportNodeDataType, size_t>(newDataType, eachNodeDataRef.second));
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
		resimport::LImportScene& outputScene
	)
	{
		resimport::LImportAxisType axisType;
		resimport::LImportUnitType unitType;
		if (axis == fbxsdk::FbxAxisSystem::eMayaZUp)
		{
			axisType = resimport::LImportAxisType::ImportAxisZupRightHand;
		}
		else if (axis == fbxsdk::FbxAxisSystem::eMayaYUp)
		{
			axisType = resimport::LImportAxisType::ImportAxisYupRightHand;
		}
		else
		{
			assert(false);
		}
		if (unit == fbxsdk::FbxSystemUnit::cm)
		{
			unitType = resimport::LImportUnitType::ImportUnitCenterMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::dm)
		{
			unitType = resimport::LImportUnitType::ImportUnitDeciMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::m)
		{
			unitType = resimport::LImportUnitType::ImportUnitMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::Inch)
		{
			unitType = resimport::LImportUnitType::ImportUnitInch;
		}
		else
		{
			assert(false);
		}
		outputScene.SetAxisAndUnit(axisType, unitType);
	}

	void LFbxSceneImportHelper::ParseSceneData(
		resimport::LImportNodeDataType type,
		const LFbxDataBase* fbxDataInput,
		const LFbxNodeBase &fbxNodeInput,
		resimport::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(fbxDataInput, fbxNodeInput, outputScene);
	}

	resimport::LImportNodeDataType LFbxSceneImportHelper::GetTypeByFbxType(LFbxDataType inType)
	{
		switch (inType)
		{
		case luna::lfbx::FbxMeshData:
			return resimport::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::lfbx::FbxMaterialData:
			return resimport::LImportNodeDataType::ImportDataMaterial;
			break;
		case luna::lfbx::FbxCameraData:
			return resimport::LImportNodeDataType::ImportDataCamera;
			break;
		default:
			assert(0);
			break;
		}
		return resimport::LImportNodeDataType::ImportDataMesh;
	}

}
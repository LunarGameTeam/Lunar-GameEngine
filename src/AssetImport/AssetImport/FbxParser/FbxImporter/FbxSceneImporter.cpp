#include "FbxSceneImporter.h"
#include "FbxMeshImporter.h"
#include "FbxSkeletonImporter.h"
#include "FbxAnimationImporter.h"
#include "FbxMaterialImporter.h"
#include "FbxCameraImporter.h"

namespace luna::lfbx
{
	LFbxSceneImportHelper::LFbxSceneImportHelper()
	{
		mImporters.insert({asset::LImportNodeDataType::ImportDataMesh,std::make_shared<LFbxImporterMesh>()});
	}

	void LFbxSceneImportHelper::ParseScene(const LFbxSceneData* fbxDataInput, asset::LImportScene& outputScene)
	{
		ParseSceneAxisAndUnit(fbxDataInput->mAxis, fbxDataInput->mUnit, outputScene);
		for (auto& eachData : fbxDataInput->mNodes)
		{
			asset::LImportSceneNode node_value;
			node_value.mIndex = eachData.mIndex;
			node_value.mName = eachData.mName;
			node_value.mParent = eachData.mParent;
			FbxVector3ToVector3(eachData.mLocalTranslation,node_value.mTranslation);
			FbxVector3ToVector3(eachData.mLocalScaling, node_value.mScal);
			FbxEulaToQuaternion(eachData.mLocalRotation, node_value.mRotation);
			node_value.mChild = eachData.mChild;
			for (auto &eachNodeDataRef : eachData.mNodeData)
			{
				asset::LImportNodeDataType newDataType;
				switch (eachNodeDataRef.first)
				{
				case FbxMeshData:
					newDataType = asset::LImportNodeDataType::ImportDataMesh;
					break;
				case FbxSkeletonData:
					newDataType = asset::LImportNodeDataType::ImportDataSkeleton;
					break;
				default:
					assert(0);
					break;
				};
				node_value.mNodeData.insert(std::pair<asset::LImportNodeDataType, size_t>(newDataType, eachNodeDataRef.second));
			}
			outputScene.AddNodeData(node_value);
		}
		LFbxImportContext dataContext;
		for (auto &eachData : fbxDataInput->mDatas)
		{
			LFbxDataType dataType = eachData->GetType();
			size_t nodeIdex = eachData->GetNodeIndex();
			ParseSceneData(GetTypeByFbxType(dataType), nodeIdex,eachData.get(), fbxDataInput->mNodes[nodeIdex], dataContext, outputScene);
		}
	}

	void LFbxSceneImportHelper::ParseSceneAxisAndUnit(
		fbxsdk::FbxAxisSystem axis,
		fbxsdk::FbxSystemUnit unit,
		asset::LImportScene& outputScene
	)
	{
		asset::LImportAxisType axisType;
		asset::LImportUnitType unitType;
		if (axis == fbxsdk::FbxAxisSystem::eMayaZUp)
		{
			axisType = asset::LImportAxisType::ImportAxisZupRightHand;
		}
		else if (axis == fbxsdk::FbxAxisSystem::eMayaYUp)
		{
			axisType = asset::LImportAxisType::ImportAxisYupRightHand;
		}
		else
		{
			assert(false);
		}
		if (unit == fbxsdk::FbxSystemUnit::cm)
		{
			unitType = asset::LImportUnitType::ImportUnitCenterMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::dm)
		{
			unitType = asset::LImportUnitType::ImportUnitDeciMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::m)
		{
			unitType = asset::LImportUnitType::ImportUnitMeter;
		}
		else if (unit == fbxsdk::FbxSystemUnit::Inch)
		{
			unitType = asset::LImportUnitType::ImportUnitInch;
		}
		else
		{
			assert(false);
		}
		outputScene.SetAxisAndUnit(axisType, unitType);
	}

	void LFbxSceneImportHelper::ParseSceneData(
		const asset::LImportNodeDataType type,
		const size_t nodeIdex,
		const LFbxDataBase* fbxDataInput,
		const LFbxNodeBase &fbxNodeInput,
		LFbxImportContext& dataContext,
		asset::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(fbxDataInput, fbxNodeInput, dataContext,outputScene);
	}

	asset::LImportNodeDataType LFbxSceneImportHelper::GetTypeByFbxType(LFbxDataType inType)
	{
		switch (inType)
		{
		case luna::lfbx::FbxMeshData:
			return asset::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::lfbx::FbxMaterialData:
			return asset::LImportNodeDataType::ImportDataMaterial;
			break;
		case luna::lfbx::FbxCameraData:
			return asset::LImportNodeDataType::ImportDataCamera;
			break;
		default:
			assert(0);
			break;
		}
		return asset::LImportNodeDataType::ImportDataMesh;
	}

}
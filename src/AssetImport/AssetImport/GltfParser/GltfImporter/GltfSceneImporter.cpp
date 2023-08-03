#include "AssetImport/GltfParser/GltfImporter/GltfSceneImporter.h"
#include "AssetImport/GltfParser/GltfImporter/GltfMeshImporter.h"
#include "AssetImport/GltfParser/GltfImporter/GltfSkeletonImporter.h"
#include "AssetImport/GltfParser/GltfImporter/GltfAnimationImporter.h"
#include "AssetImport/GltfParser/GltfImporter/GltfMaterialImporter.h"
#include "AssetImport/GltfParser/GltfImporter/GltfCameraImporter.h"

namespace luna::asset
{

LGltfSceneImportHelper::LGltfSceneImportHelper()
{
	mImporters[LImportNodeDataType::ImportDataMesh] = std::make_shared<LGltfImporterMesh>();
}

void LGltfSceneImportHelper::ParseScene(const LGltfSceneData* gltfDataInput, asset::LImportScene& outputScene)
{
	outputScene.SetAxisAndUnit(asset::LImportAxisType::ImportAxisYupLeftHand, asset::LImportUnitType::ImportUnitMeter);
	for (auto& eachData : gltfDataInput->mNodes)
	{
		asset::LImportSceneNode node_value;
		node_value.mIndex = eachData.mIndex;
		node_value.mName = eachData.mName;
		node_value.mParent = eachData.mParent;
		node_value.mTranslation = GetVector3FromGltfVector3(eachData.translation);
		node_value.mScal = GetVector3FromGltfVector3(eachData.scale);
		node_value.mRotation = GetQuaternionFromGltfQuaternion(eachData.rotation);
		node_value.mExtrMatrix = GetMatrix4FromGltfMatrix4(eachData.matrix);
		node_value.mChild = eachData.mChild;
		for (auto& eachNodeDataRef : eachData.mNodeData)
		{
			asset::LImportNodeDataType newDataType;
			switch (eachNodeDataRef.first)
			{
			case GltfMeshData:
				newDataType = asset::LImportNodeDataType::ImportDataMesh;
				break;
			default:
				assert(0);
				break;
			};
			node_value.mNodeData.insert(std::pair<asset::LImportNodeDataType, size_t>(newDataType, eachNodeDataRef.second));
		}
		outputScene.AddNodeData(node_value);
	}
	LGltfImportContext dataContext;
	for (auto& eachData : gltfDataInput->mDatas)
	{
		LGltfDataType dataType = eachData->GetType();
		const LArray<size_t>& nodeIdeses = eachData->GetNodeIndses();
		ParseSceneData(GetTypeByGltfType(dataType), eachData.get(), nodeIdeses, dataContext, outputScene);
	}
}

void LGltfSceneImportHelper::ParseSceneData(
	asset::LImportNodeDataType type,
	const LGltfDataBase* gltfDataInput,
	const LArray<size_t>& gltfNodeInput,
	LGltfImportContext& dataContext,
	asset::LImportScene& outputScene
)
{
	auto needImporter = mImporters.find(type);
	if (needImporter == mImporters.end())
	{
		return;
	}
	return needImporter->second->ParsingData(gltfDataInput, gltfNodeInput, dataContext, outputScene);
}

asset::LImportNodeDataType LGltfSceneImportHelper::GetTypeByGltfType(LGltfDataType inType)
{
	switch (inType)
	{
	case luna::asset::GltfMeshData:
		return asset::LImportNodeDataType::ImportDataMesh;
		break;
	case luna::asset::GltfMaterialData:
		return asset::LImportNodeDataType::ImportDataMaterial;
		break;
	case luna::asset::GltfCameraData:
		return asset::LImportNodeDataType::ImportDataCamera;
		break;
	default:
		assert(0);
		break;
	}
	return asset::LImportNodeDataType::ImportDataMesh;
}

}
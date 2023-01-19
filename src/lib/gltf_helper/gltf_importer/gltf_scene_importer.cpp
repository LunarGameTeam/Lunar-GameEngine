#include "gltf_scene_importer.h"
#include "gltf_mesh_importer.h"
#include "gltf_skeleton_importer.h"
#include "gltf_animation_importer.h"
#include "gltf_material_importer.h"
#include "gltf_camera_importer.h"
namespace luna::lgltf
{
	LGltfSceneImportHelper::LGltfSceneImportHelper()
	{
		mImporters.insert(std::pair<
			resimport::LImportNodeDataType, 
			std::shared_ptr<LGltfImporterBase>
		>
			(
				resimport::LImportNodeDataType::ImportDataMesh,
				std::make_shared<LGltfImporterMesh>()
			)
		);
	}

	void LGltfSceneImportHelper::ParseScene(const LGltfSceneData* gltfDataInput, resimport::LImportScene& outputScene)
	{
		for (auto& eachData : gltfDataInput->mNodes)
		{
			resimport::LImportSceneNode node_value;
			node_value.mIndex = eachData.mIndex;
			node_value.mName = eachData.mName;
			node_value.mParent = eachData.mParent;
			//node_value.mTranslation = eachData.translation;
			//node_value.mScal = eachData.rotation;
			//node_value.mRotation = eachData.scale;
			node_value.mChild = eachData.mChild;
			for (auto &eachNodeDataRef : eachData.mNodeData)
			{
				resimport::LImportNodeDataType newDataType;
				switch (eachNodeDataRef.first)
				{
				case GltfMeshData:
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
		for (auto &eachData : gltfDataInput->mDatas)
		{
			LGltfDataType dataType = eachData->GetType();
			size_t nodeIdex = eachData->GetNodeIndex();
			ParseSceneData(GetTypeByGltfType(dataType), eachData.get(), gltfDataInput->mNodes[nodeIdex], outputScene);
		}
	}

	void LGltfSceneImportHelper::ParseSceneData(
		resimport::LImportNodeDataType type,
		const LGltfDataBase* gltfDataInput,
		const LGltfNodeBase &gltfNodeInput,
		resimport::LImportScene& outputScene
	)
	{
		auto needImporter = mImporters.find(type);
		if (needImporter == mImporters.end())
		{
			return;
		}
		return needImporter->second->ParsingData(gltfDataInput, gltfNodeInput, outputScene);
	}

	resimport::LImportNodeDataType LGltfSceneImportHelper::GetTypeByGltfType(LGltfDataType inType)
	{
		switch (inType)
		{
		case luna::lgltf::GltfMeshData:
			return resimport::LImportNodeDataType::ImportDataMesh;
			break;
		case luna::lgltf::GltfMaterialData:
			return resimport::LImportNodeDataType::ImportDataMaterial;
			break;
		case luna::lgltf::GltfCameraData:
			return resimport::LImportNodeDataType::ImportDataCamera;
			break;
		default:
			assert(0);
			break;
		}
		return resimport::LImportNodeDataType::ImportDataMesh;
	}

}
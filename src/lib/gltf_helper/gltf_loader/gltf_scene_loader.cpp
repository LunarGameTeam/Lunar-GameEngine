#pragma once
#include "gltf_scene_loader.h"
#include "gltf_mesh_loader.h"
#include "gltf_skeleton_loader.h"
#include "gltf_animation_loader.h"
#include "gltf_material_loader.h"
#include "gltf_camera_loader.h"
namespace luna::lgltf
{
	LGltfLoaderFactory::LGltfLoaderFactory()
	{
		mLoaders.insert(std::pair<LGltfDataType, std::shared_ptr<LGltfLoaderBase>>(LGltfDataType::GltfMeshData, std::make_shared<LGltfLoaderMesh>()));
	}

	std::shared_ptr<LGltfDataBase> LGltfLoaderFactory::LoadGltfData(LGltfDataType type, const LVector<LGltfNodeBase>& sceneNodes, Microsoft::glTF::Node* pNode)
	{
		auto needLoader = mLoaders.find(type);
		if (needLoader == mLoaders.end())
		{
			return nullptr;
		}
		return needLoader->second->ParsingData(sceneNodes, pNode);
	}

	void LGltfLoaderHelper::LoadGltfFile(const char* pFilename, LGltfSceneData& sceneOut)
	{
	}
}
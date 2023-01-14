#pragma once
#include "gltf_mesh_loader.h"
namespace luna::lgltf
{
	std::shared_ptr<LGltfDataBase> LGltfLoaderMesh::ParsingDataImpl(const LVector<LGltfNodeBase>& sceneNodes, Microsoft::glTF::Node* pNode)
	{
		return nullptr;
	};
}
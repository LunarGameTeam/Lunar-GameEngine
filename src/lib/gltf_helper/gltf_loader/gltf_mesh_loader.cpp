#pragma once
#include "gltf_mesh_loader.h"
namespace luna::lgltf
{
	std::shared_ptr<LGltfDataBase> LGltfLoaderMesh::ParsingDataImpl(const Microsoft::glTF::Document& doc, const Microsoft::glTF::Node* pNode)
	{
		std::shared_ptr<LGltfDataMesh> newMesh = std::make_shared<LGltfDataMesh>();

		return nullptr;
	};
}
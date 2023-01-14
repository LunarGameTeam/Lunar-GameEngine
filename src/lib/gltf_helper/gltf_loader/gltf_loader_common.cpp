#pragma once
#include "gltf_loader_common.h"
namespace luna::lgltf
{
	std::shared_ptr<LGltfDataBase> LGltfLoaderBase::ParsingData(const LVector<LGltfNodeBase>& sceneNodes, Microsoft::glTF::Node* pNode)
	{
		return ParsingDataImpl(sceneNodes, pNode);
	}
}
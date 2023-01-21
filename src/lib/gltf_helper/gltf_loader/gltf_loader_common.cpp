#pragma once
#include "gltf_loader_common.h"
namespace luna::lgltf
{
	std::shared_ptr<LGltfDataBase> LGltfLoaderBase::ParsingData(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId)
	{
		return ParsingDataImpl(doc, reader,gltfDataId);
	}
}
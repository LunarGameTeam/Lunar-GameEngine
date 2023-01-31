#pragma once
#include "GltfLoaderCommon.h"
namespace luna::asset
{
	std::shared_ptr<LGltfDataBase> LGltfLoaderBase::ParsingData(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId)
	{
		return ParsingDataImpl(doc, reader,gltfDataId);
	}
}
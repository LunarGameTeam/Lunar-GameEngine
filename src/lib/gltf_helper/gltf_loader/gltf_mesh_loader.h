#pragma once
#include"gltf_loader_common.h"
namespace luna::lgltf
{
	class LGltfDataMesh : public LGltfDataBase
	{
	public:
		LGltfDataMesh() : LGltfDataBase(LGltfDataType::GltfMeshData) {};
	};

	class LGltfLoaderMesh : public LGltfLoaderBase
	{
	public:
		LGltfLoaderMesh(){};
	private:
		std::shared_ptr<LGltfDataBase> ParsingDataImpl(const Microsoft::glTF::Document& doc, const Microsoft::glTF::Node* pNode) override;
	};
}
#pragma once
#include"GltfLoader/GltfLoaderCommon.h"
namespace luna::lgltf
{

	struct LGltfSceneData
	{
		LArray<LGltfNodeBase> mNodes;
		LArray<std::shared_ptr<LGltfDataBase>> mDatas;
	};

	class LGltfLoaderFactory
	{
		LUnorderedMap<LGltfDataType, std::shared_ptr<LGltfLoaderBase>> mLoaders;
	public:
		LGltfLoaderFactory();
		std::shared_ptr<LGltfDataBase> LoadGltfData(const Microsoft::glTF::Document &doc, const Microsoft::glTF::GLTFResourceReader &reader,LGltfDataType type, size_t gltfDataId);
	};
	static std::shared_ptr<LGltfLoaderFactory> singleLoaderInterface = std::make_shared<LGltfLoaderFactory>();

	class LGltfLoaderHelper
	{
	public:
		void LoadGltfFile(const char* pFilename, LGltfSceneData& scene_out);
	};
}
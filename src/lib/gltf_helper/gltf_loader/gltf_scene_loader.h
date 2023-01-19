#pragma once
#include"gltf_loader_common.h"
namespace luna::lgltf
{

	struct LGltfSceneData
	{
		LVector<LGltfNodeBase> mNodes;
		LVector<std::shared_ptr<LGltfDataBase>> mDatas;
	};

	class LGltfLoaderFactory
	{
		LUnorderedMap<LGltfDataType, std::shared_ptr<LGltfLoaderBase>> mLoaders;
	public:
		LGltfLoaderFactory();
		std::shared_ptr<LGltfDataBase> LoadGltfData(const Microsoft::glTF::Document &doc, LGltfDataType type, const Microsoft::glTF::Node* pNode);
	};
	static std::shared_ptr<LGltfLoaderFactory> singleLoaderInterface = std::make_shared<LGltfLoaderFactory>();

	class LGltfLoaderHelper
	{
	public:
		void LoadGltfFile(const char* pFilename, LGltfSceneData& scene_out);
	};
}
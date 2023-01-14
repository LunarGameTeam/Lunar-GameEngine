#pragma once
#include "core/core_library.h"
#include<GLTFSDK/GLTF.h>
namespace luna::lgltf
{
	enum LGltfDataType
	{
		GltfMeshData = 0,
		GltfSkeletonData,
		GltfAnimationData,
		GltfMaterialData,
		GltfCameraData,
	};

	struct LGltfNodeBase
	{
		size_t mParent;
		LVector<size_t> mChild;
		LVector3f mLocalTranslation;
		LQuaternion mLocalRotation;
		LVector3f mLocalScaling;
		std::unordered_map<LGltfDataType, size_t> mNodeData;
		size_t mIndex;
		LString mName;
	};

	class LGltfDataBase
	{
		LGltfDataType mType;
		size_t mNodeIndex;
	public:
		LGltfDataBase(LGltfDataType type) { mType = type; };
		void SetNodeIndex(size_t nodeIndex) { mNodeIndex = nodeIndex; }
		size_t GetNodeIndex() const{ return mNodeIndex; };
		LGltfDataType GetType() const { return mType; };
	};

	class LGltfLoaderBase
	{
	public:
		LGltfLoaderBase() {};
		std::shared_ptr<LGltfDataBase> ParsingData(const LVector<LGltfNodeBase>& sceneNodes, Microsoft::glTF::Node* pNode);
	private:
		virtual std::shared_ptr<LGltfDataBase> ParsingDataImpl(const LVector<LGltfNodeBase>& sceneNodes, Microsoft::glTF::Node* pNode) = 0;
	};
}
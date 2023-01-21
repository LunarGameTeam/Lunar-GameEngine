#pragma once
#include "core/core_library.h"
#include<GLTFSDK/GLTF.h>
#include <GLTFSDK/Deserialize.h>
#include <GLTFSDK/GLBResourceReader.h>
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
		Microsoft::glTF::Matrix4 matrix;
		Microsoft::glTF::Quaternion rotation = Microsoft::glTF::Quaternion::IDENTITY;
		Microsoft::glTF::Vector3 scale = Microsoft::glTF::Vector3::ONE;
		Microsoft::glTF::Vector3 translation = Microsoft::glTF::Vector3::ZERO;
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
		std::shared_ptr<LGltfDataBase> ParsingData(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId);
	private:
		virtual std::shared_ptr<LGltfDataBase> ParsingDataImpl(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId) = 0;
	};
}
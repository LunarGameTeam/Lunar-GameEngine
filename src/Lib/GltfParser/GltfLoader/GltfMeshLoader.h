#pragma once
#include"GltfLoader/GltfLoaderCommon.h"
namespace luna::lgltf
{
	struct GltfSubmeshData
	{
		std::vector<float> mPosition;
		std::vector<float> mNormal;
		std::vector<float> mTangent;
		std::vector<float> mUv0;
		std::vector<float> mUv1;
		std::vector<uint32_t> mColor;
		std::vector<uint32_t> mFaceIndex;
	};
	class LGltfDataMesh : public LGltfDataBase
	{
		LArray<GltfSubmeshData> mSubmeshs;
	public:
		LGltfDataMesh() : LGltfDataBase(LGltfDataType::GltfMeshData) {};

		GltfSubmeshData& AddSubMesh();

		size_t GetSubMeshSize()const { return mSubmeshs.size(); };

		const GltfSubmeshData& GetSubMesh(size_t index)const { return mSubmeshs[index]; };
	};

	class LGltfLoaderMesh : public LGltfLoaderBase
	{
	public:
		LGltfLoaderMesh(){};
	private:
		std::shared_ptr<LGltfDataBase> ParsingDataImpl(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId) override;
	};
}
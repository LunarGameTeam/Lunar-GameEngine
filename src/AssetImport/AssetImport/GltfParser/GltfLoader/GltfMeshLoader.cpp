#pragma once
#include "AssetImport/GltfParser/GltfLoader/GltfMeshLoader.h"
#include "GLTFSDK/MeshPrimitiveUtils.h"

namespace luna::asset
{
	GltfSubmeshData& LGltfDataMesh::AddSubMesh()
	{
		mSubmeshs.emplace_back();
		return mSubmeshs.back();
	}

	std::shared_ptr<LGltfDataBase> LGltfLoaderMesh::ParsingDataImpl(const Microsoft::glTF::Document& doc, const Microsoft::glTF::GLTFResourceReader& reader, size_t gltfDataId)
	{
		std::shared_ptr<LGltfDataMesh> newMesh = std::make_shared<LGltfDataMesh>();
		const Microsoft::glTF::Mesh &mesh = doc.meshes[gltfDataId];
		newMesh->SetName(mesh.name);
		for(size_t primIndex = 0; primIndex < mesh.primitives.size(); ++primIndex)
		{
			const Microsoft::glTF::MeshPrimitive& primitive = mesh.primitives[primIndex];
			GltfSubmeshData& newSubMesh = newMesh->AddSubMesh();

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_POSITION))
			{
				newSubMesh.mPosition = Microsoft::glTF::MeshPrimitiveUtils::GetPositions(doc, reader, primitive);
			}

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_NORMAL))
			{
				newSubMesh.mNormal = Microsoft::glTF::MeshPrimitiveUtils::GetNormals(doc, reader, primitive);
			}

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TANGENT))
			{
				newSubMesh.mTangent = Microsoft::glTF::MeshPrimitiveUtils::GetTangents(doc, reader, primitive);
			}

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_COLOR_0))
			{
				newSubMesh.mColor = Microsoft::glTF::MeshPrimitiveUtils::GetColors_0(doc, reader, primitive);
			}

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TEXCOORD_0))
			{
				newSubMesh.mUv0 = Microsoft::glTF::MeshPrimitiveUtils::GetTexCoords_0(doc, reader, primitive);;
			}

			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TEXCOORD_1))
			{
				newSubMesh.mUv1 = Microsoft::glTF::MeshPrimitiveUtils::GetTexCoords_1(doc, reader, primitive);;
			}

			try 
			{
				newSubMesh.mFaceIndex = Microsoft::glTF::MeshPrimitiveUtils::GetTriangulatedIndices32(doc, reader, primitive);
			}
			catch (Microsoft::glTF::GLTFException ex)
			{
				std::vector<uint16_t> lowBitFaceIndex  = Microsoft::glTF::MeshPrimitiveUtils::GetTriangulatedIndices16(doc, reader, primitive);
				for (uint16_t& lowBitValue : lowBitFaceIndex)
				{
					newSubMesh.mFaceIndex.push_back((uint32_t)lowBitValue);
				}
			}
		}
		return newMesh;
	};
}
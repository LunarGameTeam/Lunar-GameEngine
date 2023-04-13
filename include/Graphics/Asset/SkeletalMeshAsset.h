#pragma once
#include "Graphics/Asset/MeshAsset.h"

namespace luna::render
{
	class RHIResource;

	struct SkinVertex
	{
		uint32_t mRefBone[4];
		float mWeight[4];
		static RHIVertexLayout GetVertexLayout()
		{
			RHIVertexLayout layout;
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsagePosition, 3, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageColor, 4, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageNormal, 3, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTangent, 4, 0, VertexElementInstanceType::PerVertex);

			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture0, 2, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture1, 2, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture2, 2, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Float, VertexElementUsage::UsageTexture3, 2, 0, VertexElementInstanceType::PerVertex);

			layout.AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageBlendIndex, 4, 0, VertexElementInstanceType::PerVertex);
			layout.AddVertexElement(VertexElementType::Int, VertexElementUsage::UsageBlendWeight, 4, 0, VertexElementInstanceType::PerVertex);

			return layout;
		}
	};

	class RENDER_API SubMeshSkeletal : public SubMesh
	{
		RegisterTypeEmbedd(SubMeshSkeletal, InvalidType)
	public:
		SubMeshSkeletal();
		~SubMeshSkeletal() { }
		LArray<SkinVertex> mSkinData;
	};

	class RENDER_API SkeletalMeshAsset : public MeshAsset
	{
		RegisterTypeEmbedd(SkeletalMeshAsset, MeshAsset)
	public:
		SkeletalMeshAsset()
		{
		}
	private:
		render::SubMesh* GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize) override;
		void ReadVertexData(size_t idx, const byte* ptr) override;
		void WriteVertexData(size_t idx, byte* dst) override;

	};

}
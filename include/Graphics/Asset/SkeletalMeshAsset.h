#pragma once
#include "Graphics/Asset/MeshAsset.h"

namespace luna::graphics
{
	class RHIResource;

	struct SkinVertex
	{
		uint32_t mRefBone[4];
		uint32_t mWeight[4];
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

	struct SkinRenderVertex
	{
		BaseVertex mBaseValue;
		SkinVertex mSkinValue;
	};

	class RENDER_API SubMeshSkeletal : public SubMesh
	{
		RegisterTypeEmbedd(SubMeshSkeletal, InvalidType)
	public:
		SubMeshSkeletal();
		~SubMeshSkeletal() { }
		LArray<SkinVertex> mSkinData;
		LArray<LString>    mRefBoneName;
		LArray<LMatrix4f>  mRefBonePose;
	};

	class RENDER_API SkeletalMeshAsset : public MeshAsset
	{
		RegisterTypeEmbedd(SkeletalMeshAsset, MeshAsset)
	public:
		SkeletalMeshAsset()
		{
		}
	private:
		graphics::SubMesh* GenerateSubmesh(size_t submeshVertexSize, size_t submeshIndexSize) override;
		void ReadVertexData(size_t idx, const byte* &ptr) override;
		void WriteVertexData(size_t idx, LArray<byte>& data) override;

	};

}
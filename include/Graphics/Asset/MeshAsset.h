#pragma once
#include "Core/Math/Math.h"
#include "Core/Foundation/Container.h"
#include "Core/Scripting/Binding.h"
#include "Core/Object/BaseObject.h"
#include "Core/Asset/BinaryAsset.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIResource.h"
#include "Core/Scripting/BindingProxy.h"

namespace luna::render
{
	class RHIResource;


	struct BaseVertex
	{
		LVector3f pos;
		LVector4f color;
		LVector3f normal;
		LVector4f tangent;
		LVector2f uv[4];
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
			return layout;
		}
	};


	class RENDER_API SubMesh
	{
		RegisterTypeEmbedd(SubMesh, InvalidType)
	public:
		SubMesh();
		~SubMesh() {}

		void ClearVertexData();

		void SetVertexFormat(const RHIVertexLayout& format);
		RHIVertexLayout& GetVertexLayout() { return mVeretexLayout; };

		int GetVertexCount() const { return (int)mVertexData.size(); }
		int GetIndexCount() const { return(int)mIndexData.size(); }

		LArray<BaseVertex> mVertexData;
		LArray<uint32_t> mIndexData;

		RHIVertexLayout mVeretexLayout;
		LString mAssetPath;
		size_t mSubmeshIndex;
		size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
		size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };

	};

	class RENDER_API MeshAsset : public Asset
	{
		RegisterTypeEmbedd(MeshAsset, Asset)
	public:
		MeshAsset()
		{
		}
		size_t SubMeshSize()
		{
			return mSubMesh.size();
		}

		SubMesh* GetSubMeshAt(size_t index)
		{
			return mSubMesh[index];
		};

		LArray<SubMesh*> mSubMesh;

		//Asset资源读入到内存时回调
		virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
		//Asset资源写入到磁盘时回调	
		virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;
	private:
		virtual render::SubMesh* GenerateSubmesh(size_t submeshVertexSize,size_t submeshIndexSize);
		virtual void ReadVertexData(size_t idx,const byte* ptr);
		virtual void WriteVertexData(size_t idx, byte* dst);
	};

}

namespace luna::binding
{
template<> struct binding_proxy<render::SubMesh> : native_binding_proxy<render::SubMesh> {};
}
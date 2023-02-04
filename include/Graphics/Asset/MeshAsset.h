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
	~SubMesh() { Release(); }

	void ClearVertexData();
	
	void AddLine(const BaseVertex& v1, const BaseVertex& v2);
	void AddLine(const LVector3f& v1, const LVector3f& v2);
	void AddCubeWired(const LVector3f& position, const LVector3f& size = LVector3f(1, 1, 1), const LVector4f& color = LVector4f(1, 1, 1));
	void AddTriangle(const BaseVertex& v1, const BaseVertex& v2, const BaseVertex& v3);

	void Release();
	void Update();
	
	void SetVertexFormat(const RHIVertexLayout& format);
	RHIVertexLayout& GetVertexLayout() { return mVeretexLayout;	};

	int GetVertexCount() const { return (int)mVertexData.size(); }
	int GetIndexCount() const { return(int)mIndexData.size(); }

	LArray<BaseVertex> mVertexData;
	LArray<uint32_t> mIndexData;

	RHIResourcePtr mVB;
	RHIResourcePtr mIB;
	RHIVertexLayout mVeretexLayout;

	size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
	size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
private:
	bool mReady = false;
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
	bool mReady = false;
};
}

namespace luna::binding
{
template<> struct binding_proxy<render::SubMesh> : native_binding_proxy<render::SubMesh> {};
}
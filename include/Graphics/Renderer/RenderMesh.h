#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"
#include "Graphics/RHI/RHIResource.h"
#include <functional>



namespace luna::render
{
	//这里标记了场景渲染器用到的所有的mesh相关的renderpass
	enum MeshRenderPass
	{
		DirectLightShadowDepthPass = 0,
		PointLightShadowDepthPass,
		LightingPass,
		AllNum,
	};

	class RenderMeshBase
	{
		RHIResourcePtr mVB;
		RHIResourcePtr mIB;
		RHIVertexLayout mVeretexLayout;
	public:
		size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
		size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
		RHIVertexLayout& GetVertexLayout() { return mVeretexLayout; };
		void Init(SubMesh* meshData);
	};

	class ShaderParamSceneBuffer
	{
		//存储全部材质参数信息的buffer
		RHIResourcePtr shaderParamBuffer;
		//存储全部材质贴图信息的view
		LArray<RHIViewPtr> shaderParamTexture;
		//每个材质参数的大小
		size_t perElementSize;

	};

	class MeshRenderCommand
	{
	public:
		size_t mRenderObjectId;
		MaterialTemplateAsset* mMaterialTemplate;
		size_t mMaterialInstanceIndex;
		size_t mCanBatchHash;
	};

	class MeshRenderCommandsPacket
	{
	public:
		size_t AddCommand(
			RenderObject* renderObject,
			const LString& materialAsset
		);
		size_t AddCommand(
			RenderObject* renderObject,
			MaterialTemplateAsset* materialAsset
		);

		const LUnorderedMap<uint64_t, MeshRenderCommand>& GetCommands()const { return mAllCommands; }
	private:
		LQueue<size_t> mEmptyId;
		LUnorderedMap<uint64_t, MeshRenderCommand> mAllCommands;
		LUnorderedMap<uint64_t, uint64_t> mRoCommandIndex;
	};

	class MeshRenderCommandsPassData
	{
		LArray<const MeshRenderCommand*> allVisibleCommandsRef;
	public:
		void DrawAllCommands(PackedParams* sceneAndViewParam);
		void AddCommand(const MeshRenderCommand* data);
		void ClearCommand();
	};



}

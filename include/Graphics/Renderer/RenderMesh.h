#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RHI/RHICmdList.h"
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
		size_t mVertexSize = 0;
		RHIResourcePtr mVB;
		size_t mIndexSize = 0;
		RHIResourcePtr mIB;
		RHIVertexLayout mVeretexLayout;
	public:
		size_t GetStridePerVertex() { return mVeretexLayout.GetSize()[0]; };
		size_t GetStridePerInstance() { return mVeretexLayout.GetSize()[1]; };
		size_t GetVertexSize() { return mVertexSize; };
		size_t GetIndexSize() { return mIndexSize; };

		RHIResource* GetVertexBuffer() { return mVB.get(); };
		RHIResource* GetIndexBuffer() { return mIB.get(); };
		RHIVertexLayout& GetVertexLayout() { return mVeretexLayout; };
		void Init(SubMesh* meshData);
	};

	struct MeshSkeletonLinkClusterBase
	{
		LUnorderedMap<int32_t, int32_t> mSkinBoneIndex2SkeletonBoneIndex;
		LArray<LMatrix4f> mBindposeMatrix;
	};

	struct AnimationInstanceMatrix
	{
		LArray<LMatrix4f> mBoneMatrix;
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
		MaterialInstance* mMaterialInstance;
		size_t mCanBatchHash;
	};

	class MeshRenderCommandsPacket
	{
	public:
		size_t AddCommand(
			RenderObject* renderObject,
			MaterialInstance* materialInstance
		);

		const LUnorderedMap<uint64_t, MeshRenderCommand>& GetCommands()const { return mAllCommands; }
	private:
		LQueue<size_t> mEmptyId;
		LUnorderedMap<uint64_t, MeshRenderCommand> mAllCommands;
		LUnorderedMap<uint64_t, uint64_t> mRoCommandIndex;
	};

	class MeshRenderCommandsPassData
	{
		RenderScene* mScene = nullptr;
		RenderView*  mView = nullptr;
		LArray<const MeshRenderCommand*> allVisibleCommandsRef;
	public:
		void DrawAllCommands(const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*> &shaderBindingParam);
		//暂时添加一个可以传入指定scene view的接口用于阴影这种渲染和裁剪的view不同的情况，后面看看有没有办法去掉
		void DrawAllCommands(RHIView* sceneViewParamBuffer, const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*>& shaderBindingParam);
		void AddCommand(const MeshRenderCommand* data);
		void ClearCommand();
		void SetScene(RenderScene* scene) { mScene = scene; }
		void SetView(RenderView* view) { mView = view; }
	};



}

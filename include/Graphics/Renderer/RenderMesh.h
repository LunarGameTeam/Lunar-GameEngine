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
	//�������˳�����Ⱦ���õ������е�mesh��ص�renderpass
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
		//�洢ȫ�����ʲ�����Ϣ��buffer
		RHIResourcePtr shaderParamBuffer;
		//�洢ȫ��������ͼ��Ϣ��view
		LArray<RHIViewPtr> shaderParamTexture;
		//ÿ�����ʲ����Ĵ�С
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
		//��ʱ���һ�����Դ���ָ��scene view�Ľӿ�������Ӱ������Ⱦ�Ͳü���view��ͬ����������濴����û�а취ȥ��
		void DrawAllCommands(RHIView* sceneViewParamBuffer, const std::unordered_map<luna::render::ShaderParamID, luna::render::RHIView*>& shaderBindingParam);
		void AddCommand(const MeshRenderCommand* data);
		void ClearCommand();
		void SetScene(RenderScene* scene) { mScene = scene; }
		void SetView(RenderView* view) { mView = view; }
	};



}

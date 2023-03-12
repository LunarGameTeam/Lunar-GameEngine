#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"

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
		RHIResourcePtr mVB;
		RHIResourcePtr mIB;
		RHIVertexLayout mVeretexLayout;
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

	class RenderSceneData
	{
		LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;
		LQueue<size_t> emptyMeshId;
		LUnorderedMap<size_t, RenderMeshBase> meshPrimitiveBuffer;

	};

	struct RENDER_API RenderObject
	{
		size_t            mMeshIndex;
		LMatrix4f* mWorldMat;
		bool              mCastShadow = true;
		bool              mReceiveLight = true;
		bool              mReceiveShadow = true;
		uint64_t          mID;
	};

	class MeshRenderCommand
	{
	public:
		RHIPipelineStatePtr mPipeline;
		size_t mRenderObjectId;
		TPPtr<MaterialTemplateAsset> mMaterialTemplate;
		size_t mMaterialInstanceIndex;
	};

	class MeshRenderCommandsPacket
	{
	public:
		LUnorderedMap<uint64_t, MeshRenderCommand> allCommands;
	};

	class MeshRenderCommandsPassData
	{
		LArray<MeshRenderCommand*> allVisibleCommandsRef;
	public:
		void DrawAllCommands(PackedParams* sceneAndViewParam);
		void AddCommand(MeshRenderCommand* data);
		void ClearCommand();
	};


}

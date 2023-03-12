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

	class MeshRenderCommand
	{
	public:
		size_t mRenderObjectId;
		MaterialTemplateAsset* mMaterialTemplate;
		size_t mMaterialInstanceIndex;
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
	private:
		LQueue<size_t> mEmptyId;
		LUnorderedMap<uint64_t, MeshRenderCommand> mAllCommands;
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

#include "Graphics/Renderer/RenderMesh.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"

namespace luna::render
{
	size_t MeshRenderCommandsPacket::AddCommand(
		RenderObject* renderObject,
		MaterialTemplateAsset* materialAsset
	)
	{
		size_t id = 0;
		if (mEmptyId.empty())
		{
			id = mAllCommands.size();
		}
		else
		{
			id = mEmptyId.front();
			mEmptyId.pop();
		}
		MeshRenderCommand newCommand;
		newCommand.mMaterialInstanceIndex = id;
		newCommand.mMaterialTemplate = materialAsset;
		newCommand.mRenderObjectId = renderObject->mID;
		mAllCommands.insert({ id ,newCommand });
	}

	size_t MeshRenderCommandsPacket::AddCommand(
		RenderObject* renderObject,
		const LString& materialAsset
	)
	{
		return AddCommand(renderObject, sAssetModule->LoadAsset<MaterialTemplateAsset>(materialAsset));
	}

	void MeshRenderCommandsPassData::AddCommand(MeshRenderCommand* data)
	{
		allVisibleCommandsRef.push_back(data);
	}
	
	void MeshRenderCommandsPassData::ClearCommand()
	{
		allVisibleCommandsRef.clear();
	}

	void MeshRenderCommandsPassData::DrawAllCommands(PackedParams* sceneAndViewParam)
	{

	}
}
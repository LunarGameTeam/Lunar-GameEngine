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
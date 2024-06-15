#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHICmdList.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MaterialTemplate.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"

namespace luna::graphics 
{

luna::graphics::RenderResourceContext* sRenderResourceContext = nullptr;

luna::graphics::RenderDrawContext* sRenderDrawContext = nullptr;

void RenderResourceContext::Init()
{
	LArray<ShaderMacro*> emptyShaderMacros;
	mDefaultShaderPbr = sAssetModule->LoadAsset<ShaderAsset>("/assets/built-in/Shader/Pbr.hlsl");
	mDefaultShaderVertexPbrInstance = mDefaultShaderPbr->GenerateShaderInstance(RHIShaderType::Vertex, emptyShaderMacros);
	mDefaultShaderFragmentPbrInstance = mDefaultShaderPbr->GenerateShaderInstance(RHIShaderType::Pixel, emptyShaderMacros);
	PARAM_ID(ViewBuffer);
	PARAM_ID(SceneBuffer);
}

RHICBufferDesc RenderResourceContext::GetDefaultShaderConstantBufferDesc(ShaderParamID name)
{
	if (mDefaultShaderVertexPbrInstance->GetRhiShader()->HasUniformBuffer(name))
	{
		return mDefaultShaderVertexPbrInstance->GetRhiShader()->GetUniformBuffer(name);
	}
	else if (mDefaultShaderFragmentPbrInstance->GetRhiShader()->HasUniformBuffer(name))
	{
		return mDefaultShaderFragmentPbrInstance->GetRhiShader()->GetUniformBuffer(name);
	}
	RHICBufferDesc empty;
	return empty;
}

void RenderDrawContext::Init()
{
	
}

void RenderDrawContext::BindDrawCommandPassDesc(RHICmdList* cmdList, const RenderPassDesc& desc)
{
	cmdList->BindDrawCommandPassDesc(desc);
}

void RenderDrawContext::EndRenderPass(RHICmdList* cmdList)
{
	cmdList->EndRender();
}

void RenderDrawContext::DrawFullScreen(RHICmdList* cmdList, graphics::MaterialInstanceGraphBase* mat)
{
	DrawMesh(cmdList,mFullScreenRenderMesh, mat);
}

void RenderDrawContext::DrawMesh(RHICmdList* cmdList, graphics::RenderAssetDataMesh* mesh, graphics::MaterialInstanceGraphBase* mat)
{
	ZoneScoped;
	DrawMeshInstanced(cmdList,mesh, mat, 1);
}

void RenderDrawContext::Dispatch(RHICmdList* cmdList, MaterialInstanceComputeBase* mat, LVector4i dispatchSize)
{
	auto pipeline = mat->GetPipeline();
	cmdList->SetPipelineState(pipeline);
	mat->BindToPipeline(cmdList);
	cmdList->Dispatch(dispatchSize.x(), dispatchSize.y(), dispatchSize.z());
}

void RenderDrawContext::DrawMeshBatch(RHICmdList* cmdList, const MeshDrawCommandBatch& meshDrawCommand)
{
	RHIVertexLayout layout = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexLayout();
	auto pipeline = meshDrawCommand.mDrawParameter.mMtl->GetPipeline(&layout, cmdList->GetCurPassDesc());
	cmdList->SetPipelineState(pipeline);
	meshDrawCommand.mDrawParameter.mMtl->BindToPipeline(cmdList);
	size_t vertexCount = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexSize();
	RHIResource* vb = meshDrawCommand.mDrawParameter.mRenderMeshs->GetVertexBuffer();
	size_t indexCount = meshDrawCommand.mDrawParameter.mRenderMeshs->GetIndexSize();
	RHIResource* ib = meshDrawCommand.mDrawParameter.mRenderMeshs->GetIndexBuffer();
	LArray<RHIVertexBufferDesc> descs;
	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
	vbDesc.mOffset = 0;
	vbDesc.mBufferSize = vertexCount * meshDrawCommand.mDrawParameter.mRenderMeshs->GetStridePerVertex();
	vbDesc.mVertexStride = meshDrawCommand.mDrawParameter.mRenderMeshs->GetStridePerVertex();
	vbDesc.mVertexRes = vb;
	cmdList->SetVertexBuffer(descs, 0);
	cmdList->SetIndexBuffer(ib);
	cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
	LVector4i data;
	data.x() = meshDrawCommand.mRoOffset;
	cmdList->PushInt32Constant(0,&data,sizeof(data), meshDrawCommand.mDrawParameter.mMtl->GetAsset()->GetBindingSetLayout(), RHICmdListType::Graphic3D);
	cmdList->DrawIndexedInstanced((uint32_t)indexCount, meshDrawCommand.mDrawCount, 0, 0, 0);
}

void RenderDrawContext::DrawMeshInstanced(
	RHICmdList* cmdList,
	RenderAssetDataMesh* mesh,
	MaterialInstanceGraphBase* mat,
	int32_t instancingSize /*= 1*/
)
{
	ZoneScoped;
	RHIVertexLayout layout = mesh->GetVertexLayout();

	auto pipeline = mat->GetPipeline(&layout, cmdList->GetCurPassDesc());

	cmdList->SetPipelineState(pipeline);
	mat->BindToPipeline(cmdList);

	size_t vertexCount = mesh->GetVertexSize();

	RHIResource* vb = mesh->GetVertexBuffer();
	size_t indexCount = mesh->GetIndexSize();
	RHIResource* ib = mesh->GetIndexBuffer();

	LArray<RHIVertexBufferDesc> descs;
	RHIVertexBufferDesc& vbDesc = descs.emplace_back();
	vbDesc.mOffset = 0;
	vbDesc.mBufferSize = vertexCount * mesh->GetStridePerVertex();
	vbDesc.mVertexStride = mesh->GetStridePerVertex();
	vbDesc.mVertexRes = vb;

	cmdList->SetVertexBuffer(descs, 0);
	cmdList->SetIndexBuffer(ib);
	switch (((MaterialGraphAsset*)mat->GetAsset())->GetPrimitiveType())
	{
	case RHIPrimitiveTopologyType::Triangle:
		cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::TriangleList);
		break;
	case RHIPrimitiveTopologyType::Line:
		cmdList->SetDrawPrimitiveTopology(RHIPrimitiveTopology::LineList);
		break;
	default:
		break;
	}
	cmdList->DrawIndexedInstanced((uint32_t)indexCount, instancingSize, 0, 0, 0);
}



void GenerateGlobelRenderResourceContext()
{
	sRenderResourceContext = new RenderResourceContext();
	sRenderResourceContext->Init();
}

void GenerateGlobelEncoderHelper()
{
	sRenderDrawContext = new RenderDrawContext();
	sRenderDrawContext->Init();
}

}



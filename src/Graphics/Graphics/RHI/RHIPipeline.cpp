#pragma once

#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/RHI/RHIDevice.h"
namespace luna::graphics
{
	RHIBindingSetLayoutPtr GenerateAndCompileShaderLayout(
		RHIDevice* device,
		const LArray<RHIShaderBlob*>& shaderPack
	)
	{
		std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
		for (RHIShaderBlob* mShaderDta : shaderPack)
		{
			for (auto& it : mShaderDta->mBindPoints)
			{
				auto& bindKey = it.second;
				result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
			}
		}
		std::vector<RHIBindPoint> bindingKeys;
		for (auto it : result)
		{
			bindingKeys.push_back(it.second);
		}
		return device->CreateBindingSetLayout(bindingKeys);
	}


	void RHIPipelineState::Create(RHIDevice* device)
	{
		LArray<RHIShaderBlob*> shaderPack;
		for (auto& itor : mPSODesc->mShaders)
		{
			shaderPack.push_back(itor.second.get());
		}
		mBindingSetLayout = GenerateAndCompileShaderLayout(device,shaderPack);
		CreateImpl();
	}

	RHIPipelineStateCompute::RHIPipelineStateCompute(LSharedPtr<RHIPipelineStateDescBase> psoDesc) : RHIPipelineState(psoDesc, RHICmdListType::Compute)
	{

	}

	void RHIPipelineStateCompute::CreateImpl()
	{
		RHIPipelineStateComputeDesc* graphPipelineDesc = static_cast<RHIPipelineStateComputeDesc*>(mPSODesc.get());
		CreateComputePipelineImpl(graphPipelineDesc);
	}

	RHIPipelineStateGraphic::RHIPipelineStateGraphic(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc,
		const RHIVertexLayout &inputLayout,
		const RenderPassDesc &renderPassDesc
	):
		RHIPipelineState(psoDesc, RHICmdListType::Graphic3D),
		mInputLayout(inputLayout),
		mRenderPassDesc(renderPassDesc)
	{

	}

	void RHIPipelineStateGraphic::CreateImpl()
	{
		RHIPipelineStateGraphDrawDesc* graphPipelineDesc = static_cast<RHIPipelineStateGraphDrawDesc*>(mPSODesc.get());
		CreateGraphDrawPipelineImpl(graphPipelineDesc, mInputLayout, mRenderPassDesc);
	}
}

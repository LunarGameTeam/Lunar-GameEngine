#pragma once
#include "Graphics/RHI/RhiUtils/RHIPipelineCache.h"
#include "Graphics/RHI/RHIDevice.h"
namespace luna::graphics
{
	size_t RhiObjectCache::GetDataGlobelId(const RHIObject* customData)
	{
		auto dataExist = mDataGlobelId.find(customData);
		if (dataExist != mDataGlobelId.end())
		{
			return dataExist->second;
		}
		return size_t(-1);
	}

	void RhiObjectCache::ReleaseData(LMemoryHash& newHash)
	{
		auto dataExist = mData.find(newHash);
		if (dataExist != mData.end())
		{
			mData.erase(dataExist);
			emptyID.push(mDataGlobelId[dataExist->second]);
			mDataGlobelId.erase(dataExist->second);
		}
	}

	RHIShaderBlobPtr ShaderBlobCache::CreateShader(const RHIShaderDesc& desc)
	{
		Log("Graphics", "Create shader:{}", desc.mName);

		std::function<void(luna::LMemoryHash&, const luna::graphics::RHIShaderDesc&)> dataHashFunc = [&](luna::LMemoryHash& newHash, const luna::graphics::RHIShaderDesc& desc)
		{
			newHash.Combine((uint8_t*)desc.mName.c_str(), desc.mName.Length() * sizeof(char));
			//newHash.Combine((uint8_t*)desc.mContent.c_str(), desc.mContent.Length() * sizeof(char));
			newHash.Combine((uint8_t*)desc.mEntryPoint.c_str(), desc.mEntryPoint.Length() * sizeof(char));
			newHash.Combine((uint8_t*)&desc.mType, sizeof(desc.mType));
			LArray<RhiShaderMacro> curMacroValues = desc.mShaderMacros;
			std::sort(curMacroValues.begin(), curMacroValues.end(), [&](const RhiShaderMacro& a, const RhiShaderMacro& b)->bool {
				return a.mMacroName.Compare(b.mMacroName) > 0;
				}
			);
			for (RhiShaderMacro& eachMacro : curMacroValues)
			{
				newHash.Combine((uint8_t*)eachMacro.mMacroName.c_str(), eachMacro.mMacroName.Length() * sizeof(char));
				char divide = '\t';
				newHash.Combine((uint8_t*)&divide, sizeof(divide));
				newHash.Combine((uint8_t*)eachMacro.mMacroValue.c_str(), eachMacro.mMacroValue.Length() * sizeof(char));
			}
			newHash.GenerateHash();
		};
		std::function<RHIShaderBlobPtr(const RHIShaderDesc& desc)> dataCreateFunc = [&](const RHIShaderDesc& desc)->RHIShaderBlobPtr
		{
			return sGlobelRenderDevice->CreateShader(desc);
		};
		return CreateRHIObject<RHIShaderBlob, const luna::graphics::RHIShaderDesc&>(
			desc,
			dataHashFunc,
			dataCreateFunc
		);
	}

	PipelineStateCache::PipelineStateCache(ShaderBlobCache* shaderCache)
	{
		mShaderCache = shaderCache;
	}

	void PipelineStateCache::PackShaderToMemory(luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData)
	{
		size_t shader_id = -1;
		if (shaderData != nullptr)
		{
			shader_id = mShaderCache->GetDataGlobelId(shaderData);

		}
		newHash.Combine((uint8_t*)&shader_id, sizeof(size_t));
	}

	RHIPipelineStatePtr PipelineStateCache::CreatePipelineGraphic(
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	)
	{
		std::function<void(luna::LMemoryHash&, const RHIPipelineStateGraphDrawDesc&,
			const RHIVertexLayout&,
			const RenderPassDesc&)> dataHashFunc = [&](
				luna::LMemoryHash& newHash,
				const RHIPipelineStateGraphDrawDesc& desc,
				const RHIVertexLayout& inputLayout,
				const RenderPassDesc& renderPassDesc
				)
		{
			//graph desc shader信息
			for (auto& eachShader : desc.mShaders)
			{
				PackShaderToMemory(newHash, eachShader.second.get());
			}
			//input element信息
			for (auto& element : inputLayout.mElements)
			{
				newHash.Combine((uint8_t*)&element.mElementType, sizeof(element.mElementType));
				newHash.Combine((uint8_t*)&element.mElementCount, sizeof(element.mElementCount));
				newHash.Combine((uint8_t*)&element.mUsage, sizeof(element.mUsage));
				newHash.Combine((uint8_t*)&element.mOffset, sizeof(element.mOffset));
				newHash.Combine((uint8_t*)&element.mBufferSlot, sizeof(element.mBufferSlot));
				newHash.Combine((uint8_t*)&element.mInstanceUsage, sizeof(element.mInstanceUsage));
			}
			//pass信息
			for (auto& view : renderPassDesc.mColorView)
			{
				const RHITextureFormat& srv_desc = view->mBindResource->GetDesc().Format;
				newHash.Combine((const uint8_t*)&srv_desc, sizeof(srv_desc));
			}
			if (renderPassDesc.mDepths.size() > 0)
			{
				const RHITextureFormat& dsv_desc = renderPassDesc.mDepthStencilView->mBindResource->GetDesc().Format;
				newHash.Combine((const uint8_t*)&dsv_desc, sizeof(dsv_desc));
			}
			else
			{
				RHITextureFormat dsv_desc = RHITextureFormat::FORMAT_UNKNOWN;
				newHash.Combine((uint8_t*)&dsv_desc, sizeof(dsv_desc));
			}
			//graph desc state信息
			newHash.Combine((uint8_t*)&desc.BlendState.AlphaToCoverageEnable, sizeof(desc.BlendState.AlphaToCoverageEnable));
			newHash.Combine((uint8_t*)&desc.BlendState.IndependentBlendEnable, sizeof(desc.BlendState.IndependentBlendEnable));
			newHash.Combine((uint8_t*)desc.BlendState.RenderTarget.data(), desc.BlendState.RenderTarget.size() * sizeof(RHIBlendStateTargetDesc));
			newHash.Combine((uint8_t*)&desc.SampleMask, sizeof(desc.SampleMask));
			newHash.Combine((uint8_t*)&desc.RasterizerState, sizeof(desc.RasterizerState));
			newHash.Combine((uint8_t*)&desc.DepthStencilState, sizeof(desc.DepthStencilState));
			newHash.Combine((uint8_t*)&desc.IBStripCutValue, sizeof(desc.IBStripCutValue));
			newHash.Combine((uint8_t*)&desc.PrimitiveTopologyType, sizeof(desc.PrimitiveTopologyType));
			newHash.Combine((uint8_t*)&desc.NumRenderTargets, sizeof(desc.NumRenderTargets));
			newHash.Combine((uint8_t*)&desc.SampleDesc, sizeof(desc.SampleDesc));
			newHash.Combine((uint8_t*)&desc.NodeMask, sizeof(desc.NodeMask));
			newHash.GenerateHash();
		};

		std::function<RHIPipelineStatePtr(
			const RHIPipelineStateGraphDrawDesc& desc,
			const RHIVertexLayout& inputLayout,
			const RenderPassDesc& renderPassDesc)> dataCreateFunc = [&](
				const RHIPipelineStateGraphDrawDesc& desc,
				const RHIVertexLayout& inputLayout,
				const RenderPassDesc& renderPassDesc)->RHIPipelineStatePtr
		{
			auto vsItor = desc.mShaders.find(RHIShaderType::Vertex);
			if (vsItor != desc.mShaders.end())
			{
				Log("Graphics", "Create pipeline for shader:{}", vsItor->second->mDesc.mName);
			}
			return sGlobelRenderDevice->CreatePipelineGraphic(desc, inputLayout, renderPassDesc);
		};

		return CreateRHIObject<RHIPipelineState,
			const RHIPipelineStateGraphDrawDesc&,
			const RHIVertexLayout&,
			const RenderPassDesc&>(
				desc,
				inputLayout,
				renderPassDesc,
				dataHashFunc,
				dataCreateFunc
			);
	}

	RHIPipelineStatePtr PipelineStateCache::CreatePipelineCompute(
		const RHIPipelineStateComputeDesc& desc
	)
	{
		std::function<void(luna::LMemoryHash&, const luna::graphics::RHIPipelineStateComputeDesc&)> dataHashFunc = [&](luna::LMemoryHash& newHash, const luna::graphics::RHIPipelineStateComputeDesc& desc)
		{
			for (auto& eachShader : desc.mShaders)
			{
				PackShaderToMemory(newHash, eachShader.second.get());
			}
			newHash.GenerateHash();
		};
		std::function<RHIPipelineStatePtr(const RHIPipelineStateComputeDesc& desc)> dataCreateFunc = [&](const RHIPipelineStateComputeDesc& desc)->RHIPipelineStatePtr
		{
			auto vsItor = desc.mShaders.find(RHIShaderType::Compute);
			if (vsItor != desc.mShaders.end())
			{
				Log("Graphics", "Create pipeline for shader:{}", vsItor->second->mDesc.mName);
			}
			return sGlobelRenderDevice->CreatePipelineCompute(desc);
		};
		return CreateRHIObject<RHIPipelineState, const luna::graphics::RHIPipelineStateComputeDesc&>(
			desc,
			dataHashFunc,
			dataCreateFunc
		);
	}

	CmdSignatureCache::CmdSignatureCache(PipelineStateCache* pipelineCache)
	{
		mPipelineCache = pipelineCache;
	}
	RHICmdSignaturePtr CmdSignatureCache::CreateCmdSignature(
		RHIPipelineState* pipeline,
		const LArray<CommandArgDesc>& allCommondDesc
	)
	{
		std::function<void(luna::LMemoryHash&, RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)> dataHashFunc = [&](luna::LMemoryHash& newHash, RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
		{
			size_t pipelineId = mPipelineCache->GetDataGlobelId(pipeline);
			newHash.Combine((uint8_t*)&pipelineId, sizeof(size_t));
			newHash.Combine((uint8_t*)allCommondDesc.data(), allCommondDesc.size() * sizeof(CommandArgDesc));
			newHash.GenerateHash();
		};
		std::function<RHICmdSignaturePtr(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)> dataCreateFunc = [&](
			 RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)->RHICmdSignaturePtr
		{
			return sGlobelRenderDevice->CreateCmdSignature(pipeline, allCommondDesc);
		};
		return CreateRHIObject<RHICmdSignature, RHIPipelineState*, const LArray<CommandArgDesc>&>(
			pipeline,
			allCommondDesc,
			dataHashFunc,
			dataCreateFunc
		);
	}
}

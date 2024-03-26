#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/RHI/RHICmdList.h"
#include "Core/Foundation/MemoryHash.h"

namespace luna::graphics
{
	class RhiObjectCache
	{
		LQueue<size_t> emptyID;

		LUnorderedMap<LMemoryHash, TRHIPtr<RHIObject>> mData;

		LUnorderedMap<const RHIObject*, size_t> mDataGlobelId;
	public:
		size_t GetDataGlobelId(const RHIObject* customData);

		void ReleaseData(LMemoryHash& newHash);
	protected:
		template <typename T, typename... Args>
		TRHIPtr<T> CreateRHIObject(
			Args... inputDesc,
			std::function<void(LMemoryHash&, Args...)>& dataHashFunc,
			std::function<TRHIPtr<T>(Args...)> dataCreateFunc
		)
		{
			LMemoryHash newHash;
			dataHashFunc(newHash, inputDesc...);
			auto dataExist = mData.find(newHash);
			if (dataExist != mData.end())
			{
				return dataExist->second;
			}
			TRHIPtr<T> newData = dataCreateFunc(inputDesc...);
			mData.insert({ newHash ,newData });
			if (emptyID.empty())
			{
				size_t newID = mDataGlobelId.size();
				mDataGlobelId.insert({ newData ,newID });
			}
			else
			{
				size_t newID = emptyID.front();
				emptyID.pop();
				mDataGlobelId.insert({ newData ,newID });
			}
			return newData;
		}
	};

	class ShaderBlobCache :public RhiObjectCache
	{
	public:
		RHIShaderBlobPtr CreateShader(const RHIShaderDesc& desc);
	};

	class PipelineStateCache :public RhiObjectCache
	{
	public:
		PipelineStateCache(ShaderBlobCache* shaderCache);

		RHIPipelineStatePtr CreatePipelineGraphic(
			const RHIPipelineStateGraphDrawDesc& desc,
			const RHIVertexLayout& inputLayout,
			const RenderPassDesc& renderPassDesc
		);

		RHIPipelineStatePtr CreatePipelineCompute(
			const RHIPipelineStateComputeDesc& desc
		);
	private:
		ShaderBlobCache* mShaderCache = nullptr;

		void PackShaderToMemory(luna::LMemoryHash& newHash, const RHIShaderBlob* shaderData);
	};

	class CmdSignatureCache :public RhiObjectCache
	{
	public:
		CmdSignatureCache(PipelineStateCache* pipelineCache);
		RHICmdSignaturePtr CreateCmdSignature(
			RHIPipelineState* pipeline,
			const LArray<CommandArgDesc>& allCommondDesc
		);
	private:
		PipelineStateCache* mPipelineCache = nullptr;
	};

	class RENDER_API CommandArgBufferPool
	{
		LMemoryHash mCommandSignatureHash;
		RHICmdSignaturePtr mCommandSignature;
		LArray<RHICmdArgBuffer> mArgBufferPool;
		LQueue<size_t> mEmptyBuffer;
	public:
		CommandArgBufferPool(
			RHIPipelineState* pipeline,
			const LArray<CommandArgDesc>& allCommondDesc
		);

	};

}

#include"Graphics/Renderer/RenderGpuScene.h"
namespace luna::graphics
{
	GpuSceneUploadComputeCommand* GpuRenderScene::AddComputeCommand()
	{
		mAllComputeCommand.emplace_back();
		return &mAllComputeCommand.back();
	}

	GpuSceneUploadCopyCommand* GpuRenderScene::AddCopyCommand()
	{
		mAllCopyCommand.emplace_back();
		return &mAllCopyCommand.back();
	}

	void GpuRenderScene::ExcuteCopy()
	{
		LArray<ResourceBarrierDesc> curResourceBarrier;
		//修改资源状态
		for (auto& eachCommand : mAllComputeCommand)
		{
			for (auto& eachInput : eachCommand.mStorageBufferInput)
			{
				ResourceBarrierDesc newBarrierDesc;
				newBarrierDesc.mBarrierRes = eachInput.second;
				newBarrierDesc.mStateBefore = ResourceState::kGenericRead;
				newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
				curResourceBarrier.push_back(newBarrierDesc);
			}
			for (auto& eachOut : eachCommand.mStorageBufferInput)
			{
				ResourceBarrierDesc newBarrierDesc;
				newBarrierDesc.mBarrierRes = eachOut.second;
				newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
				newBarrierDesc.mStateAfter = ResourceState::kUnorderedAccess;
				curResourceBarrier.push_back(newBarrierDesc);
			}
		}
		for (auto& eachCommand : mAllCopyCommand)
		{
			ResourceBarrierDesc newInputBarrierDesc;
			newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
			newInputBarrierDesc.mStateBefore = ResourceState::kGenericRead;
			newInputBarrierDesc.mStateAfter = ResourceState::kCopySource;
			curResourceBarrier.push_back(newInputBarrierDesc);

			ResourceBarrierDesc newOutputBarrierDesc;
			newOutputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
			newOutputBarrierDesc.mStateBefore = ResourceState::kVertexAndConstantBuffer;
			newOutputBarrierDesc.mStateAfter = ResourceState::kCopyDest;
			curResourceBarrier.push_back(newInputBarrierDesc);
		}
		sRenderModule->mRenderContext->mBarrierCmd->GetCmdList()->ResourceBarrierExt(curResourceBarrier);
		//执行GPUscene的更新指令
		for (auto& eachCommand : mAllComputeCommand)
		{
			sRenderModule->mRenderContext->Dispatch(eachCommand.mComputeMaterial, eachCommand.mDispatchSize);
		}
		for (auto& eachCommand : mAllCopyCommand)
		{
			sRenderModule->mRenderContext->mGraphicCmd->GetCmdList()->CopyBufferToBuffer(eachCommand.mStorageBufferOutput, eachCommand.mDstOffset, eachCommand.mUniformBufferInput, eachCommand.mSrcOffset, eachCommand.mCopyLength);
		}
		//恢复资源状态
		curResourceBarrier.clear();
		for (auto& eachCommand : mAllComputeCommand)
		{
			for (auto& eachInput : eachCommand.mStorageBufferInput)
			{
				ResourceBarrierDesc newBarrierDesc;
				newBarrierDesc.mBarrierRes = eachInput.second;
				newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
				newBarrierDesc.mStateAfter = ResourceState::kGenericRead;
				curResourceBarrier.push_back(newBarrierDesc);
			}
			for (auto& eachOut : eachCommand.mStorageBufferInput)
			{
				ResourceBarrierDesc newBarrierDesc;
				newBarrierDesc.mBarrierRes = eachOut.second;
				newBarrierDesc.mStateBefore = ResourceState::kUnorderedAccess;
				newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
				curResourceBarrier.push_back(newBarrierDesc);
			}
		}
		for (auto& eachCommand : mAllCopyCommand)
		{
			ResourceBarrierDesc newInputBarrierDesc;
			newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
			newInputBarrierDesc.mStateBefore = ResourceState::kCopySource;
			newInputBarrierDesc.mStateAfter = ResourceState::kGenericRead;
			curResourceBarrier.push_back(newInputBarrierDesc);

			ResourceBarrierDesc newOutputBarrierDesc;
			newOutputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
			newOutputBarrierDesc.mStateBefore = ResourceState::kCopyDest;
			newOutputBarrierDesc.mStateAfter = ResourceState::kVertexAndConstantBuffer;
			curResourceBarrier.push_back(newInputBarrierDesc);
		}
		//情况G所有puScene的更新指令
		mAllComputeCommand.clear();
		mAllCopyCommand.clear();
	}
}
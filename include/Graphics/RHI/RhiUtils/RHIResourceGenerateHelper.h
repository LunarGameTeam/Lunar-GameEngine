#pragma once
#include "Graphics/RHI/RhiUtils/RHIUniformBuffer.h"
#include "Graphics/RHI/RhiUtils/RHIPipelineCache.h"
namespace luna::graphics
{
	struct StaticSampler
	{
		void Init(SamplerDesc& desc, ViewDesc& view);

		RHIResourcePtr mSampler;

		RHIViewPtr     mView;
	};

	class RhiResourceGenerateHelper
	{
		LSharedPtr<DeviceResourceGenerateHelper> mDeviceResourceGen;

		LSharedPtr<RhiUniformBufferPool> mUniformGenPool;

		LSharedPtr<ShaderBlobCache> mShaderBlobCache;

		LSharedPtr<PipelineStateCache> mPipelineCache;

		LSharedPtr<CmdSignatureCache> mCmdSignatureCache;
	public:
		RhiResourceGenerateHelper();

		void Init();

		inline DeviceResourceGenerateHelper* GetDeviceResourceGenerator()
		{
			return mDeviceResourceGen.get();
		};

		inline RhiUniformBufferPool* GetDeviceUniformObjectGenerator()
		{
			return mUniformGenPool.get();
		}

		inline ShaderBlobCache* GetDeviceShaderGenerator()
		{
			return mShaderBlobCache.get();
		}

		inline PipelineStateCache* GetDevicePipelineGenerator()
		{
			return mPipelineCache.get();
		}

		RHIDescriptorPoolPtr GetDefaultDescriptorPool() { return mDefaultPool; }

		inline StaticSampler& GetClampSamper() 
		{
			return mClamp;
		}

		inline StaticSampler& GetRepeatSamper()
		{
			return mRepeat;
		}

		RHIBindingSetPtr CreateBindingsetByDefaultPool(RHIBindingSetLayoutPtr layout);
	private:
		//Samplers
		StaticSampler         mClamp;

		StaticSampler         mRepeat;

		RHIDescriptorPoolPtr  mDefaultPool;
	};

	void RENDER_API GenerateRhiResourceGenerator();

	void RENDER_API ReleaseRhiResourceGenerator();

	luna::graphics::RhiResourceGenerateHelper* sGlobelRhiResourceGenerator = nullptr;
}

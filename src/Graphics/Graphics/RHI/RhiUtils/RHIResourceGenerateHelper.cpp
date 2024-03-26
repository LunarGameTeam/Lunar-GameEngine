#pragma once

#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
#include "Graphics/RHI/RHIDevice.h"
namespace luna::graphics
{
	void StaticSampler::Init(SamplerDesc& desc, ViewDesc& view)
	{
		mSampler = sGlobelRenderDevice->CreateSamplerExt(desc);
		mView = sGlobelRenderDevice->CreateView(view);
		mView->BindResource(mSampler);
	};

	RhiResourceGenerateHelper::RhiResourceGenerateHelper()
	{

	}

	void RhiResourceGenerateHelper::Init()
	{
		//Ä¬ÈÏ²ÉÑùÆ÷
		SamplerDesc descClamp;
		descClamp.filter = SamplerFilter::kMinMagMipLinear;
		descClamp.func = SamplerComparisonFunc::kNever;
		descClamp.mode = SamplerTextureAddressMode::kClamp;
		ViewDesc samplerDescClamp;
		samplerDescClamp.mViewType = RHIViewType::kSampler;
		mClamp.Init(descClamp, samplerDescClamp);

		SamplerDesc descRepeat;
		descRepeat.filter = SamplerFilter::kAnisotropic;
		descRepeat.func = SamplerComparisonFunc::kNever;
		descRepeat.mode = SamplerTextureAddressMode::kWrap;
		ViewDesc samplerDescRepeat;
		samplerDescRepeat.mViewType = RHIViewType::kSampler;
		mRepeat.Init(descRepeat, samplerDescRepeat);
		//Ä¬ÈÏÃèÊö·û³Ø
		DescriptorPoolDesc poolDesc;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::CBV_SRV_UAV] = 4096 * 4;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::SAMPLER] = 1024;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::DSV] = 100;
		poolDesc.mPoolAllocateSizes[DescriptorHeapType::RTV] = 100;
		mDefaultPool = sGlobelRenderDevice->CreateDescriptorPool(poolDesc);
	}

	void ReleaseRhiResourceGenerator()
	{
		if (sGlobelRhiResourceGenerator != nullptr)
		{
			delete sGlobelRhiResourceGenerator;
		}
	}

	void GenerateRhiResourceGenerator()
	{
		sGlobelRhiResourceGenerator = new RhiResourceGenerateHelper();
	}

	RHIBindingSetPtr RhiResourceGenerateHelper::CreateBindingsetByDefaultPool(RHIBindingSetLayoutPtr layout)
	{
		return sGlobelRenderDevice->CreateBindingSet(mDefaultPool, layout);
	}
}

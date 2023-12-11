#pragma once

#include "Graphics/RHI/RHIPtr.h"


namespace luna::graphics
{

class VulkanDevice; 
struct RHIShaderDesc;
struct RHIResDesc;
struct RenderPassDesc;
struct SamplerDesc;
struct DescriptorPoolDesc;
struct ViewDesc;
struct FrameBufferDesc;
struct RHIBufferDesc;
struct RHIMemoryDesc;
struct PassColorDesc;
struct PassDepthStencilDesc;

class RHIDevice;

class RHIView;
using RHIViewPtr           = TRHIPtr<RHIView> ;

class RHIDescriptorPool;
using RHIDescriptorPoolPtr = TRHIPtr<RHIDescriptorPool> ;

class RHIFence;
using RHIFencePtr          = TRHIPtr<RHIFence> ;

class RHIFrameBuffer;
using RHIFrameBufferPtr    = TRHIPtr<RHIFrameBuffer> ;

class RHIMemory;
using RHIMemoryPtr         = TRHIPtr< RHIMemory> ;

class RHIPipelineState;
using RHIPipelineStatePtr  = TRHIPtr<RHIPipelineState> ;

class RHIShaderBlob;
using RHIShaderBlobPtr     = TRHIPtr<RHIShaderBlob> ;

class RHIResource;
using RHIResourcePtr       = TRHIPtr<RHIResource> ;

class RHICmdSignature;
using RHICmdSignaturePtr = TRHIPtr<RHICmdSignature>;

class RHIBindingSet;
using RHIBindingSetPtr	   = TRHIPtr<RHIBindingSet>;

class RHIBindingSetLayout;
using RHIBindingSetLayoutPtr  = TRHIPtr<RHIBindingSetLayout> ;

class RHISwapChain;
using RHISwapChainPtr      = TRHIPtr<RHISwapChain> ;

class RHIRenderPass;
using RHIRenderPassPtr     = TRHIPtr<RHIRenderPass> ;

class RHICmdAllocator;
using RHICmdAllocatorPtr   = TRHIPtr<RHICmdAllocator> ;

class RHICmdList;
using RHICmdListPtr = TRHIPtr<RHICmdList>;

class RHISinglePoolSingleCmdList;
using RHISinglePoolSingleCmdListPtr = TRHIPtr<RHISinglePoolSingleCmdList>;

class RHISinglePoolMultiCmdList;
using RHISinglePoolMultiCmdListPtr = TRHIPtr<RHISinglePoolMultiCmdList>;

class RHIMultiFrameCmdList;
using RHIMultiFrameCmdListPtr = TRHIPtr<RHIMultiFrameCmdList>;



class RHIRenderQueue;
using RHIRenderQueuePtr = TRHIPtr<RHIRenderQueue>;

class RHIDevice;
using RHIDevicePtr = TRHIPtr<RHIDevice>;

}
#pragma once

#include "Graphics/RHI/RHIPtr.h"


namespace luna::render
{

struct RHIShaderDesc;
struct RHIPipelineStateDesc;
struct RHIResDesc;
struct RenderPassDesc;
struct SamplerDesc;
struct DescriptorPoolDesc;
struct ViewDesc;
struct FrameBufferDesc;
struct RHIBufferDesc;
struct RHITextureDesc;
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

class RHIGraphicCmdList;
using RHIGraphicCmdListPtr = TRHIPtr<RHIGraphicCmdList> ;

class RHIRenderQueue;
using RHIRenderQueuePtr = TRHIPtr<RHIRenderQueue>;

}
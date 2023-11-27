#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"
#include "Graphics/RHI/RHIShader.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIRenderPass.h"

namespace luna::graphics
{

//基础的pso格式
struct RHIPipelineStateDescBase
{
	LUnorderedMap<RHIShaderType, RHIShaderBlobPtr> mShaders;
};
//compute pipeline的pso格式
struct RHIPipelineStateComputeDesc : public RHIPipelineStateDescBase
{

};
RHIBindingSetLayoutPtr GenerateAndCompileShaderLayout(
	RHIDevice* device,
	const LArray<RHIShaderBlob*>& shaderPack
);
//绑定槽位的名称格式
struct RenderBindingSlotTypeDesc
{
	LString           name;
	RHIViewType type;
};

struct RHIBlendStateTargetDesc
{
	bool                BlendEnable           = true;
	bool                LogicOpEnable         = false;
	RHIRenderBlend      SrcBlend              = RHIRenderBlend::BLEND_ONE;
	RHIRenderBlend      DestBlend             = RHIRenderBlend::BLEND_ZERO;
	RHIBlendOption      BlendOp               = RHIBlendOption::BLEND_OP_ADD;
	RHIRenderBlend      SrcBlendAlpha         = RHIRenderBlend::BLEND_ONE;
	RHIRenderBlend      DestBlendAlpha        = RHIRenderBlend::BLEND_ZERO;
	RHIBlendOption      BlendOpAlpha          = RHIBlendOption::BLEND_OP_ADD;
	RHIBlendLogicOption LogicOp               = RHIBlendLogicOption::LOGIC_OP_NOOP;
	LColorWriteEnable   RenderTargetWriteMask = LColorWriteEnable::COLOR_WRITE_ENABLE_ALL;
};

struct RHIBlendStateDesc
{
	bool AlphaToCoverageEnable = false;
	bool IndependentBlendEnable = false;
	LArray<RHIBlendStateTargetDesc> RenderTarget;
};

struct RHIRasterizerStateDesc
{
	RHIRasterizerFillMode         FillMode              = RHIRasterizerFillMode::FILL_MODE_SOLID;
	RHIRasterizerCullMode         CullMode              = RHIRasterizerCullMode::BackFace;
	bool                          FrontCounterClockwise = false;
	int32_t                       DepthBias             = 0;
	float                         DepthBiasClamp        = 0;
	float                         SlopeScaledDepthBias  = 0;
	bool                          DepthClipEnable       = false;
	bool                          MultisampleEnable     = false;
	bool                          AntialiasedLineEnable = false;
	uint32_t                      ForcedSampleCount     = 0;
	RHIConservativeRasterizerMode ConservativeRaster    = RHIConservativeRasterizerMode::CONSERVATIVE_RASTERIZATION_MODE_OFF;
};

struct RHIDepthStencilOptionDesc
{
public:
	RHIStencilOption   StencilFailOp      = RHIStencilOption::STENCIL_OP_KEEP;
	RHIStencilOption   StencilDepthFailOp = RHIStencilOption::STENCIL_OP_KEEP;
	RHIStencilOption   StencilPassOp      = RHIStencilOption::STENCIL_OP_KEEP;
	RHIComparisionFunc StencilFunc        = RHIComparisionFunc::FuncAlways;
};

struct RHIDepthStencilStateDesc
{
	bool                      DepthEnable = true;
	bool					  DepthWrite = true;
	RHIComparisionFunc        DepthFunc = RHIComparisionFunc::FuncLess;
	bool                      StencilEnable = false;
	uint8_t                   StencilReadMask = 0xFF;
	uint8_t                   StencilWriteMask = 0xFF;
	RHIDepthStencilOptionDesc FrontFace;
	RHIDepthStencilOptionDesc BackFace;
};

//渲染pipeline的pso格式
struct RHIPipelineStateGraphDrawDesc : public RHIPipelineStateDescBase
{
	RHIBlendStateDesc           BlendState;
	uint32_t                    SampleMask            = 0xFFFFFFFF;
	RHIRasterizerStateDesc      RasterizerState;
	RHIDepthStencilStateDesc    DepthStencilState;
	RHIIndexBufferStripCutValue IBStripCutValue       = RHIIndexBufferStripCutValue::INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	RHIPrimitiveTopologyType    PrimitiveTopologyType = RHIPrimitiveTopologyType::Triangle;
	uint32_t                    NumRenderTargets      = 0;
	//此处废弃
	RHIGraphicSampleCount       SampleDesc;
	uint32_t                    NodeMask              = 0;
};

class RENDER_API RHIPipelineState : public RHIObject
{
protected:
	RHICmdListType                       mType;

	LSharedPtr<RHIPipelineStateDescBase> mPSODesc;

	RHIBindingSetLayoutPtr               mBindingSetLayout;
public:
	RHIPipelineState(LSharedPtr<RHIPipelineStateDescBase> psoDesc, RHICmdListType type)
	{
		mType = type;
		mPSODesc = psoDesc;
	};

	void Create(RHIDevice* device);

	RHIPipelineStateDescBase* GetDesc()
	{
		return mPSODesc.get();
	};

	virtual ~RHIPipelineState() override
	{
	};
private:
	virtual void CreateImpl() = 0;
};

class RENDER_API RHIPipelineStateCompute : public RHIPipelineState
{
public:
	RHIPipelineStateCompute(LSharedPtr<RHIPipelineStateDescBase> psoDesc);
private:
	void CreateImpl() override;

	virtual void CreateComputePipelineImpl(
		RHIPipelineStateComputeDesc* computePipelineDesc
	) = 0;
};

class RENDER_API RHIPipelineStateGraphic : public RHIPipelineState
{
	RHIVertexLayout mInputLayout;

	RenderPassDesc mRenderPassDesc;
public:
	RHIPipelineStateGraphic(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

private:
	void CreateImpl() override;

	virtual void CreateGraphDrawPipelineImpl(
		RHIPipelineStateGraphDrawDesc* graphPipelineDesc,
		RHIVertexLayout &inputLayout,
		RenderPassDesc &renderPassDesc
	) = 0;
};

}

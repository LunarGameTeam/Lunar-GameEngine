#pragma once
#include "render/render_config.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_binding_set_layout.h"
#include "render/rhi/rhi_shader.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_render_pass.h"

namespace luna::render
{

//绑定槽位的名称格式
struct RenderBindingSlotTypeDesc
{
	LString           name;
	RHIViewType type;
};

struct RHIBlendStateTargetDesc
{
	bool                BlendEnable           = false;
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
	RHIRasterizerCullMode         CullMode              = RHIRasterizerCullMode::CULL_MODE_BACK;
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


struct RHIPipelineStateObjectDesc
{
	RHIShaderBlobPtr            mVertexShader;
	RHIShaderBlobPtr            mPixelShader;
	RHIShaderBlobPtr            mGeometryShader;
	RHIShaderBlobPtr            mDominShader;
	RHIShaderBlobPtr            mHullShader;
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

struct RenderPipelineStateDescGraphic
{
	RHIPipelineStateObjectDesc mPipelineStateDesc = {};
	RHIVertexLayout mInputLayout;
	RenderPassDesc mRenderPassDesc;
};

struct RHIPipelineStateDesc
{
	RHICmdListType mType;
	RenderPipelineStateDescGraphic mGraphicDesc;
};

class RENDER_API RHIPipelineState : public RHIObject
{
public:
	RHIPipelineStateDesc                                 mPSODesc;
	LUnorderedMap<RHIShaderType, TRHIPtr<RHIShaderBlob>> mShaders;

	RHIBindingSetLayoutPtr                               mBindingSetLayout;
public:
	RHIPipelineState(const RHIPipelineStateDesc& pso_desc)
	{
		mPSODesc = pso_desc;
	};

	RHIPipelineStateDesc GetDesc()
	{
		return mPSODesc;
	};

	RHIShaderBlob* GetVertexShader() const { return mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader; }
	RHIShaderBlob* GetPixelShader() const { return mPSODesc.mGraphicDesc.mPipelineStateDesc.mPixelShader; }

	RHIBindPoint GetBindPoint(ShaderParamID id) const
	{
		return GetVertexShader()->GetBindPoint(id);
	};

	RHIConstantBufferDesc& GetConstantBufferDesc(const LString& name)
	{
		return GetVertexShader()->GetUniformBuffer(name);
	}

	RHIBindingSetLayoutPtr& GetBindingSetLayout()
	{
		return mBindingSetLayout;
	};

	~RHIPipelineState() override
	{
	};
};

}

#include "render/rhi/vulkan/vulkan_pipeline.h"

#include "render/pch.h"
#include "render/rhi/rhi_device.h"

#include "render/rhi/vulkan/vulkan_swapchain.h"

#include "render/rhi/vulkan/vulkan_swapchain.h"
#include "render/rhi/vulkan/vulkan_shader.h"
#include "render/rhi/vulkan/vulkan_render_pass.h"
#include "render/rhi/vulkan/vulkan_binding_set_layout.h"
#include "render/rhi/vulkan/vulkan_resource.h"


namespace luna::render
{


vk::CompareOp Convert(RHIComparisionFunc func)
{
	switch (func)
	{
	case RHIComparisionFunc::FuncNever:
		return vk::CompareOp::eNever;
	case RHIComparisionFunc::FuncLess:
		return vk::CompareOp::eLess;
	case RHIComparisionFunc::FuncEqual:
		return vk::CompareOp::eEqual;
	case RHIComparisionFunc::FuncLessEqual:
		return vk::CompareOp::eLessOrEqual;
	case RHIComparisionFunc::FuncGreater:
		return vk::CompareOp::eGreater;
	case RHIComparisionFunc::FuncNotEqual:
		return vk::CompareOp::eNotEqual;
	case RHIComparisionFunc::FuncGreaterEuqal:
		return vk::CompareOp::eGreaterOrEqual;
	case RHIComparisionFunc::FuncAlways:
		return vk::CompareOp::eAlways;
	default:
		assert(false);
	}
}

VulkanPipelineState::VulkanPipelineState(const RHIPipelineStateDesc& pso_desc)
	: RHIPipelineState(pso_desc)
{

	Init();
}

void VulkanPipelineState::Init()
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();

	vk::PipelineRenderingCreateInfo renderingInfos;
	std::vector<vk::Format> colors;
	for (auto& it : mPSODesc.mGraphicDesc.mRenderPass.mColors)
	{
		colors.push_back(Convert(it.mFormat));
	}
	renderingInfos.pColorAttachmentFormats = colors.data();
	renderingInfos.colorAttachmentCount = colors.size();
	if (!mPSODesc.mGraphicDesc.mRenderPass.mDepths.empty())
	{
		renderingInfos.stencilAttachmentFormat = Convert(mPSODesc.mGraphicDesc.mRenderPass.mDepths[0].mDepthStencilFormat);
 		renderingInfos.depthAttachmentFormat = Convert(mPSODesc.mGraphicDesc.mRenderPass.mDepths[0].mDepthStencilFormat);
	}
	
		

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};

	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader->As<VulkanShaderBlob>()->mShaderModule;
	vertShaderStageInfo.pName = "VSMain";

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;;
	fragShaderStageInfo.module = mPSODesc.mGraphicDesc.mPipelineStateDesc.mPixelShader->As<VulkanShaderBlob>()->mShaderModule;;
	fragShaderStageInfo.pName = "PSMain";

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = false;

	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.depthClampEnable = false;
	rasterizer.rasterizerDiscardEnable = false;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eFront;
	rasterizer.frontFace = vk::FrontFace::eClockwise;
	rasterizer.depthBiasEnable = false;

	vk::PipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR
		| vk::ColorComponentFlagBits::eG
		| vk::ColorComponentFlagBits::eB
		| vk::ColorComponentFlagBits::eA;

	colorBlendAttachment.blendEnable = VK_FALSE;

	vk::PipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	vk::PipelineDepthStencilStateCreateInfo depthCreateInfo{};
	depthCreateInfo.depthTestEnable = mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthEnable;
	depthCreateInfo.depthWriteEnable = mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthWrite;

	depthCreateInfo.depthCompareOp = Convert(mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthFunc);
	depthCreateInfo.depthBoundsTestEnable = false;
	depthCreateInfo.stencilTestEnable = false;
// 	depthCreateInfo.back = VkStencilOpState
// 	depthCreateInfo.front = Convert(m_desc.depth_stencil_desc.front_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);



	std::vector<vk::DynamicState> dynamicStates = {
		vk::DynamicState::eVertexInputBindingStride,
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
		
	};
	vk::PipelineDynamicStateCreateInfo dynamicState{};
	
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	std::vector<RHIBindPoint> bindingKeys;
	std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
	for (RHIBindPoint& bindKey : mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader->As<VulkanShaderBlob>()->mBindings)
	{
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (RHIBindPoint& bindKey : mPSODesc.mGraphicDesc.mPipelineStateDesc.mPixelShader->As<VulkanShaderBlob>()->mBindings)
	{
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (auto it : result)
	{
		bindingKeys.push_back(it.second);
	}
	int idx = 0;
	mBindingSetLayout = sRenderModule->GetRHIDevice()->CreateBindingSetLayout(bindingKeys);

	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	
	pipelineInfo.pVertexInputState = &mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader->As<VulkanShaderBlob>()->mVertexInputState;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = &depthCreateInfo;

	pipelineInfo.pDynamicState = &dynamicState;


	pipelineInfo.pNext = &renderingInfos;
	assert(mBindingSetLayout);
	pipelineInfo.layout = mBindingSetLayout->As <VulkanBindingSetLayout>()->mPipelineLayout;

	vk::Result   res;

	std::tie(res, mPipeline) = device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
	
}

}

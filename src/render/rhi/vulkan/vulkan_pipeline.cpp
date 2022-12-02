#include "render/rhi/vulkan/vulkan_pipeline.h"

#include "render/pch.h"
#include "render/rhi/rhi_device.h"

#include "render/rhi/vulkan/vulkan_swapchain.h"

#include "render/rhi/vulkan/vulkan_swapchain.h"
#include "render/rhi/vulkan/vulkan_shader.h"
#include "render/rhi/vulkan/vulkan_render_pass.h"
#include "render/rhi/vulkan/vulkan_binding_set_layout.h"


namespace luna::render
{


VkCompareOp Convert(RHIComparisionFunc func)
{
	switch (func)
	{
	case RHIComparisionFunc::FuncNever:
		return VK_COMPARE_OP_NEVER;
	case RHIComparisionFunc::FuncLess:
		return VK_COMPARE_OP_LESS;
	case RHIComparisionFunc::FuncEqual:
		return VK_COMPARE_OP_EQUAL;
	case RHIComparisionFunc::FuncLessEqual:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case RHIComparisionFunc::FuncGreater:
		return VK_COMPARE_OP_GREATER;
	case RHIComparisionFunc::FuncNotEqual:
		return VK_COMPARE_OP_NOT_EQUAL;
	case RHIComparisionFunc::FuncGreaterEuqal:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case RHIComparisionFunc::FuncAlways:
		return VK_COMPARE_OP_ALWAYS;
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
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader->As<VulkanShaderBlob>()->mShaderModule;
	vertShaderStageInfo.pName = "VSMain";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = mPSODesc.mGraphicDesc.mPipelineStateDesc.mPixelShader->As<VulkanShaderBlob>()->mShaderModule;;
	fragShaderStageInfo.pName = "PSMain";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	VkPipelineDepthStencilStateCreateInfo depthCreateInfo{};
	depthCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthCreateInfo.depthTestEnable = mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthEnable;
	depthCreateInfo.depthTestEnable = mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthWrite;

	depthCreateInfo.depthCompareOp = Convert(mPSODesc.mGraphicDesc.mPipelineStateDesc.DepthStencilState.DepthFunc);
	depthCreateInfo.depthBoundsTestEnable = false;
	depthCreateInfo.stencilTestEnable = false;
// 	depthCreateInfo.back = VkStencilOpState
// 	depthCreateInfo.front = Convert(m_desc.depth_stencil_desc.front_face, m_desc.depth_stencil_desc.stencil_read_mask, m_desc.depth_stencil_desc.stencil_write_mask);



	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
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

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
	if(mBindingSetLayout)

		pipelineInfo.layout = mBindingSetLayout->As <VulkanBindingSetLayout>()->mPipelineLayout;
	if(mPSODesc.mGraphicDesc.mRenderPass)
		pipelineInfo.renderPass = mPSODesc.mGraphicDesc.mRenderPass->As<VulkanRenderPass>()->mRenderPass;
	
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
		assert(false);
	}
}

}

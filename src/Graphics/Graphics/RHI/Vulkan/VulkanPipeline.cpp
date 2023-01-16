#include "Graphics/RHI/Vulkan/VulkanPipeline.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDevice.h"

#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"

#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanShader.h"
#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"

#include <Vulkan/vulkan_format_traits.hpp>


namespace luna::render
{


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
	for (auto& it : mPSODesc.mGraphicDesc.mRenderPassDesc.mColors)
	{
		colors.push_back(Convert(it.mFormat));
	}
	renderingInfos.pColorAttachmentFormats = colors.data();
	renderingInfos.colorAttachmentCount = (uint32_t)colors.size();
	if (!mPSODesc.mGraphicDesc.mRenderPassDesc.mDepths.empty())
	{
		renderingInfos.stencilAttachmentFormat = Convert(mPSODesc.mGraphicDesc.mRenderPassDesc.mDepths[0].mDepthStencilFormat);
 		renderingInfos.depthAttachmentFormat = Convert(mPSODesc.mGraphicDesc.mRenderPassDesc.mDepths[0].mDepthStencilFormat);
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

	switch (mPSODesc.mGraphicDesc.mPipelineStateDesc.RasterizerState.CullMode)
	{
	case RHIRasterizerCullMode::CULL_MODE_BACK:
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		break;
	case RHIRasterizerCullMode::CULL_MODE_FRONT:
		rasterizer.cullMode = vk::CullModeFlagBits::eFront;
		break;
	case RHIRasterizerCullMode::CULL_MODE_NONE:
		rasterizer.cullMode = vk::CullModeFlagBits::eNone;
		break;
	default:
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		break;
	}
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
		vk::DynamicState::eScissor,
		vk::DynamicState::ePrimitiveTopology
		
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
	

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	std::vector<vk::VertexInputAttributeDescription> inputAttributes;
	uint32_t inputLocationIndex = 0;
	LArray<vk::VertexInputBindingDescription> inputBindings;
	inputBindings.resize(16);
	uint32_t inputVertexBufferNum = 0;
	for (auto& vertexElement : mPSODesc.mGraphicDesc.mInputLayout.mElements)
	{
		auto& attr = inputAttributes.emplace_back();
		attr.location = inputLocationIndex;
		attr.binding = vertexElement.mBufferSlot;
		attr.offset = 0;
		if (vertexElement.mElementType == VertexElementType::Float)
		{
			if(vertexElement.mElementCount == 1)
				attr.format = vk::Format::eR32Sfloat;
			else if (vertexElement.mElementCount == 2)
				attr.format = vk::Format::eR32G32Sfloat;
			else if (vertexElement.mElementCount == 3)
				attr.format = vk::Format::eR32G32B32Sfloat;
			else if (vertexElement.mElementCount == 4)
				attr.format = vk::Format::eR32G32B32A32Sfloat;
		}
		else if(vertexElement.mElementType == VertexElementType::Int)
		{
			if (vertexElement.mElementCount == 1)
				attr.format = vk::Format::eR32Uint;
			else if (vertexElement.mElementCount == 2)
				attr.format = vk::Format::eR32G32Uint;
			else if (vertexElement.mElementCount == 3)
				attr.format = vk::Format::eR32G32B32Uint;
			else if (vertexElement.mElementCount == 4)
				attr.format = vk::Format::eR32G32B32A32Uint;
		}
		inputLocationIndex++;
		//����vertex buffer�ĸ�ʽ��Ϣ
		if (attr.binding + 1 > inputVertexBufferNum)
		{
			inputVertexBufferNum = attr.binding + 1;
		}
		if (vertexElement.mInstanceUsage == VertexElementInstanceType::PerInstance)
		{
			inputBindings[attr.binding].inputRate = vk::VertexInputRate::eInstance;
		}
		else
		{
			inputBindings[attr.binding].inputRate = vk::VertexInputRate::eVertex;
		}
		inputBindings[attr.binding].binding = attr.binding;
		inputBindings[attr.binding].stride = 0;
	}
	for (auto& attr : inputAttributes)
	{
		uint32_t format_size = vk::blockSize(attr.format);
		format_size = (uint32_t)Alignment(format_size, 8);
		attr.offset = inputBindings[attr.binding].stride;
		inputBindings[attr.binding].stride += format_size;
	}
	vertexInputInfo.pVertexAttributeDescriptions = inputAttributes.data();
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)inputAttributes.size();
	vertexInputInfo.pVertexBindingDescriptions = inputBindings.data();
	vertexInputInfo.vertexBindingDescriptionCount = inputVertexBufferNum;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
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

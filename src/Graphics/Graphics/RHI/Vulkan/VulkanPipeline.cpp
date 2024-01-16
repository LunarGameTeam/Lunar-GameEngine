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


namespace luna::graphics
{
VulkanPipelineStateGraphic::VulkanPipelineStateGraphic(
	LSharedPtr<RHIPipelineStateDescBase> psoDesc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
): RHIPipelineStateGraphic(psoDesc, inputLayout, renderPassDesc)
{

}

void VulkanPipelineStateGraphic::CreateGraphDrawPipelineImpl(
	RHIPipelineStateGraphDrawDesc* graphPipelineDesc,
	RHIVertexLayout& inputLayout,
	RenderPassDesc& renderPassDesc
)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();

	vk::PipelineRenderingCreateInfo renderingInfos;
	std::vector<vk::Format> colors;
	for (auto& it : renderPassDesc.mColorView)
	{
		colors.push_back(Convert(it->mBindResource->GetDesc().Format));
	}
	renderingInfos.pColorAttachmentFormats = colors.data();
	renderingInfos.colorAttachmentCount = (uint32_t)colors.size();
	if (!renderPassDesc.mDepths.empty())
	{
		renderingInfos.stencilAttachmentFormat = Convert(renderPassDesc.mDepths[0].mDepthStencilFormat);
		renderingInfos.depthAttachmentFormat = Convert(renderPassDesc.mDepths[0].mDepthStencilFormat);
	}

	LArray<vk::PipelineShaderStageCreateInfo> shaderStages;
	for (auto& eachShader : graphPipelineDesc->mShaders)
	{
		vk::PipelineShaderStageCreateInfo curInfo = {};
		switch (eachShader.first)
		{
		case RHIShaderType::Vertex:
		{
			curInfo.stage = vk::ShaderStageFlagBits::eVertex;
			curInfo.module = eachShader.second->As<VulkanShaderBlob>()->mShaderModule;
			curInfo.pName = "VSMain";
			break;
		}
		case RHIShaderType::Pixel:
		{
			curInfo.stage = vk::ShaderStageFlagBits::eFragment;
			curInfo.module = eachShader.second->As<VulkanShaderBlob>()->mShaderModule;
			curInfo.pName = "PSMain";
			break;
		}
		case RHIShaderType::Geomtry:
		{
			curInfo.stage = vk::ShaderStageFlagBits::eGeometry;
			curInfo.module = eachShader.second->As<VulkanShaderBlob>()->mShaderModule;
			curInfo.pName = "GSMain";
			break;
		case RHIShaderType::Hull:
		{
			curInfo.stage = vk::ShaderStageFlagBits::eTessellationControl;
			curInfo.module = eachShader.second->As<VulkanShaderBlob>()->mShaderModule;
			curInfo.pName = "HSMain";
			break;
		}
		case RHIShaderType::Domin:
		{
			curInfo.stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
			curInfo.module = eachShader.second->As<VulkanShaderBlob>()->mShaderModule;
			curInfo.pName = "DSMain";
			break;
		}
		}
		default:
			break;
		}
		shaderStages.push_back(curInfo);
	}

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	switch (graphPipelineDesc->PrimitiveTopologyType)
	{
	case RHIPrimitiveTopologyType::Triangle:
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		break;
	case RHIPrimitiveTopologyType::Line:
		inputAssembly.topology = vk::PrimitiveTopology::eLineList;
		break;
	default:
		assert(false);
		break;
	}
	inputAssembly.primitiveRestartEnable = false;

	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.depthClampEnable = false;
	rasterizer.rasterizerDiscardEnable = false;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;

	switch (graphPipelineDesc->RasterizerState.CullMode)
	{
	case RHIRasterizerCullMode::BackFace:
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		break;
	case RHIRasterizerCullMode::FrontFace:
		rasterizer.cullMode = vk::CullModeFlagBits::eFront;
		break;
	case RHIRasterizerCullMode::None:
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

	colorBlendAttachment.blendEnable = false;

	vk::PipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 1.0f;
	colorBlending.blendConstants[1] = 1.0f;
	colorBlending.blendConstants[2] = 1.0f;
	colorBlending.blendConstants[3] = 1.0f;


	vk::PipelineDepthStencilStateCreateInfo depthCreateInfo{};
	depthCreateInfo.depthTestEnable = graphPipelineDesc->DepthStencilState.DepthEnable;
	depthCreateInfo.depthWriteEnable = graphPipelineDesc->DepthStencilState.DepthWrite;

	depthCreateInfo.depthCompareOp = Convert(graphPipelineDesc->DepthStencilState.DepthFunc);
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

	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();


	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	std::vector<vk::VertexInputAttributeDescription> inputAttributes;
	uint32_t inputLocationIndex = 0;
	LArray<vk::VertexInputBindingDescription> inputBindings;
	inputBindings.resize(16);
	uint32_t inputVertexBufferNum = 0;
	for (auto& vertexElement : inputLayout.mElements)
	{
		auto& attr = inputAttributes.emplace_back();
		attr.location = static_cast<uint32_t>(vertexElement.mUsage);
		attr.binding = vertexElement.mBufferSlot;
		attr.offset = 0;
		if (vertexElement.mElementType == VertexElementType::Float)
		{
			if (vertexElement.mElementCount == 1)
				attr.format = vk::Format::eR32Sfloat;
			else if (vertexElement.mElementCount == 2)
				attr.format = vk::Format::eR32G32Sfloat;
			else if (vertexElement.mElementCount == 3)
				attr.format = vk::Format::eR32G32B32Sfloat;
			else if (vertexElement.mElementCount == 4)
				attr.format = vk::Format::eR32G32B32A32Sfloat;
		}
		else if (vertexElement.mElementType == VertexElementType::Int)
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
		//inputLocationIndex++;
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

VulkanPipelineStateCompute::VulkanPipelineStateCompute(LSharedPtr<RHIPipelineStateDescBase> psoDesc) : RHIPipelineStateCompute(psoDesc)
{

}

void VulkanPipelineStateCompute::CreateComputePipelineImpl(RHIPipelineStateComputeDesc* computePipelineDesc)
{
	vk::ComputePipelineCreateInfo pipelineInfo{};
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	pipelineInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
	pipelineInfo.stage.module = computePipelineDesc->mShaders[RHIShaderType::Compute]->As<VulkanShaderBlob>()->mShaderModule;
	pipelineInfo.stage.pName = "CSMain";
	pipelineInfo.layout = mBindingSetLayout->As <VulkanBindingSetLayout>()->mPipelineLayout;
	vk::Result   res; 
	std::tie(res, mPipeline) = device.createComputePipeline(VK_NULL_HANDLE, pipelineInfo);
}

}

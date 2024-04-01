#pragma once

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
namespace luna::graphics
{
	RENDER_API CONFIG_IMPLEMENT(LString, Render, RenderDeviceType, "Vulkan");
	luna::graphics::RHIDevice* sGlobelRenderDevice = nullptr;
	RHIRenderQueuePtr GenerateRenderQueue(RHIQueueType queueType)
	{
		RHIRenderQueuePtr curRenderQueue;
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			curRenderQueue = CreateRHIObject<DX12RenderQueue>(queueType);
		else
			curRenderQueue = CreateRHIObject<VulkanRenderQueue>(queueType);
		return curRenderQueue;
	}

	graphics::RHIDevicePtr GenerateRenderDevice()
	{
		graphics::RHIDevicePtr curDeviceValue;
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			curDeviceValue = CreateRHIObject<DX12Device>();
		else
			curDeviceValue = CreateRHIObject<VulkanDevice>();
		sGlobelRenderDevice = curDeviceValue.get();
		sGlobelRenderDevice->InitDeviceData();
		return curDeviceValue;
	}
}

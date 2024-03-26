#pragma once

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
namespace luna::graphics
{
	

	RHIRenderQueuePtr GenerateRenderQueue(RHIQueueType queueType)
	{
		RHIRenderQueuePtr curRenderQueue;
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			curRenderQueue = CreateRHIObject<DX12RenderQueue>(queueType);
		else
			curRenderQueue = CreateRHIObject<VulkanRenderQueue>(queueType);
		return curRenderQueue;
	}

	void GenerateRenderDevice()
	{
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			sGlobelRenderDevice = CreateRHIObject<DX12Device>();
		else
			sGlobelRenderDevice = CreateRHIObject<VulkanDevice>();
		sGlobelRenderDevice->InitDeviceData();
	}
}

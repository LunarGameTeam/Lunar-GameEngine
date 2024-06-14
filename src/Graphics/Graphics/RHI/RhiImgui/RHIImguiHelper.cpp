#include "Graphics/RHI/RhiImgui/RHIImguiHelper.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorPool.h"
#include "Graphics/RHI/DirectX12/DX12View.h"
#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"
#include "Graphics/RHI/Vulkan/VulkanCmdList.h"
#include "Graphics/RHI/Vulkan/VulkanView.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
#include "Core/Foundation/Config.h"

#include "imgui_impl_vulkan.h"

namespace luna::graphics
{
	void RenderImguiInit(
		SDL_Window* window,
		RHIRenderQueue* renderQueue,
		graphics::RHIRenderPass* renderPass,
		RHISinglePoolSingleCmdList* cmdList
	)
	{
		if (Config_RenderDeviceType.GetValue() == "DirectX12") 
		{
			static graphics::Dx12DescriptorSet sDx12FontDescriptor;
			graphics::DX12Device* dx12_device = sGlobelRenderDevice->As<DX12Device>();
			graphics::DX12DescriptorPool* dx12Pool = sGlobelRhiResourceGenerator->GetDefaultDescriptorPool()->As<graphics::DX12DescriptorPool>();
			dx12Pool->SelectSegment(graphics::DescriptorHeapType::CBV_SRV_UAV)->AllocDescriptorSet(1, sDx12FontDescriptor);
			ImGui_ImplSDL2_InitForD3D(window);
			ImGui_ImplDX12_Init(dx12_device->GetDx12Device(), 2,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				dx12_device->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetDeviceHeap(),
				sDx12FontDescriptor.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mCPUHandle,
				sDx12FontDescriptor.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle
			);
		}
		else
		{
			graphics::VulkanDevice* curDevice = sGlobelRenderDevice->As<VulkanDevice>();
			ImGui_ImplSDL2_InitForVulkan(window);
			ImGui_ImplVulkan_InitInfo vulkanInit = {};
			vulkanInit.Instance = curDevice->GetVkInstance();
			vulkanInit.PhysicalDevice = curDevice->GetPhysicalDevice();
			vulkanInit.Device = curDevice->GetVKDevice();
			vulkanInit.Queue = renderQueue->As<graphics::VulkanRenderQueue>()->mQueue;
			vulkanInit.DescriptorPool = sGlobelRhiResourceGenerator->GetDefaultDescriptorPool()->As<graphics::VulkanDescriptorPool>()->mPool;
			vulkanInit.MinImageCount = 3;
			vulkanInit.ImageCount = 3;
			vulkanInit.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			ImGui_ImplVulkan_Init(&vulkanInit, renderPass->As<graphics::VulkanRenderPass>()->mRenderPass);


			cmdList->GetCmdList()->BeginEvent("ImguiFont");
			cmdList->Reset();
			ImGui_ImplVulkan_CreateFontsTexture(cmdList->GetCmdList()->As<graphics::VulkanGraphicCmdList>()->mCommandBuffer);
			cmdList->GetCmdList()->CloseCommondList();
			cmdList->GetCmdList()->EndEvent();

			renderQueue->As<graphics::VulkanRenderQueue>()->ExecuteCommandLists(cmdList->GetCmdList());
			vkDeviceWaitIdle(curDevice->GetVKDevice());
		}
	}

	void ImGuiRhiRenderDrawData(ImDrawData* drawData, RHISinglePoolSingleCmdList* cmdList)
	{
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
		{
			ImGui_ImplDX12_RenderDrawData(drawData, cmdList->GetCmdList()->As<graphics::DX12GraphicCmdList>()->mDxCmdList.Get());
		}
		else
		{
			ImGui_ImplVulkan_RenderDrawData(drawData, cmdList->GetCmdList()->As<graphics::VulkanGraphicCmdList>()->mCommandBuffer);
		}
	}

	void AddImguiRhiTexture(ImguiTexture* imguiTexture, RHIResource* res)
	{
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
		{
			graphics::DX12Device* dx12Device = sGlobelRenderDevice->As<DX12Device>();
			Dx12GpuDescriptorHeap* global = dx12Device->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			imguiTexture->mDX12Pool = global->AllocPool(2);
			Dx12DescriptorSet empty;
			imguiTexture->mDX12Pool->As<DX12GpuDescriptorSegment>()->AllocDescriptorSet(1, empty);
			dx12Device->GetDx12Device()->CopyDescriptorsSimple(
				1,
				empty.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mCPUHandle,
				imguiTexture->mView->As<DX12View>()->GetCpuHandle(),
				D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			imguiTexture->mImg = (void*)(empty.mDescriptorLists[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].mGPUHandle.ptr);
		}
		else
		{
			sGlobelRhiResourceGenerator->GetClampSamper();
			VkDescriptorSet  newSet = ImGui_ImplVulkan_AddTexture(sGlobelRhiResourceGenerator->GetClampSamper().mSampler->As<graphics::VulkanResource>()->mSampler, imguiTexture->mView->As<VulkanView>()->mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			imguiTexture->mImg = newSet;
		}
	}

	void ImGuiRhiGenNewFrame()
	{
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
		{
			ImGui_ImplDX12_NewFrame();
		}
		else
		{
			ImGui_ImplVulkan_NewFrame();
		}
	}
}

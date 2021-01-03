#pragma once
#include "LDirectX12Pipeline.h"
namespace luna
{
	using LDirectXGraphicCommondList = ID3D12GraphicsCommandList1;
	class LDx12GraphicRenderCommondAllocator :public ILunarGraphicRenderCommondAllocator
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directx_commond_allocator;
	public:
		LDx12GraphicRenderCommondAllocator(const LunarGraphicPipeLineType& commond_allocator_type);
		inline ID3D12CommandAllocator* GetAllocator()
		{
			return directx_commond_allocator.Get();
		}
	private:
		LResult InitCommondAllocatorData() override;

	};
	class LDx12GraphicRenderCommondList : public ILunarGraphicRenderCommondList
	{
		Microsoft::WRL::ComPtr<LDirectXGraphicCommondList> directx_commond_list;
	public:
		LDx12GraphicRenderCommondList(const luna::LunarGraphicPipeLineType& commond_list_type);
		inline LDirectXGraphicCommondList* GetCommondList()
		{
			return directx_commond_list.Get();
		}
		virtual void DrawIndexedInstanced(
			const uint32_t& IndexCountPerInstance,
			const uint32_t& InstanceCount,
			const uint32_t& StartIndexLocation,
			const int32_t& BaseVertexLocation,
			const int32_t& StartInstanceLocation) override;
		void SetDrawPrimitiveTopology(const LunaRenderPrimitiveTopology& primitive_topology) override;
		void ResourceBarrier(const LunaResourceUsingState& resource_state_before, const LunaResourceUsingState& resource_state_target) override;
	private:
		LResult InitCommondListData(LSharedObject* allocator, LSharedObject* pipeline) override;
		LResult ResetCommondListData(LSharedObject* allocator, LSharedObject* pipeline) override;
	};

}

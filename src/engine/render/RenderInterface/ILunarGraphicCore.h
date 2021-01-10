#pragma once
#include "core/core_module.h"
#include "core/file/path.h"
#include "core/misc/FastId.h"
#include "core/reflection/json_utility.h"
#include"core/reflection/reflection.h"
#include"core/Common/MultiThreadMap.h"
#include"core/asset/lasset.h"
#include"core/memory/ptr.h"
#include "core/Common/LinerGrowMap.h"
#define LunarObjectID uint32_t
#define LunarResourceSize size_t

#define AddNewGraphicDeviceFunc(FuncName) \
	virtual LBasicAsset* Create##FuncName##FromFile(const LString &file_name,const Json::Value &root_value,const bool if_check_repeat) = 0;\
	virtual LBasicAsset* Create##FuncName##FromMemory(const void* ##FuncName##_desc_message,const bool if_check_repeat) = 0;

#define AddNewGraphicDeviceFuncOverride(FuncName) \
	LBasicAsset* Create##FuncName##FromFile(const LString &file_name,const Json::Value &root_value,const bool if_check_repeat) override;\
	LBasicAsset* Create##FuncName##FromMemory(const void* ##FuncName##_desc_message,const bool if_check_repeat) override;

//fenceid
LInitNewFastIdClass(LGraphicFenceId, size_t);
namespace luna
{
	enum LunaRenderCommondListState
	{
		COMMOND_LIST_STATE_NULL = 0,
		COMMOND_LIST_STATE_PREPARING,
		COMMOND_LIST_STATE_DEALING,
		COMMOND_LIST_STATE_FINISHED
	};
	enum LunaResourceUsingState
	{
		RESOURCE_USING_STATE_UNIFORM_BUFFER = 0,
		RESOURCE_USING_STATE_VERTEX_BUFFER,
		RESOURCE_USING_STATE_INDEX_BUFFER,
		RESOURCE_USING_STATE_RENDER_TARGET,
		RESOURCE_USING_STATE_DEPTH_WRITE,
		RESOURCE_USING_STATE_DEPTH_READE,
		RESOURCE_USING_STATE_SHADER_RESOURCE_NON_PIXEL,
		RESOURCE_USING_STATE_SHADER_RESOURCE_PIXEL,
		RESOURCE_USING_STATE_SHADER_STREAM_OUT,
		RESOURCE_USING_STATE_SHADER_INDIRECT_ARGUMENT,
		RESOURCE_USING_STATE_SHADER_COPY_DEST,
		RESOURCE_USING_STATE_SHADER_COPY_SOURCE,
		RESOURCE_USING_STATE_SHADER_RESOLVE_DEST,
		RESOURCE_USING_STATE_SHADER_RESOLVE_SOURCE,
		RESOURCE_USING_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_SOURCE,
		RESOURCE_USING_STATE_RAYTRACING_GENERIC_READ,
		RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_PRESENT,
		RESOURCE_USING_STATE_RAYTRACING_SHADING_RATE_PREDICATION
	};
	enum LunaRenderPrimitiveTopology
	{
		PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
		PRIMITIVE_TOPOLOGY_POINTLIST,
		PRIMITIVE_TOPOLOGY_LINELIST,
		PRIMITIVE_TOPOLOGY_LINESTRIP,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
		PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
		PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
	};
	enum class LunarGraphicResourceType
	{
		GRAPHIC_RESOURCE_UNDEFINE = 0,
		GRAPHIC_RESOURCE_SHADER,
		GRAPHIC_RESOURCE_ROOT_SIGNATURE,
		GRAPHIC_RESOURCE_PIPELINE,
		GRAPHIC_RESOURCE_ENGINE_RESOURCE,
		GRAPHIC_RESOURCE_ENGINE_RESOURCE_HEAP,
		GRAPHIC_RESOURCE_ENGINE_DESCRIPTOR,
		GRAPHIC_RESOURCE_ENGINE_DESCRIPTOR_HEAP
	};
	enum LunarGraphicPipeLineType
	{
		PIPELINE_GRAPHIC3D = 0,
		PIPELINE_COMPUTE,
		PIPELINE_COPY,
		PIPELINE_RAYTRACING
	};
	enum class LunarGraphicShaderType
	{
		SHADER_VERTEX = 0,
		SHADER_PIXEL,
		SHADER_GEOMETRY,
		SHADER_COMPUTE,
		SHADER_HULL,
		SHADER_DOMIN
	};
	enum LunarGraphicDescriptorType
	{
		DESCRIPTOR_UNIFORM_INVALID =0,
		DESCRIPTOR_UNIFORM_BUFFER,
		DESCRIPTOR_SHADER_RESOURCE,
		DESCRIPTOR_UNORDERED_ACCESS,
		DESCRIPTOR_RENDER_TARGET,
		DESCRIPTOR_DEPTH_STENCIL
	};
	//命令分配器
	class ILunarGraphicRenderCommondAllocator :public LObject
	{
		LunarGraphicPipeLineType m_commond_allocator_type;
	public:
		ILunarGraphicRenderCommondAllocator(const LunarGraphicPipeLineType& commond_allocator_type);
		luna::LResult Create();
		const LunarGraphicPipeLineType& GetPipelineType()
		{
			return m_commond_allocator_type;
		}
	private:
		virtual LResult InitCommondAllocatorData() = 0;
	};
	//commond list, 用于录制渲染命令
	class ILunarGraphicRenderCommondList : public LObject
	{
		LunaRenderCommondListState m_commond_list_work_state;
		LunarGraphicPipeLineType m_commond_list_type;
	public:
		ILunarGraphicRenderCommondList(const LunarGraphicPipeLineType&commond_list_type);
		luna::LResult Create(LSharedObject* allocator, LSharedObject* pipeline);
		const LunarGraphicPipeLineType& GetPipelineType()
		{
			return m_commond_list_type;
		}
		luna::LResult ResetCommondList(LSharedObject* allocator, LSharedObject* pipeline);
		/*virtual void SetVertexBufferView();
		virtual void SetIndexBufferView();
		virtual void OMSetRenderTargets();*/
		virtual void DrawIndexedInstanced(
			const uint32_t& IndexCountPerInstance,
			const uint32_t& InstanceCount,
			const uint32_t& StartIndexLocation,
			const int32_t& BaseVertexLocation,
			const int32_t& StartInstanceLocation) = 0;
		virtual void SetDrawPrimitiveTopology(const LunaRenderPrimitiveTopology& primitive_topology) = 0;
		virtual void ResourceBarrier(const LunaResourceUsingState& resource_state_before, const LunaResourceUsingState& resource_state_target) = 0;
	private:
		virtual LResult InitCommondListData(LSharedObject* allocator, LSharedObject* pipeline) = 0;
		virtual LResult ResetCommondListData(LSharedObject* allocator, LSharedObject* pipeline) = 0;
	};
	//fence,用于检测gpu端命令是否运行完毕
	class ILunarGraphicRenderFence : public LObject
	{
	protected:
		size_t fence_value_self_add;
	public:
		ILunarGraphicRenderFence();
		virtual size_t SetFence() = 0;
		virtual bool CheckIfFenceFinished(const size_t &fence_value_check) = 0;
		virtual void WaitForFence(const size_t& fence_value_check) = 0;
		luna::LResult Create();
	private:
		virtual LResult InitGraphicRenderFence() = 0;
	};
	//交换链，用于绑定渲染窗口
	struct LWindowRenderDesc
	{
		uint32_t window_width;
		uint32_t window_height;
		int32_t max_frame_number;
	};
	class ILunarGraphicRenderSwapChain : public LObject
	{
	protected:
		LWindowRenderDesc m_render_window_desc;
	public:
		ILunarGraphicRenderSwapChain(const void* trarget_window,const LWindowRenderDesc &trarget_window_desc);
		inline const LWindowRenderDesc &GetDesc()
		{
			return m_render_window_desc;
		}
		LResult Create();
		LResult ResetSwapChain(const LWindowRenderDesc &window_width_in);
		virtual void PresentFrame() = 0;
		virtual uint32_t GetNowFrameID() = 0;
	private:
		virtual LResult InitGraphicSwapChain() = 0;
		virtual LResult ResetSwapChainData(const LWindowRenderDesc& window_width_in) = 0;
	};
	//描述符的创建绑定格式
	class ILunarGraphicDescriptorCompleteDesc : public LObject
	{
		LunarGraphicDescriptorType m_descriptor_type;
	public:
		ILunarGraphicDescriptorCompleteDesc(const LunarGraphicDescriptorType& descriptor_type)
		{
			m_descriptor_type = descriptor_type;
		};
		inline LunarGraphicDescriptorType GetDesc()
		{
			return m_descriptor_type;
		}
	};
	//描述符，用于绑定资源
	class ILunarGraphicRenderDescriptor : public LObject
	{
	protected:
		LunarGraphicDescriptorType m_descriptor_type;
		size_t m_descriptor_offset;
		size_t m_descriptor_size;
		LPtrBasic m_descriptor_heap;
	public:
		ILunarGraphicRenderDescriptor(const LunarGraphicDescriptorType& descriptor_type, const size_t &m_descriptor_offset,const size_t &descriptor_size);
		luna::LResult Create(LObject* descriptor_heap);
		virtual void BindBufferToDescriptor(const LVector<LBasicAsset*>& buffer_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& buffer_descriptor_desc) = 0;
		virtual void BindTextureToDescriptor(const LVector<LBasicAsset*>& texture_data, const LVector<ILunarGraphicDescriptorCompleteDesc*>& tex_descriptor_desc) = 0;
	private:
		virtual LResult InitGraphicRenderDescriptor(LObject* descriptor_heap) = 0;
	};
	//图形设备单例，用于处理图形资源的分配和管理
	class ILunarGraphicDeviceCore
	{
	protected:
		ILunarGraphicDeviceCore();
	public:
		static ILunarGraphicDeviceCore* _graphic_device_instance;
		static luna::LResult SingleCreate();
		static ILunarGraphicDeviceCore* GetInstance()
		{
			return _graphic_device_instance;
		}
		virtual ~ILunarGraphicDeviceCore();
		virtual void* GetVirtualDevice() = 0;
		virtual void* GetVirtualGraphicInterface() = 0;
		//图形管线
		AddNewGraphicDeviceFunc(Shader);
		AddNewGraphicDeviceFunc(RootSignature);
		AddNewGraphicDeviceFunc(Pipeline);
		//资源及资源堆
		AddNewGraphicDeviceFunc(BufferResource);
		AddNewGraphicDeviceFunc(TextureResource);
		AddNewGraphicDeviceFunc(ResourceHeap);
		//描述符堆
		AddNewGraphicDeviceFunc(DescriptorHeap);
		virtual uint8_t* GetPointerFromSharedMemory(LSharedObject* dynamic_buffer_pointer) = 0;
		virtual LBasicAsset * CreateUniforBuffer(const size_t &uniform_buffer_size) = 0;
		virtual ILunarGraphicRenderCommondList* CreateCommondList(
			LSharedObject* allocator,
			LSharedObject* pipeline, 
			const LunarGraphicPipeLineType& pipeline_type) = 0;
		virtual ILunarGraphicRenderCommondAllocator* CreateCommondAllocator(const LunarGraphicPipeLineType& pipeline_type) = 0;
		virtual ILunarGraphicRenderFence* CreateFence() = 0;
		virtual ILunarGraphicRenderDescriptor* CreateDescriptor(const LunarGraphicDescriptorType& descriptor_type, const size_t& m_descriptor_offset, const size_t& descriptor_size, LObject* descriptor_heap) = 0;
	protected:
		LResult Create();
	private:
		virtual LResult InitDeviceData() = 0;
	};
	
	//图形队列单例,用于提交渲染命令
	class ILunarGraphicRenderQueueCore
	{
	protected:
		ILunarGraphicRenderQueueCore();
	public:
		static ILunarGraphicRenderQueueCore* _graphic_render_queue_instance;
		static luna::LResult SingleCreate();
		static ILunarGraphicRenderQueueCore* GetInstance()
		{
			return _graphic_render_queue_instance;
		}
		virtual void ExecuteCommandLists(const int32_t& commond_list_num, LVector<ILunarGraphicRenderCommondList*> commond_list_array) = 0;
		virtual ILunarGraphicRenderSwapChain* CreateSwapChain(
			const void* trarget_window,
			const LWindowRenderDesc& trarget_window_desc,
			const LunarGraphicPipeLineType& pipeline_type
		) = 0;
	protected:
		LResult Create();
	private:
		virtual LResult InitRenderQueueData() = 0;
	};
}



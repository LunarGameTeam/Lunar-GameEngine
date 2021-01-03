#pragma once
#define AnimationSize
#include"PancyBufferDx12.h"
#include"PancyTextureDx12.h"
#define threadBlockSize 128
namespace luna
{
	//普通的绑定描述符
	struct CommonDescriptorPointer
	{
		//是否使用双缓冲
		bool if_multi_buffer;
		//描述符页的起始地址
		std::vector<LunarObjectID> descriptor_offset;
		//描述符类型
		D3D12_DESCRIPTOR_HEAP_TYPE descriptor_type;
		//描述符对应的资源指针
		std::vector<VirtualResourcePointer> resource_data;
		CommonDescriptorPointer()
		{
			//初始化普通描述符指针
			if_multi_buffer = false;
			descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		}
	};
	//解绑定的描述符页
	struct BindlessResourceViewPointer
	{
		bool if_dynamic = false;
		//描述符页的起始地址
		LunarObjectID resource_view_offset;
		//描述符页所包含的描述符数量
		LunarObjectID resource_view_num;
		//每一个描述符的格式数据
		std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> SRV_desc;
		//每一个描述符所指向的资源
		std::vector<VirtualResourcePointer> describe_memory_data;
		BindlessResourceViewPointer()
		{
			resource_view_offset = 0;
			resource_view_num = 0;
		}
	};
	//描述符段
	class BindlessResourceViewSegmental
	{
		ID3D12DescriptorHeap *descriptor_heap_data;//描述符堆的真实地址
		LunarObjectID per_descriptor_size;       //每个描述符的大小
		LunarObjectID segmental_offset_position; //当前描述符段在全局的偏移
		LunarObjectID max_descriptor_num;        //当前描述符段最大容纳的描述符数量
		LunarObjectID now_pointer_offset;        //当前描述符段已使用数据的偏移
		LunarObjectID now_pointer_refresh;       //当前描述符段如果需要一次整理操作，其合理的起始整理位置
		LunarObjectID now_descriptor_pack_id_self_add;//当前描述符段为每个描述符页分配的ID的自增长号码
		std::queue<LunarObjectID> now_descriptor_pack_id_reuse;//之前被释放掉，现在可以重新使用的描述符ID

		std::unordered_map<LunarResourceID, BindlessResourceViewPointer> descriptor_data;//每个被分配出来的描述符页
	public:
		BindlessResourceViewSegmental(
			const LunarObjectID &max_descriptor_num,
			const LunarObjectID &segmental_offset_position_in,
			const LunarObjectID &per_descriptor_size_in,
			const ComPtr<ID3D12DescriptorHeap> descriptor_heap_data_in
		);
		inline LunarObjectID GetEmptyDescriptorNum()
		{
			return max_descriptor_num - now_pointer_offset;
		}
		//从描述符段里开辟一组bindless描述符页
		luna::LResult BuildBindlessShaderResourceViewPack(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> &SRV_desc,
			const std::vector<VirtualResourcePointer> &describe_memory_data,
			const LunarObjectID &SRV_pack_size,
			LunarObjectID &SRV_pack_id
		);
		//从起始位置创建一个虚拟的srv打包，只创建，不填充(todo:之后要移除这种方法)
		luna::LResult BuildBindlessShaderResourceViewFromBegin(LunarObjectID& SRV_pack_id);
		luna::LResult RebindBindlessShaderResourceViewFromBegin(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC>& SRV_desc,
			const std::vector<VirtualResourcePointer>& describe_memory_data
			);
		//从描述符段里删除一组bindless描述符页
		luna::LResult DeleteBindlessShaderResourceViewPack(const LunarObjectID &SRV_pack_id);
		//为描述符段进行一次描述符碎片的整理操作
		luna::LResult RefreshBindlessShaderResourceViewPack();
		//从描述符段里删除一组bindless描述符页并执行一次整理操作
		luna::LResult DeleteBindlessShaderResourceViewPackAndRefresh(const LunarObjectID &SRV_pack_id);
		//获取一个描述符页的基础偏移
		const BindlessResourceViewPointer &GetDescriptorPageOffset(const LunarObjectID &descriptor_page_id);
		//将描述符段虚拟填满(用于长期控制该段的所有数据)
		inline void FullSetAllDescriptor() 
		{
			now_pointer_offset = max_descriptor_num;
		}
		//将描述符段虚拟填满(用于释放长期控制该段的所有数据)
		inline void EmptySetAllDescriptor()
		{
			now_pointer_offset = 0;
		}
	private:
		//根据描述符页的指针信息，在描述符堆开辟描述符
		luna::LResult BuildShaderResourceView(BindlessResourceViewPointer &resource_view_pointer);
	};

	//解绑定描述符页的id号
	struct BindlessDescriptorID
	{
		//全局ID
		LunarObjectID bindless_id;
		//空余资源的数量
		LunarObjectID empty_resource_size;
		//重载小于运算符
		bool operator<(const BindlessDescriptorID& other)  const;
	};
	//解绑定描述符段的id号
	struct BindlessResourceViewID
	{
		LunarObjectID segmental_id;
		LunarObjectID page_id;
	};

	//用于处理描述符堆
	class PancyDescriptorHeap
	{
		D3D12_DESCRIPTOR_HEAP_DESC descriptor_desc;                                        //描述符堆的格式
		std::string descriptor_heap_name;                                                  //描述符堆的名称
		LunarObjectID per_descriptor_size;                                               //每个描述符的大小
		ComPtr<ID3D12DescriptorHeap> descriptor_heap_data;                                 //描述符堆的真实数据

		//管理三种不同的描述符(全局描述符，bindless描述符以及普通的绑定描述符)
		std::unordered_map<std::string, LunarObjectID> descriptor_globel_map;//描述符堆的全局描述符的数据集合

		LunarObjectID bind_descriptor_num;                                               //描述符堆最大支持的绑定描述符数量
		std::queue<LunarObjectID> bind_descriptor_offset_reuse;                          //绑定描述符的回收利用的ID号
		std::unordered_map<LunarObjectID, CommonDescriptorPointer> descriptor_bind_map;  //描述符堆的绑定描述符的数据集合

		LunarObjectID bindless_descriptor_num;                                                 //描述符堆最大支持的bindless描述符数量
		LunarObjectID per_segmental_size;                                                      //每一个描述符段的大小
		std::unordered_map<LunarObjectID, BindlessDescriptorID> bindless_descriptor_id_map;    //描述符堆的所有bindless描述符段的id集合
		std::map<BindlessDescriptorID, BindlessResourceViewSegmental*> descriptor_segmental_map; //描述符堆的所有bindless描述符段的数据集合

	public:
		PancyDescriptorHeap();
		~PancyDescriptorHeap();
		luna::LResult Create(
			const D3D12_DESCRIPTOR_HEAP_DESC &descriptor_heap_desc,
			const std::string &descriptor_heap_name_in,
			const LunarObjectID &bind_descriptor_num_in,
			const LunarObjectID &bindless_descriptor_num_in,
			const LunarObjectID &per_segmental_size_in
		);
		inline luna::LResult GetDescriptorHeapData(ID3D12DescriptorHeap **descriptor_heap_out)
		{
			*descriptor_heap_out = descriptor_heap_data.Get();
			return luna::g_Succeed;
		}
		//创建全局描述符
		luna::LResult BuildGlobelDescriptor(
			const std::string &globel_name,
			const std::vector<BasicDescriptorDesc> &SRV_desc,
			std::vector <VirtualResourcePointer>& memory_data,
			const bool if_build_multi_buffer
		);
		luna::LResult DeleteGlobelDescriptor(const std::string &globel_name);
		luna::LResult GetGlobelDesciptorID(const std::string &globel_name, LunarObjectID &descriptor_id);
		luna::LResult BindGlobelDescriptor(
			const std::string &globel_name,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
		//创建私有的绑定描述符
		luna::LResult BuildBindDescriptor(
			const std::vector<BasicDescriptorDesc> &descriptor_desc,
			std::vector<VirtualResourcePointer>& memory_data,
			const bool if_build_multi_buffer,
			LunarObjectID &descriptor_id
		);
		luna::LResult DeleteBindDescriptor(const LunarObjectID &descriptor_id);
		luna::LResult BindCommonDescriptor(
			const LunarObjectID &descriptor_id,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
		luna::LResult GetCommonDescriptorCpuOffset(const LunarObjectID &descriptor_id, CD3DX12_CPU_DESCRIPTOR_HANDLE &Cpu_Handle);
		//创建私有的bindless描述符页
		luna::LResult BuildBindlessShaderResourceViewPage(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> &SRV_desc,
			const std::vector<VirtualResourcePointer> &describe_memory_data,
			const LunarObjectID &SRV_pack_size,
			BindlessResourceViewID &descriptor_id
		);
		//获取指定大小的bindless描述符段（消耗较大，避免频繁使用todo:之后需要删除这个逻辑，使用bindless描述符跨段以及独立的动态描述符区域）
		luna::LResult LockDescriptorSegmental(
			const LunarObjectID& lock_size,
			std::vector<BindlessDescriptorID> &descriptor_segmental_list,
			BindlessResourceViewID& descriptor_id
		);
		luna::LResult RebindSahderResourceViewDynamic(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC>& SRV_desc,
			const std::vector<VirtualResourcePointer>& describe_memory_data,
			const BindlessResourceViewID& descriptor_id
		);
		//删除私有的bindless描述符页(可以指定是否删除完毕后对页碎片进行整理)
		luna::LResult DeleteBindlessShaderResourceViewPage(
			const BindlessResourceViewID &descriptor_id,
			bool is_refresh_segmental = true
		);
		//整理所有的描述符段，消耗较大，在切换地图资源的时候配合不整理碎片的删除使用
		luna::LResult RefreshBindlessShaderResourceViewSegmental();
		//将解绑定描述符绑定至rootsignature
		luna::LResult BindBindlessDescriptor(
			const BindlessResourceViewID &descriptor_id,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
	private:
		//重新刷新解绑定资源描述符段的大小，当描述符段增删查改的时候被调用
		luna::LResult RefreshBindlessResourcesegmentalSize(const LunarObjectID &resourc_id);
		//预创建描述符数据
		luna::LResult PreBuildBindDescriptor(
			const D3D12_DESCRIPTOR_HEAP_TYPE &descriptor_type,
			const bool if_build_multi_buffer,
			std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> &descriptor_cpu_handle,
			CommonDescriptorPointer &new_descriptor_data
		);
		//获取描述符堆格式
		D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapTypeOfDescriptor(const BasicDescriptorDesc &descriptor_desc);
	};

	//绑定描述符的虚拟指针
	struct BindDescriptorPointer
	{
		//描述符堆ID
		LunarResourceID descriptor_heap_id;
		//描述符ID
		LunarObjectID descriptor_id;
	};
	//解绑定描述符的虚拟指针
	struct BindlessDescriptorPointer
	{
		//描述符堆ID
		LunarResourceID descriptor_heap_id;
		//描述符ID
		BindlessResourceViewID descriptor_pack_id;
	};
	//描述符的json反射
	struct PancyDescriptorHeapDesc 
	{
		D3D12_DESCRIPTOR_HEAP_DESC directx_descriptor;
		LunarObjectID bind_descriptor_num;
		LunarObjectID bindless_descriptor_num;
		LunarObjectID per_segmental_num;
	};
	INIT_REFLECTION_CLASS(PancyDescriptorHeapDesc,
		reflect_data.directx_descriptor.Type,
		reflect_data.directx_descriptor.NumDescriptors,
		reflect_data.directx_descriptor.Flags,
		reflect_data.directx_descriptor.NodeMask,
		reflect_data.bind_descriptor_num,
		reflect_data.bindless_descriptor_num,
		reflect_data.per_segmental_num
	)
	//动态描述符分配器，只用于独立系统每帧创建描述符
	class PancyDescriptorHeapDynamic 
	{
		LunarResourceID common_descriptor_heap_shader_resource; //虚拟数据对应真实的描述符堆id
		std::vector<BindlessDescriptorID> descriptor_segmental_list;
		BindlessDescriptorPointer descriptor_id;
	public:
		PancyDescriptorHeapDynamic();
		luna::LResult Create(const int32_t &max_descriptor_num);
		luna::LResult BuildShaderResourceViewFromHead(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC>& SRV_desc,
			const std::vector<VirtualResourcePointer>& describe_memory_data
		);
		inline BindlessDescriptorPointer GetDescriptorID()
		{
			return descriptor_id;
		};
	};
	
	//用于管理所有的描述符堆
	class PancyDescriptorHeapControl
	{
		LunarResourceID descriptor_heap_id_self_add;
		std::queue<LunarResourceID> descriptor_heap_id_reuse;
		LunarResourceID common_descriptor_heap_shader_resource;
		LunarResourceID common_descriptor_heap_render_target;
		LunarResourceID common_descriptor_heap_depth_stencil;
		std::unordered_map<LunarResourceID, PancyDescriptorHeap*> descriptor_heap_map;
		PancyDescriptorHeapControl();
	public:
		~PancyDescriptorHeapControl();

		static PancyDescriptorHeapControl* GetInstance()
		{
			static PancyDescriptorHeapControl* this_instance;
			if (this_instance == NULL)
			{
				this_instance = new PancyDescriptorHeapControl();
			}
			return this_instance;
		}
		//获取基础的描述符堆
		luna::LResult GetBasicDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE &descriptor_desc, ID3D12DescriptorHeap **descriptor_heap_out);
		//全局描述符
		luna::LResult BuildCommonGlobelDescriptor(
			const std::string &globel_srv_name,
			const std::vector<BasicDescriptorDesc> &now_descriptor_desc_in,
			std::vector<VirtualResourcePointer>& memory_data,
			const bool if_build_multi_buffer
		);
		luna::LResult GetCommonGlobelDescriptorID(
			PancyDescriptorType basic_descriptor_type,
			const std::string &globel_srv_name,
			BindDescriptorPointer &descriptor_id
		);
		luna::LResult BindCommonGlobelDescriptor(
			PancyDescriptorType basic_descriptor_type,
			const std::string &globel_name,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
		luna::LResult BindCommonRenderTargetUncontiguous(
			const std::vector<LunarObjectID> rendertarget_list,
			const LunarObjectID depthstencil_descriptor,
			PancyRenderCommandList *m_commandList,
			const bool &if_have_rendertarget = true,
			const bool &if_have_depthstencil = true
		);
		//todo:目前由于RTV在交换链中取出来无法管理，暂时给出取出depthstencil的方法用于测试，在资源管理器重做后要删除
		luna::LResult GetCommonDepthStencilBufferOffset(
			const LunarObjectID depthstencil_descriptor,
			CD3DX12_CPU_DESCRIPTOR_HANDLE &dsvHandle
		);
		//todo:目前暂时将描述符堆数据锁住用于动态描述符处理
		luna::LResult LockDescriptorSegmental(
			const LunarObjectID& lock_size,
			std::vector<BindlessDescriptorID>& descriptor_segmental_list,
			BindlessDescriptorPointer& descriptor_id
		);
		luna::LResult RebindSahderResourceViewDynamicCommon(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC>& SRV_desc,
			const std::vector<VirtualResourcePointer>& describe_memory_data,
			const BindlessDescriptorPointer& descriptor_id
		);
		//绑定描述符
		luna::LResult BuildCommonDescriptor(
			const std::vector<BasicDescriptorDesc> &now_descriptor_desc_in,
			std::vector<VirtualResourcePointer>& memory_data,
			const bool if_build_multi_buffer,
			BindDescriptorPointer &descriptor_id
		);
		luna::LResult BindCommonDescriptor(
			const BindDescriptorPointer &descriptor_id,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
		//解绑定描述符
		luna::LResult BuildCommonBindlessShaderResourceView(
			const std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> &SRV_desc,
			const std::vector<VirtualResourcePointer> &describe_memory_data,
			const LunarObjectID &SRV_pack_size,
			BindlessDescriptorPointer &descriptor_id
		);
		luna::LResult BindBindlessDescriptor(
			const BindlessDescriptorPointer &descriptor_id,
			const D3D12_COMMAND_LIST_TYPE &render_param_type,
			PancyRenderCommandList *m_commandList,
			const LunarObjectID &root_signature_offset
		);
		luna::LResult ClearRenderTarget();
		//添加与删除一个描述符堆
		luna::LResult BuildNewDescriptorHeapFromJson(const std::string &json_name, const Json::Value &root_value, LunarResourceID &descriptor_heap_id);
		luna::LResult BuildNewDescriptorHeapFromJson(const std::string &json_file_name, LunarResourceID &descriptor_heap_id);
		luna::LResult DeleteDescriptorHeap(const LunarResourceID &descriptor_heap_id);
	private:
		LunarResourceID GetCommonDescriptorHeapID(const BasicDescriptorDesc &descriptor_desc);
		LunarResourceID GetCommonDescriptorHeapID(const PancyDescriptorType &descriptor_type);
	};

	

}

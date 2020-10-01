#pragma once
#include"PancyMemoryBasic.h"
#include"core\reflection\PancyResourceJsonReflect.h"
namespace LunarEngine
{
	//upload缓冲区资源块
	struct UploadResourceBlock
	{
		//存储开辟空间前后的指针位置，由于并非每次开辟都是首尾相连，所以开辟前的位置+存储区的大小并不一定等于开辟后的位置
		LunarResourceSize pointer_before_alloc;
		LunarResourceSize pointer_after_alloc;
		ResourceBlockGpu dynamic_buffer_resource;
		ResourceBlockGpu* static_gpu_resource;
		UploadResourceBlock(
			const LunarResourceSize& pointer_before_alloc_in,
			const LunarResourceSize& pointer_after_alloc_in,
			const uint64_t& memory_size_in,
			ComPtr<ID3D12Resource> resource_data_in,
			const D3D12_HEAP_TYPE& resource_usage_in,
			const D3D12_RESOURCE_STATES& resource_state,
			ResourceBlockGpu* static_gpu_resource_input
		) :dynamic_buffer_resource(
			memory_size_in,
			resource_data_in,
			resource_usage_in,
			resource_state
		)
		{
			pointer_before_alloc = pointer_before_alloc_in;
			pointer_after_alloc = pointer_after_alloc_in;
			static_gpu_resource = static_gpu_resource_input;
		}
	};
	class PancyDynamicRingBuffer
	{
		ComPtr<ID3D12Heap> ringbuffer_heap_data;
		LunarResourceSize buffer_size;
		LunarResourceSize pointer_head_offset;
		LunarResourceSize pointer_tail_offset;
		std::queue<UploadResourceBlock*> ResourceUploadingMap;
		PancyDynamicRingBuffer();
	public:
		static PancyDynamicRingBuffer* GetInstance()
		{
			static PancyDynamicRingBuffer* this_instance;
			if (this_instance == NULL)
			{
				this_instance = new PancyDynamicRingBuffer();
			}
			return this_instance;
		}
		~PancyDynamicRingBuffer();
		//拷贝数据到显存
		LunarEngine::LResult CopyDataToGpu(
			PancyRenderCommandList* commandlist,
			void* data_pointer,
			const LunarResourceSize& data_size_in,
			ResourceBlockGpu& gpu_resource_pointer
		);
		LunarEngine::LResult CopyDataToGpu(
			PancyRenderCommandList* commandlist,
			std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
			UINT64* pRowSizesInBytes,
			UINT* pNumRows,
			const LunarResourceSize& data_size,
			ResourceBlockGpu& gpu_resource_pointer
		);
	private:
		LunarEngine::LResult AllocNewDynamicData(
			LunarResourceSize data_size,
			ResourceBlockGpu& gpu_resource_pointer,
			UploadResourceBlock** new_block
		);
		//加载初始化信息
		LunarEngine::LResult LoadInitData();
		//刷新老的缓冲区，释放不需要的资源所占的空间
		LunarEngine::LResult RefreshOldDynamicData();
	};
	//虚拟资源
	class PancyBasicVirtualResource
	{
	protected:
		bool if_could_reload;//资源是否允许重复加载
		std::string resource_type_name;
	protected:
		std::string resource_name;
		std::atomic<LunarObjectID> reference_count;
	public:
		PancyBasicVirtualResource(const bool& if_could_reload_in);
		virtual ~PancyBasicVirtualResource();
		//从json文件中加载资源
		LunarEngine::LResult Create(const std::string& resource_name_in);
		//从json内存中加载资源
		LunarEngine::LResult Create(const std::string& resource_name_in, const Json::Value& root_value_in);
		//直接从结构体加载资源
		LunarEngine::LResult Create(const std::string& resource_name_in, void* resource_data, const std::string& resource_type, const LunarResourceSize& resource_size);
		inline const std::string& GetResourceTypeName() const
		{
			return resource_type_name;
		}
		//修改引用计数
		void AddReference();
		void DeleteReference();
		inline int32_t GetReferenceCount()
		{
			return reference_count;
		}
		inline std::string GetResourceName()
		{
			return resource_name;
		}
		//检测当前的资源是否已经被载入GPU
		virtual bool CheckIfResourceLoadFinish() = 0;
	private:
		//注册并加载资源
		virtual LunarEngine::LResult InitResourceJson(const std::string& resource_name_in, const Json::Value& root_value_in) = 0;
		//从内存中加载资源
		virtual LunarEngine::LResult InitResourceMemory(void* resource_data, const std::string& resource_type, const LunarResourceSize& resource_size) = 0;
		//直接从文件中加载资源（非json文件）
		virtual LunarEngine::LResult InitResourceDirect(const std::string& file_name) = 0;
	};
	template<typename ResourceDescStruct>
	class PancyCommonVirtualResource : public PancyBasicVirtualResource
	{
		ResourceDescStruct resource_desc;
	public:
		PancyCommonVirtualResource(const bool& if_could_reload_in);
		inline ResourceDescStruct& GetResourceDesc()
		{
			return resource_desc;
		};
		virtual ~PancyCommonVirtualResource();
	private:
		//从json类中加载资源
		LunarEngine::LResult InitResourceJson(const std::string& resource_name_in, const Json::Value& root_value_in) override;
		//从内存中加载资源
		LunarEngine::LResult InitResourceMemory(void* resource_data, const std::string& resource_type, const LunarResourceSize& resource_size) override;
		//直接从文件中加载资源（非json文件）
		LunarEngine::LResult InitResourceDirect(const std::string& file_name) override;
		//根据资源格式创建资源数据
		virtual LunarEngine::LResult LoadResoureDataByDesc(const ResourceDescStruct& ResourceDescStruct) = 0;
		//根据其他文件读取资源，并获取资源格式
		virtual LunarEngine::LResult LoadResoureDataByOtherFile(const std::string& file_name, ResourceDescStruct& resource_desc);
	};
	template<typename ResourceDescStruct>
	PancyCommonVirtualResource<ResourceDescStruct>::PancyCommonVirtualResource(const bool& if_could_reload_in) : PancyBasicVirtualResource(if_could_reload_in)
	{
	}
	template<typename ResourceDescStruct>
	PancyCommonVirtualResource<ResourceDescStruct>::~PancyCommonVirtualResource()
	{
	}
	template<typename ResourceDescStruct>
	LunarEngine::LResult PancyCommonVirtualResource<ResourceDescStruct>::InitResourceJson(const std::string& resource_name_in, const Json::Value& root_value_in)
	{
		LunarEngine::LResult check_error;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonMemory(resource_desc, root_value_in, resource_name_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = LoadResoureDataByDesc(resource_desc);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename ResourceDescStruct>
	LunarEngine::LResult PancyCommonVirtualResource<ResourceDescStruct>::InitResourceMemory(void* resource_data, const std::string& resource_type, const LunarResourceSize& resource_size)
	{
		//进行数据类型检查，检测成功后拷贝数据
		if ((typeid(ResourceDescStruct*).name() != resource_type) || (resource_size != sizeof(ResourceDescStruct)))
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(0, "class type dismatch: " + resource_type + " haven't init to reflect class", error_message);

			return error_message;
		}
		resource_desc = *reinterpret_cast<ResourceDescStruct*>(resource_data);
		auto check_error = LoadResoureDataByDesc(resource_desc);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename ResourceDescStruct>
	LunarEngine::LResult PancyCommonVirtualResource<ResourceDescStruct>::InitResourceDirect(const std::string& file_name)
	{
		auto check_error = LoadResoureDataByOtherFile(file_name, resource_desc);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename ResourceDescStruct>
	LunarEngine::LResult PancyCommonVirtualResource<ResourceDescStruct>::LoadResoureDataByOtherFile(const std::string& file_name, ResourceDescStruct& resource_desc)
	{
		//默认情况下，不处理任何非json文件的加载
		LunarEngine::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not parse file: " + file_name, error_message);

		return error_message;
	}
	//虚拟资源的模拟智能指针
	class VirtualResourcePointer
	{
		bool if_NULL;
		LunarObjectID resource_id;
		PancyBasicVirtualResource* data_pointer = NULL;
	public:
		VirtualResourcePointer();
		VirtualResourcePointer(const LunarObjectID& resource_id_in);
		VirtualResourcePointer(const VirtualResourcePointer& copy_data);
		~VirtualResourcePointer();
		VirtualResourcePointer& operator=(const VirtualResourcePointer& b);
		LunarEngine::LResult MakeShared(const LunarObjectID& resource_id_in);
		inline LunarObjectID GetResourceId() const
		{
			return resource_id;
		}
		inline PancyBasicVirtualResource* GetResourceData()
		{
			if (if_NULL)
			{
				return NULL;
			}
			else
			{
				return data_pointer;
			}
		}
	};
	//todo：weak_ptr处理临时使用资源的情况
	class VirtualWeakResourcePointer
	{

	};
	class PancyGlobelResourceControl
	{
		std::unordered_map<LunarObjectID, PancyBasicVirtualResource*> basic_resource_array;
	private:
		std::unordered_map<std::string, LunarObjectID> resource_name_list;
		std::unordered_set<LunarObjectID> free_id_list;
		PancyGlobelResourceControl();
	public:
		static PancyGlobelResourceControl* GetInstance()
		{
			static PancyGlobelResourceControl* this_instance;
			if (this_instance == NULL)
			{
				this_instance = new PancyGlobelResourceControl();
			}
			return this_instance;
		}
		virtual ~PancyGlobelResourceControl();
		LunarEngine::LResult GetResourceById(const LunarObjectID& resource_id, PancyBasicVirtualResource** data_pointer);

		LunarEngine::LResult AddResurceReference(const LunarObjectID& resource_id);
		LunarEngine::LResult DeleteResurceReference(const LunarObjectID& resource_id);

		template<typename ResourceType>
		LunarEngine::LResult LoadResource(
			const std::string& desc_file_in,
			VirtualResourcePointer& id_need,
			bool if_allow_repeat = false
		);
		template<class ResourceType>
		LunarEngine::LResult LoadResource(
			const std::string& name_resource_in,
			const Json::Value& root_value,
			VirtualResourcePointer& id_need,
			bool if_allow_repeat = false
		);
		template<class ResourceType>
		LunarEngine::LResult LoadResource(
			const std::string& name_resource_in,
			void* resource_data,
			const std::string& resource_type,
			const LunarResourceSize& resource_size,
			VirtualResourcePointer& id_need,
			bool if_allow_repeat = false
		);
	private:
		LunarEngine::LResult AddResourceToControl(
			const std::string& name_resource_in,
			PancyBasicVirtualResource* new_data,
			VirtualResourcePointer& res_pointer,
			const bool& if_allow_repeat
		);
	};
	template<class ResourceType>
	LunarEngine::LResult PancyGlobelResourceControl::LoadResource(
		const std::string& name_resource_in,
		const Json::Value& root_value,
		VirtualResourcePointer& res_pointer,
		bool if_allow_repeat
	)
	{
		LunarEngine::LResult check_error;
		//资源加载判断重复
		if (!if_allow_repeat)
		{
			auto check_data = resource_name_list.find(name_resource_in);
			if (check_data != resource_name_list.end())
			{
				res_pointer.MakeShared(check_data->second);
				LunarEngine::LResult error_message;
				LunarDebugLogWarning(E_FAIL, "repeat load resource : " + name_resource_in, error_message);

				return error_message;
			}
		}
		//创建一个新的资源
		PancyBasicVirtualResource* new_data = new ResourceType(if_allow_repeat);
		check_error = new_data->Create(name_resource_in, root_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = AddResourceToControl(name_resource_in, new_data, res_pointer, if_allow_repeat);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename ResourceType>
	LunarEngine::LResult PancyGlobelResourceControl::LoadResource(
		const std::string& desc_file_in,
		VirtualResourcePointer& res_pointer,
		bool if_allow_repeat
	)
	{
		LunarEngine::LResult check_error;
		//资源加载判断重复
		if (!if_allow_repeat)
		{
			auto check_data = resource_name_list.find(desc_file_in);
			if (check_data != resource_name_list.end())
			{
				LunarEngine::LResult error_message;
				LunarDebugLogWarning(E_FAIL, "repeat load resource : " + desc_file_in, error_message);

				return error_message;
			}
		}
		//创建一个新的资源
		PancyBasicVirtualResource* new_data = new ResourceType(if_allow_repeat);
		check_error = new_data->Create(desc_file_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = AddResourceToControl(desc_file_in, new_data, res_pointer, if_allow_repeat);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<class ResourceType>
	LunarEngine::LResult PancyGlobelResourceControl::LoadResource(
		const std::string& name_resource_in,
		void* resource_data,
		const std::string& resource_type,
		const LunarResourceSize& resource_size,
		VirtualResourcePointer& id_need,
		bool if_allow_repeat
	)
	{
		LunarEngine::LResult check_error;
		//资源加载判断重复
		if (!if_allow_repeat)
		{
			auto check_data = resource_name_list.find(name_resource_in);
			if (check_data != resource_name_list.end())
			{
				LunarEngine::LResult error_message;
				LunarDebugLogWarning(E_FAIL, "repeat load resource : " + name_resource_in, error_message);

				return error_message;
			}
		}
		//创建一个新的资源
		PancyBasicVirtualResource* new_data = new ResourceType(if_allow_repeat);
		check_error = new_data->Create(name_resource_in, resource_data, resource_type, resource_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = AddResourceToControl(name_resource_in, new_data, id_need, if_allow_repeat);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
};


#pragma once
#include "core/core_module.h"
#include"PancyDx12Basic.h"
#include"PancyMemoryBasic.h"
#include"PancyBufferDx12.h"
#include"PancyThreadBasic.h"
#define IndexType uint32_t
namespace LunarEngine
{
	//纯顶点格式(用于网格显示)
	struct PointPositionSingle
	{
		DirectX::XMFLOAT4 position;
	};
	//2D顶点格式
	struct Point2D
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 tex_color;  //用于采样的坐标
		//DirectX::XMFLOAT4 tex_range;  //用于限制采样矩形的坐标
	};
	struct PointUI
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 tex_color;  //用于采样的坐标
		DirectX::XMUINT4  tex_id;     //UI的ID号
	};
	//标准3D顶点格式
	struct PointCommon
	{
		DirectX::XMFLOAT3 position;   //位置
		DirectX::XMFLOAT3 normal;     //法线
		DirectX::XMFLOAT3 tangent;    //切线
		DirectX::XMUINT4  tex_id;     //使用的纹理ID号
		DirectX::XMFLOAT4 tex_uv;     //用于采样的坐标
	};
	//带骨骼的顶点格式(最大支持单顶点4骨骼)
	struct PointSkinCommon4
	{
		DirectX::XMFLOAT3 position;   //位置
		DirectX::XMFLOAT3 normal;     //法线
		DirectX::XMFLOAT3 tangent;    //切线
		DirectX::XMUINT4  tex_id;     //使用的纹理ID号
		DirectX::XMFLOAT4 tex_uv;     //用于采样的坐标
		DirectX::XMUINT4  bone_id;    //骨骼ID号
		DirectX::XMFLOAT4 bone_weight;//骨骼权重
	};
	//带骨骼的顶点格式(最大支持单顶点8骨骼)
	struct PointSkinCommon8
	{
		DirectX::XMFLOAT3 position;     //位置
		DirectX::XMFLOAT3 normal;       //法线
		DirectX::XMFLOAT3 tangent;      //切线
		DirectX::XMUINT4  tex_id;       //使用的纹理ID号
		DirectX::XMFLOAT4 tex_uv;       //用于采样的坐标
		DirectX::XMUINT4  bone_id;      //骨骼ID号
		DirectX::XMFLOAT4 bone_weight0; //骨骼权重
		DirectX::XMFLOAT4 bone_weight1; //骨骼权重
	};
	//带顶点动画的顶点格式
	struct PointCatchCommon
	{
		DirectX::XMFLOAT3 position;   //位置
		DirectX::XMFLOAT3 normal;     //法线
		DirectX::XMFLOAT3 tangent;    //切线
		DirectX::XMUINT4  tex_id;     //使用的纹理ID号
		DirectX::XMFLOAT4 tex_uv;     //用于采样的坐标
		DirectX::XMUINT4  anim_id;    //对应的动画顶点
	};
	//几何体基础类型
	class GeometryBasic
	{
	protected:
		//几何体的渲染buffer
		VirtualResourcePointer geometry_vertex_buffer;
		VirtualResourcePointer geometry_index_buffer;
		VirtualResourcePointer geometry_adjindex_buffer;
		D3D12_VERTEX_BUFFER_VIEW geometry_vertex_buffer_view;
		D3D12_INDEX_BUFFER_VIEW geometry_index_buffer_view;
		D3D12_INDEX_BUFFER_VIEW geometry_adj_index_buffer_view;
		uint32_t all_vertex;
		uint32_t all_index;
		uint32_t all_index_adj;
		//几何体的创建信息
		bool if_create_adj;
		bool if_buffer_created;
	public:
		GeometryBasic();
		LunarEngine::LResult Create();
		virtual ~GeometryBasic();
		//检测数据是否正确创建
		bool CheckIfCreateSucceed();
		//获取缓冲区数据的大小
		inline uint32_t GetVetexNum()
		{
			return all_vertex;
		}
		inline uint32_t GetIndexNum()
		{
			return all_index;
		}
		inline uint32_t GetIndexAdjNum()
		{
			return all_index_adj;
		}
		//获取缓冲区数据的渲染指针(描述符)
		inline D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()
		{
			return geometry_vertex_buffer_view;
		};
		inline D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()
		{
			return geometry_index_buffer_view;
		};
		inline D3D12_INDEX_BUFFER_VIEW GetIndexAdjBufferView()
		{
			return geometry_adj_index_buffer_view;
		};
		//获取缓冲区数据的虚拟指针
		inline VirtualResourcePointer& GetVertexBufferResource()
		{
			return geometry_vertex_buffer;
		};
		inline VirtualResourcePointer& GetIndexBufferResource()
		{
			return geometry_index_buffer;
		};
		inline VirtualResourcePointer& GetAdjIndexBufferResource()
		{
			return geometry_adjindex_buffer;
		};
	protected:
		virtual LunarEngine::LResult InitGeometryDesc(
			bool& if_create_adj
		) = 0;
		virtual LunarEngine::LResult InitGeometry(
			uint32_t& all_vertex_need,
			uint32_t& all_index_need,
			uint32_t& all_index_adj_need,
			VirtualResourcePointer& geometry_vertex_buffer,
			VirtualResourcePointer& geometry_index_buffer,
			VirtualResourcePointer& geometry_adjindex_buffer,
			D3D12_VERTEX_BUFFER_VIEW& geometry_vertex_buffer_view_in,
			D3D12_INDEX_BUFFER_VIEW& geometry_index_buffer_view_in,
			D3D12_INDEX_BUFFER_VIEW& geometry_index_adj_buffer_view_in
		) = 0;
	};

	//基本模型几何体
	template<typename T>
	class GeometryCommonModel : public GeometryBasic
	{
		T* vertex_data;
		IndexType* index_data;
		bool if_save_CPU_data;//是否保留cpu备份

		//模型的基本数据
		bool if_model_adj;
		uint32_t all_model_vertex;
		uint32_t all_model_index;

	public:
		GeometryCommonModel(
			const T* vertex_data_in,
			const IndexType* index_data_in,
			const uint32_t& input_vert_num,
			const uint32_t& input_index_num,
			bool if_adj_in = false,
			bool if_save_cpu_data_in = false
		);
		LunarEngine::LResult GetModelData(
			std::vector<T>& vertex_data_in,
			std::vector<IndexType>& index_data_in
		);
		~GeometryCommonModel();
	private:
		LunarEngine::LResult InitGeometryDesc(
			bool& if_create_adj
		);
		LunarEngine::LResult InitGeometry(
			uint32_t& all_vertex_need,
			uint32_t& all_index_need,
			uint32_t& all_index_adj_need,
			VirtualResourcePointer& geometry_vertex_buffer,
			VirtualResourcePointer& geometry_index_buffer,
			VirtualResourcePointer& geometry_adjindex_buffer,
			D3D12_VERTEX_BUFFER_VIEW& geometry_vertex_buffer_view_in,
			D3D12_INDEX_BUFFER_VIEW& geometry_index_buffer_view_in,
			D3D12_INDEX_BUFFER_VIEW& geometry_index_adj_buffer_view_in
		);
	};
	template<typename T>
	GeometryCommonModel<T>::GeometryCommonModel(
		const T* vertex_data_in,
		const IndexType* index_data_in,
		const uint32_t& input_vert_num,
		const uint32_t& input_index_num,
		bool if_adj_in,
		bool if_save_cpu_data_in
	)
	{
		//拷贝CPU数据
		vertex_data = NULL;
		index_data = NULL;
		if (input_vert_num != 0)
		{
			all_model_vertex = input_vert_num;
			vertex_data = new T[input_vert_num];
			memcpy(vertex_data, vertex_data_in, input_vert_num * sizeof(T));
		}
		if (input_index_num != 0)
		{
			all_model_index = input_index_num;
			index_data = new IndexType[input_index_num];
			memcpy(index_data, index_data_in, input_index_num * sizeof(IndexType));
		}
		if_save_CPU_data = if_save_cpu_data_in;
		if_model_adj = if_adj_in;
	}
	template<typename T>
	GeometryCommonModel<T>::~GeometryCommonModel()
	{
		if (vertex_data != NULL)
		{
			delete[] vertex_data;
			vertex_data = NULL;

		}
		if (index_data != NULL)
		{
			delete[] index_data;
			index_data = NULL;
		}
	}
	template<typename T>
	LunarEngine::LResult GeometryCommonModel<T>::InitGeometryDesc(
		bool& if_create_adj
	)
	{
		if_create_adj = if_model_adj;
		return LunarEngine::g_Succeed;
	}
	template<typename T>
	LunarEngine::LResult GeometryCommonModel<T>::InitGeometry(
		uint32_t& all_vertex_need,
		uint32_t& all_index_need,
		uint32_t& all_index_adj_need,
		VirtualResourcePointer& geometry_vertex_buffer_in,
		VirtualResourcePointer& geometry_index_buffer_in,
		VirtualResourcePointer& geometry_adjindex_buffer_in,
		D3D12_VERTEX_BUFFER_VIEW& geometry_vertex_buffer_view_in,
		D3D12_INDEX_BUFFER_VIEW& geometry_index_buffer_view_in,
		D3D12_INDEX_BUFFER_VIEW& geometry_index_adj_buffer_view_in
	)
	{
		LunarEngine::LResult check_error;
		all_vertex_need = all_model_vertex;
		all_index_need = all_model_index;
		LunarResourceSize vbuffer_realszie = static_cast<LunarResourceSize>(all_vertex_need) * sizeof(T);
		LunarResourceSize ibuffer_real_size = 0;
		if (sizeof(IndexType) == sizeof(UINT) || sizeof(IndexType) == sizeof(uint16_t))
		{
			ibuffer_real_size = static_cast<LunarResourceSize>(all_index_need) * sizeof(IndexType);
		}
		else
		{
			ibuffer_real_size = 0;
			LunarEngine::LResult error_message;
			LunarDebugLogError(E_FAIL, "unsurpported index buffer type: " + std::to_string(sizeof(IndexType)), error_message);

			return error_message;
		}
		auto VertexBufferSize = LunarEngine::SizeAligned(vbuffer_realszie, 65536);
		auto IndexBufferSize = LunarEngine::SizeAligned(ibuffer_real_size, 65536);
		//创建顶点缓冲区
		PancyCommonBufferDesc buffer_resource_desc;
		buffer_resource_desc.buffer_type = Buffer_Vertex;
		buffer_resource_desc.buffer_res_desc.Alignment = 0;
		buffer_resource_desc.buffer_res_desc.DepthOrArraySize = 1;
		buffer_resource_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buffer_resource_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		buffer_resource_desc.buffer_res_desc.Height = 1;
		buffer_resource_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buffer_resource_desc.buffer_res_desc.MipLevels = 1;
		buffer_resource_desc.buffer_res_desc.SampleDesc.Count = 1;
		buffer_resource_desc.buffer_res_desc.SampleDesc.Quality = 0;
		buffer_resource_desc.buffer_res_desc.Width = VertexBufferSize;
		check_error = BuildBufferResource(
			"vertex_buffer",
			buffer_resource_desc,
			geometry_vertex_buffer_in,
			true
		);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//创建索引缓冲区
		buffer_resource_desc.buffer_type = Buffer_Index;
		buffer_resource_desc.buffer_res_desc.Width = IndexBufferSize;
		check_error = BuildBufferResource(
			"index_buffer",
			buffer_resource_desc,
			geometry_index_buffer_in,
			true
		);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}

		//todo 邻接三角面计算
		if (if_model_adj)
		{
		}
		//todo:修改成使用resource拷贝的格式
		//获取用于拷贝的commond list
		PancyRenderCommandList* copy_render_list;
		PancyThreadIdGPU copy_render_list_ID;
		check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->GetEmptyRenderlist(NULL, &copy_render_list, copy_render_list_ID);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//拷贝资源数据(顶点缓冲区)
		ResourceBlockGpu* vertex_buffer_gpu_resource = GetBufferResourceData(geometry_vertex_buffer_in, check_error);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = PancyDynamicRingBuffer::GetInstance()->CopyDataToGpu(copy_render_list, vertex_data, vbuffer_realszie, *vertex_buffer_gpu_resource);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//拷贝资源数据(索引缓冲区)
		ResourceBlockGpu* index_buffer_gpu_resource = GetBufferResourceData(geometry_index_buffer_in, check_error);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = PancyDynamicRingBuffer::GetInstance()->CopyDataToGpu(copy_render_list, index_data, ibuffer_real_size, *index_buffer_gpu_resource);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		copy_render_list->UnlockPrepare();
		//提交渲染命令
		ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitRenderlist(1, &copy_render_list_ID);
		//分配等待眼位
		PancyFenceIdGPU WaitFence;
		ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->SetGpuBrokenFence(WaitFence);
		check_error = vertex_buffer_gpu_resource->SetResourceCopyBrokenFence(WaitFence);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		check_error = index_buffer_gpu_resource->SetResourceCopyBrokenFence(WaitFence);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//删除CPU备份
		if (!if_save_CPU_data)
		{
			if (vertex_data != NULL)
			{
				delete[] vertex_data;
				vertex_data = NULL;
			}
			if (index_data != NULL)
			{
				delete[] index_data;
				index_data = NULL;
			}
		}
		//创建顶点缓存视图
		check_error = vertex_buffer_gpu_resource->BuildVertexBufferView(0, vbuffer_realszie, sizeof(T), geometry_vertex_buffer_view_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//创建索引缓存视图
		DXGI_FORMAT index_format;
		if (sizeof(IndexType) == sizeof(UINT))
		{
			index_format = DXGI_FORMAT_R32_UINT;
		}
		else if (sizeof(IndexType) == sizeof(uint16_t))
		{
			index_format = DXGI_FORMAT_R16_UINT;
		}
		check_error = index_buffer_gpu_resource->BuildIndexBufferView(0, ibuffer_real_size, index_format, geometry_index_buffer_view_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return LunarEngine::g_Succeed;
	}
	template<typename T>
	LunarEngine::LResult GeometryCommonModel<T>::GetModelData(
		std::vector<T>& vertex_data_in,
		std::vector<IndexType>& index_data_in
	)
	{
		vertex_data_in.clear();
		index_data_in.clear();
		if (!if_save_CPU_data)
		{
			LunarEngine::LResult error_message(E_FAIL, "the model doesn't save the data to cpu");

			return error_message;
		}
		for (int i = 0; i < all_vertex; ++i)
		{
			vertex_data_in.push_back(vertex_data[i]);
		}
		for (int i = 0; i < all_index; ++i)
		{
			index_data_in.push_back(index_data[i]);
		}
		return LunarEngine::g_Succeed;
	}
	class ModelResourceBasic
	{
	};
}
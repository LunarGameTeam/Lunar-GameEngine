#include"PancyThreadBasic.h"
ThreadPoolGPUControl* ThreadPoolGPUControl::threadpool_control_instance = NULL;
PancyRenderCommandList::PancyRenderCommandList(PancyThreadIdGPU command_list_ID_in)
{
	command_list_ID = command_list_ID_in;
	if_preparing.store(false);
	if_finish.store(true);
}
LunarEngine::LResult PancyRenderCommandList::Create(ID3D12CommandAllocator* allocator_use_in, ID3D12PipelineState* pso_use_in, D3D12_COMMAND_LIST_TYPE command_list_type_in)
{
	command_list_type = command_list_type_in;
	//创建commondlist
	HRESULT hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateCommandList(0, command_list_type_in, allocator_use_in, pso_use_in, IID_PPV_ARGS(&command_list_data));
	if (FAILED(hr))
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(hr, "Create CommandList Error When build commandlist", error_message);

		return error_message;
	}
	if_preparing.store(true);
	if_finish.store(false);
	return LunarEngine::g_Succeed;
}

LunarEngine::LResult CommandListEngine::Create()
{
	HRESULT hr;
	//创建并解锁alloctor
	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateCommandAllocator(engine_type_id, IID_PPV_ARGS(&allocator_use));
	if (FAILED(hr))
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(hr, "Create Direct CommandAllocator Error When init D3D basic", error_message);

		return error_message;
	}
	if_alloctor_locked = false;
	//创建用于接收gpu同步信息的fence
	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPU_thread_fence));
	if (FAILED(hr))
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(hr, "Create Direct Fence Error When init D3D basic", error_message);

		return error_message;
	}
	//创建一个同步fence消息的事件
	wait_thread_ID = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (wait_thread_ID == nullptr)
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(hr, "Create fence event Error When build commandlist", error_message);

		return error_message;
	}
	now_prepare_commandlist = 0;
	return LunarEngine::g_Succeed;
}
CommandListEngine::~CommandListEngine()
{
	ReleaseList(command_list_work);
	ReleaseList(command_list_finish);
	ReleaseList(command_list_empty);
}
CommandListEngine::CommandListEngine(const D3D12_COMMAND_LIST_TYPE& type_need)
{
	//为id号清零
	command_list_ID_selfadd = 0;
	engine_type_id = type_need;
	fence_value_self_add = 1;
	now_prepare_commandlist = 0;
	wait_thread_ID = NULL;
}
void CommandListEngine::UpdateLastRenderList()
{
	if (if_alloctor_locked)
	{
		//检查上一个处理的commandlist是否已经处理完毕
		auto now_use_commandlist = command_list_empty.find(now_prepare_commandlist);
		if (now_use_commandlist != command_list_empty.end())
		{
			if (!now_use_commandlist->second->CheckIfPrepare())
			{
				//上一个commandlist已经处理完毕，将其移动到已完成列表
				command_list_finish.insert(*now_use_commandlist);
				command_list_empty.erase(now_use_commandlist);
				if_alloctor_locked = false;
			}
		}
	}
}
LunarEngine::LResult CommandListEngine::GetEmptyRenderlist
(
	ID3D12PipelineState* pso_use_in,
	PancyRenderCommandList** command_list_data,
	PancyThreadIdGPU& command_list_ID
)
{
	//更新一下当前正在使用资源分配器的GPU命令，看其是否已经分配完毕
	UpdateLastRenderList();
	//资源分配器正在使用
	if (if_alloctor_locked)
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(E_FAIL, "the resource alloctor been using now,could not alloct new resource:", error_message);

		return error_message;
	}
	//检查当前是否还有剩余的空闲commandlist
	if (!command_list_empty.empty())
	{
		//从当前空闲的commanlist里获取一个commandlist
		command_list_ID = command_list_empty.begin()->first;
		*command_list_data = command_list_empty.begin()->second;
		(*command_list_data)->LockPrepare(allocator_use.Get(), pso_use_in);
		now_prepare_commandlist = command_list_ID;
	}
	else
	{
		//新建一个空闲的commandlist
		PancyRenderCommandList* new_render_command_list = new PancyRenderCommandList(command_list_ID_selfadd);
		LunarEngine::LResult check_error;
		check_error = new_render_command_list->Create(allocator_use.Get(), pso_use_in, engine_type_id);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		command_list_empty.insert(std::pair<uint32_t, PancyRenderCommandList*>(command_list_ID_selfadd, new_render_command_list));
		command_list_ID = command_list_empty.begin()->first;
		*command_list_data = command_list_empty.begin()->second;
		now_prepare_commandlist = command_list_ID;
		command_list_ID_selfadd += 1;
	}
	//锁住资源分配器
	if_alloctor_locked = true;
	return LunarEngine::g_Succeed;
}
LunarEngine::LResult CommandListEngine::FreeAlloctor()
{
	if (command_list_work.size() != 0)
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(E_FAIL, "not all of the command list work finished engine ID: " + std::to_string(engine_type_id), error_message);

		return error_message;
	}
	HRESULT hr = allocator_use->Reset();
	if (FAILED(hr))
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(hr, "release command allocator error: " + std::to_string(engine_type_id), error_message);

		return error_message;
	}
	return LunarEngine::g_Succeed;
}
LunarEngine::LResult CommandListEngine::SubmitRenderlist
(
	const uint32_t command_list_num,
	const PancyThreadIdGPU* command_list_ID
)
{
	if (command_list_num > MaxSubmitCommandList)
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(E_FAIL, "could not submit too much commandlist which num > " + std::to_string(MaxSubmitCommandList), error_message);

		return error_message;
	}
	//更新commandlist池
	UpdateLastRenderList();
	//开辟用于提交的commandlist数组
	std::vector<PancyRenderCommandList*> now_render_list_array;
	//组织待提交的所有commandlist
	auto now_working_list = GPU_broken_point.find(fence_value_self_add);
	if (now_working_list == GPU_broken_point.end())
	{
		std::vector<PancyThreadIdGPU> new_thread;
		GPU_broken_point.insert(std::pair<PancyFenceIdGPU, std::vector<PancyThreadIdGPU>>(fence_value_self_add, new_thread));
		now_working_list = GPU_broken_point.find(fence_value_self_add);
	}
	for (uint32_t i = 0; i < command_list_num; ++i)
	{
		//将待提交的commandlist加入当前的GPU断点区域
		now_working_list->second.push_back(command_list_ID[i]);
		//组装commandlist
		auto finish_command_list_use = command_list_finish.find(command_list_ID[i]);
		if (finish_command_list_use != command_list_finish.end())
		{
			now_render_list_array.push_back(finish_command_list_use->second);
			commandlist_array[i] = finish_command_list_use->second->GetCommandList();
			//转换commandlist
			command_list_work.insert(*finish_command_list_use);
			command_list_finish.erase(finish_command_list_use);
		}
		else
		{
			//无法找到commandlist
			LunarEngine::LResult error_message;
			LunarDebugLogWarning(E_FAIL, "could not find finish thread ID" + std::to_string(command_list_ID[i]), error_message);

			return error_message;
		}
	}
	if (command_list_num != 0)
	{
		ID3D12CommandQueue* now_command_queue;
		//找到对应的commandqueue
		if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)
		{
			now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueDirect();
		}
		if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY)
		{
			now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueCopy();
		}
		if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{
			now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueCompute();
		}
		//提交commandlist
		now_command_queue->ExecuteCommandLists(command_list_num, commandlist_array);
		return LunarEngine::g_Succeed;
	}
	return LunarEngine::g_Succeed;
}
LunarEngine::LResult CommandListEngine::SetGpuBrokenFence(PancyFenceIdGPU& broken_point_id)
{
	ComPtr<ID3D12CommandQueue> now_command_queue;
	//找到对应的commandqueue
	if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)
	{
		now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueDirect();
	}
	if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY)
	{
		now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueCopy();
	}
	if (engine_type_id == D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE)
	{
		now_command_queue = PancyDx12DeviceBasic::GetInstance()->GetCommandQueueCompute();
	}
	//为GPU创建一个回调信号
	HRESULT hr = now_command_queue->Signal(GPU_thread_fence.Get(), fence_value_self_add);
	if (FAILED(hr))
	{
		LunarEngine::LResult error_message;
		LunarDebugLogError(E_FAIL, "signel waiting value error in engine ID: " + std::to_string(engine_type_id), error_message);

		return error_message;
	}
	broken_point_id = fence_value_self_add;
	fence_value_self_add += 1;
	return LunarEngine::g_Succeed;
}
bool CommandListEngine::CheckGpuBrokenFence(const PancyFenceIdGPU& broken_point_id)
{
	PancyFenceIdGPU now_wait_value = GPU_thread_fence->GetCompletedValue();
	if (now_wait_value < broken_point_id)
	{
		return false;
	}
	//将小于等于该眼位的所有眼位所监视的线程设置为已工作完毕
	auto now_working_list = GPU_broken_point.begin();
	while (now_working_list != GPU_broken_point.end() && now_working_list->first <= broken_point_id)
	{
		for (auto working_thread = now_working_list->second.begin(); working_thread != now_working_list->second.end(); ++working_thread)
		{
			auto work_command_list_use = command_list_work.find(*working_thread);
			if (work_command_list_use == command_list_work.end())
			{
				LunarEngine::LResult error_message;
				LunarDebugLogError(E_FAIL, "could not find the working thread id :" + std::to_string(*working_thread), error_message);

			}
			//将该线程标记为运行结束的待回收线程
			work_command_list_use->second->EndProcessing();
			command_list_empty.insert(*work_command_list_use);
			command_list_work.erase(work_command_list_use);
		}
		GPU_broken_point.erase(now_working_list);
		now_working_list = GPU_broken_point.begin();
	}
	return true;
}
LunarEngine::LResult CommandListEngine::WaitGpuBrokenFence(const PancyFenceIdGPU& broken_point_id)
{
	if (!CheckGpuBrokenFence(broken_point_id))
	{
		HRESULT hr;

		hr = GPU_thread_fence->SetEventOnCompletion(broken_point_id, wait_thread_ID);
		if (FAILED(hr))
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(hr, "reflect GPU thread fence to CPU thread handle failed", error_message);

			return error_message;
		}
		WaitForSingleObject(wait_thread_ID, INFINITE);
		//将小于等于该眼位的所有眼位所监视的线程设置为已工作完毕
		auto now_working_list = GPU_broken_point.begin();
		while (now_working_list != GPU_broken_point.end() && now_working_list->first <= broken_point_id)
		{
			for (auto working_thread = now_working_list->second.begin(); working_thread != now_working_list->second.end(); ++working_thread)
			{
				auto work_command_list_use = command_list_work.find(*working_thread);
				if (work_command_list_use == command_list_work.end())
				{
					LunarEngine::LResult error_message;
					LunarDebugLogError(E_FAIL, "could not find the working thread id :" + std::to_string(*working_thread), error_message);

					return error_message;
				}
				//将该线程标记为运行结束的待回收线程
				work_command_list_use->second->EndProcessing();
				command_list_empty.insert(*work_command_list_use);
				command_list_work.erase(work_command_list_use);
			}
			GPU_broken_point.erase(now_working_list);
			now_working_list = GPU_broken_point.begin();
		}
	}
	return LunarEngine::g_Succeed;
}

ThreadPoolGPU::ThreadPoolGPU(uint32_t GPUThreadPoolID_in, bool if_rename_in)
{
	GPUThreadPoolID = GPUThreadPoolID_in;
	if_rename = if_rename_in;
	if (if_rename)
	{
		int32_t frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
		multi_engine_list.resize(frame_num);
	}
	else
	{
		multi_engine_list.resize(1);
	}
}
ThreadPoolGPU::~ThreadPoolGPU()
{
	for (int32_t i = 0; i < multi_engine_list.size(); ++i)
	{
		ReleaseList(multi_engine_list[i]);
	}
}
LunarEngine::LResult ThreadPoolGPU::BuildNewEngine(D3D12_COMMAND_LIST_TYPE engine_type)
{
	int32_t frame_num;
	//获取引擎所使用的最大rename数量
	if (if_rename)
	{
		frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	}
	else
	{
		frame_num = 1;
	}
	//创建对应类型的commandlist引擎
	for (int i = 0; i < frame_num; ++i)
	{
		LunarEngine::LResult check_error;
		CommandListEngine* new_engine_list = new CommandListEngine(engine_type);
		check_error = new_engine_list->Create();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		multi_engine_list[i].insert(std::pair<PancyEngineIdGPU, CommandListEngine*>(static_cast<PancyEngineIdGPU>(engine_type), new_engine_list));
	}
	return LunarEngine::g_Succeed;
}
LunarEngine::LResult ThreadPoolGPU::Create()
{
	return LunarEngine::g_Succeed;
}
CommandListEngine* ThreadPoolGPU::GetThreadPool(D3D12_COMMAND_LIST_TYPE engine_type)
{
	int32_t now_frame;
	if (if_rename)
	{
		now_frame = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	}
	else
	{
		now_frame = 0;
	}
	PancyEngineIdGPU engine_id = static_cast<PancyEngineIdGPU>(engine_type);
	auto engine_data = multi_engine_list[now_frame].find(engine_id);
	if (engine_data == multi_engine_list[now_frame].end())
	{
		//尚未创建对应类型的存储开辟装置
		LunarEngine::LResult check_error = BuildNewEngine(engine_type);
		if (check_error.m_IsOK)
		{
			engine_data = multi_engine_list[now_frame].find(engine_id);
			return engine_data->second;
		}
		else
		{
			return NULL;
		}
	}
	return engine_data->second;
}
CommandListEngine* ThreadPoolGPU::GetLastThreadPool(D3D12_COMMAND_LIST_TYPE engine_type)
{
	int32_t now_frame;
	if (if_rename)
	{
		now_frame = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	}
	else
	{
		now_frame = 0;
	}
	PancyEngineIdGPU engine_id = static_cast<PancyEngineIdGPU>(engine_type);
	auto engine_data = multi_engine_list[now_frame].find(engine_id);
	if (engine_data == multi_engine_list[now_frame].end())
	{
		//尚未创建对应类型的存储开辟装置
		LunarEngine::LResult check_error = BuildNewEngine(engine_type);
		if (check_error.m_IsOK)
		{
			engine_data = multi_engine_list[now_frame].find(engine_id);
			return engine_data->second;
		}
		else
		{
			return NULL;
		}
	}
	return engine_data->second;
}
//线程池管理器
ThreadPoolGPUControl::ThreadPoolGPUControl()
{
	main_contex = NULL;
	resource_load_contex = NULL;
	main_contex = new ThreadPoolGPU(0);
	resource_load_contex = new ThreadPoolGPU(1, false);
	GPU_thread_pool_self_add = 2;
}
LunarEngine::LResult ThreadPoolGPUControl::Create()
{
	LunarEngine::LResult check_error;
	check_error = main_contex->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = resource_load_contex->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return LunarEngine::g_Succeed;
}
ThreadPoolGPUControl::~ThreadPoolGPUControl()
{
	if (main_contex != NULL)
	{
		delete main_contex;
	}
	if (resource_load_contex != NULL)
	{
		delete resource_load_contex;
	}
	for (auto data_working = GPU_thread_pool_working.begin(); data_working != GPU_thread_pool_working.end(); ++data_working)
	{
		delete data_working->second;
	}
	GPU_thread_pool_working.clear();
	for (auto data_working = GPU_thread_pool_empty.begin(); data_working != GPU_thread_pool_empty.end(); ++data_working)
	{
		delete data_working->second;
	}
	GPU_thread_pool_empty.clear();
}

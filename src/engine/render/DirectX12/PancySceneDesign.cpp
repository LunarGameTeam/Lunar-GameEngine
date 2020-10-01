#include"PancySceneDesign.h"
using namespace LunarEngine;
SceneRoot::SceneRoot()
{
	If_dsv_loaded = false;
	time_game = 0;
	scene_center_pos = DirectX::XMFLOAT3(0, 0, 0);
}
LResult SceneRoot::Create(int32_t width_in, int32_t height_in)
{
	back_buffer_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	frame_constant_buffer.resize(back_buffer_num);

	LResult check_error;
	check_error = ResetScreen(width_in, height_in);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = Init();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult SceneRoot::ResetScreen(int32_t width_in, int32_t height_in)
{
	LResult check_error;
	Scene_width = width_in;
	Scene_height = height_in;
	std::vector<D3D12_HEAP_FLAGS> heap_flags;
	//创建新的屏幕空间纹理格式
	D3D12_RESOURCE_DESC default_tex_RGB_dxdesc;
	default_tex_RGB_dxdesc.Alignment = 0;
	default_tex_RGB_dxdesc.DepthOrArraySize = 1;
	default_tex_RGB_dxdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	default_tex_RGB_dxdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	default_tex_RGB_dxdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	default_tex_RGB_dxdesc.Height = Scene_height;
	default_tex_RGB_dxdesc.Width = Scene_width;
	default_tex_RGB_dxdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	default_tex_RGB_dxdesc.MipLevels = 1;
	default_tex_RGB_dxdesc.SampleDesc.Count = 1;
	default_tex_RGB_dxdesc.SampleDesc.Quality = 0;
	//创建rgb8类型的窗口大小纹理格式
	default_tex_desc_RGB.heap_flag_in = D3D12_HEAP_FLAG_NONE;
	default_tex_desc_RGB.heap_type = D3D12_HEAP_TYPE_DEFAULT;
	default_tex_desc_RGB.if_force_srgb = false;
	default_tex_desc_RGB.if_gen_mipmap = false;
	default_tex_desc_RGB.max_size = 0;
	default_tex_desc_RGB.texture_data_file = "";
	default_tex_desc_RGB.texture_res_desc = default_tex_RGB_dxdesc;
	default_tex_desc_RGB.texture_type = Texture_Render_Target;
	//创建srgb8类型的窗口大小纹理格式
	default_tex_desc_SRGB = default_tex_desc_RGB;
	default_tex_desc_SRGB.texture_res_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//创建rgb16类型的窗口大小纹理格式
	default_tex_desc_float = default_tex_desc_RGB;
	default_tex_desc_float.texture_res_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	//创建深度模板缓冲区
	default_tex_desc_depthstencil = default_tex_desc_RGB;
	default_tex_desc_depthstencil.texture_res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	default_tex_desc_depthstencil.texture_res_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	default_tex_desc_depthstencil.heap_flag_in = D3D12_HEAP_FLAG_NONE;
	if (If_dsv_loaded)
	{
		//todo:删除旧的深度模板缓冲区
		/*
		for (int i = 0; i < back_buffer_num; ++i)
		{
			PancyTextureControl::GetInstance()->DeleteResurceReference(Default_depthstencil_buffer[i]);
		}
		*/
	}

	std::vector<BasicDescriptorDesc> back_buffer_desc;
	for (int i = 0; i < back_buffer_num; ++i)
	{
		//加载深度模板缓冲区
		VirtualResourcePointer default_dsv_resource;
		auto check_error = BuildTextureResource("ScreenSpaceDepthstencil", default_tex_desc_depthstencil, default_dsv_resource, true);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		Default_depthstencil_buffer.push_back(default_dsv_resource);
		//创建深度模板缓冲区描述符
		D3D12_DEPTH_STENCIL_VIEW_DESC DSV_desc;
		DSV_desc.Flags = D3D12_DSV_FLAG_NONE;
		DSV_desc.Format = default_tex_desc_depthstencil.texture_res_desc.Format;
		DSV_desc.Texture2D.MipSlice = 0;
		DSV_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		BasicDescriptorDesc new_descriptor_desc;
		new_descriptor_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeDepthStencilView;
		new_descriptor_desc.depth_stencil_view_desc = DSV_desc;
		back_buffer_desc.push_back(new_descriptor_desc);
	}
	check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonGlobelDescriptor("DefaultDepthBufferSRV", back_buffer_desc, Default_depthstencil_buffer, true);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	check_error = ScreenChange();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	If_dsv_loaded = true;
	return g_Succeed;
}
LResult SceneRoot::GetGlobelCbufferByFrame(
	const LunarObjectID& frame_id,
	const LunarObjectID& PSO_id,
	const std::string& cbuffer_name,
	PancyConstantBuffer** cbuffer_data,
	bool& if_create
)
{
	LResult check_error;
	//根据pso的id号查找对应的Cbuffer链表
	auto PSO_cbuffer_list = frame_constant_buffer[frame_id].find(PSO_id);
	if (PSO_cbuffer_list == frame_constant_buffer[frame_id].end())
	{
		if_create = true;
		//指定的pso尚未创建cbuffer
		std::string pso_name_pre;
		//先检查pso是否存在
		check_error = PancyEffectGraphic::GetInstance()->GetPSOName(PSO_id, pso_name_pre);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//添加一个PSO的cbuffer表
		std::unordered_map<std::string, PancyConstantBuffer*> new_pso_cbuffer_list;
		frame_constant_buffer[frame_id].insert(std::pair<LunarObjectID, std::unordered_map<std::string, PancyConstantBuffer*>>(PSO_id, new_pso_cbuffer_list));
		PSO_cbuffer_list = frame_constant_buffer[frame_id].find(PSO_id);
	}
	//根据cbuffer的名称寻找常量缓冲区
	auto cbuffer_out = PSO_cbuffer_list->second.find(cbuffer_name);
	if (cbuffer_out == PSO_cbuffer_list->second.end())
	{
		//cbuffer未创建，加载一个常量缓冲区
		PancyConstantBuffer* new_cbuffer = new PancyConstantBuffer();
		check_error = PancyEffectGraphic::GetInstance()->BuildCbufferByName(PSO_id, cbuffer_name, *new_cbuffer);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		PSO_cbuffer_list->second.insert(std::pair<std::string, PancyConstantBuffer*>(cbuffer_name, new_cbuffer));
		cbuffer_out = PSO_cbuffer_list->second.find(cbuffer_name);
	}
	*cbuffer_data = cbuffer_out->second;
	return g_Succeed;
}
LResult SceneRoot::GetGlobelCbuffer(
	const LunarObjectID& PSO_id,
	const std::string& cbuffer_name,
	PancyConstantBuffer** cbuffer_data
)
{
	LResult check_error;
	bool if_create = false;
	LunarObjectID now_frame = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	check_error = GetGlobelCbufferByFrame(now_frame, PSO_id, cbuffer_name, cbuffer_data, if_create);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult SceneRoot::GetGlobelCbuffer(
	const LunarObjectID& PSO_id,
	const std::string& cbuffer_name,
	std::vector<PancyConstantBuffer*>& cbuffer_data
)
{
	LResult check_error;
	LunarObjectID frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	//todo：目前先在加载的时候顺便完成全局cbuffer的处理，以后全局cbuffer需要提前创建
	std::vector<VirtualResourcePointer> back_buffer_data;
	std::vector<BasicDescriptorDesc> back_buffer_desc;
	bool if_create = false;
	for (LunarObjectID i = 0; i < frame_num; ++i)
	{
		PancyConstantBuffer* cbuffer_data;
		check_error = GetGlobelCbufferByFrame(i, PSO_id, cbuffer_name, &cbuffer_data, if_create);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		cbuffer_data->GetBufferResource();
		back_buffer_data.push_back(cbuffer_data->GetBufferResource());
		BasicDescriptorDesc new_desc;
		new_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeConstantBufferView;
		new_desc.offset = cbuffer_data->GetCbufferOffsetFromBufferHead();
		new_desc.block_size = cbuffer_data->GetCbufferSize();
		back_buffer_desc.push_back(new_desc);
	}
	std::string pso_name;
	check_error = PancyEffectGraphic::GetInstance()->GetPSOName(PSO_id, pso_name);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建描述符
	if (if_create)
	{
		check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonGlobelDescriptor(cbuffer_name, back_buffer_desc, back_buffer_data, true);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	return g_Succeed;
}
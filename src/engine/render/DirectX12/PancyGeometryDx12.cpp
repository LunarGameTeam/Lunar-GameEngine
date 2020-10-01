#include"PancyGeometryDx12.h"
using namespace LunarEngine;
GeometryBasic::GeometryBasic()
{
	all_vertex = 0;
	all_index = 0;
	all_index_adj = 0;
	if_buffer_created = false;
}
LResult GeometryBasic::Create()
{
	LResult check_error;
	//检验是否资源已经创建
	if (if_buffer_created)
	{
		return g_Succeed;
	}
	//注册几何体格式
	check_error = InitGeometryDesc(if_create_adj);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//注册几何体资源
	check_error = InitGeometry(
		all_vertex,
		all_index,
		all_index_adj,
		geometry_vertex_buffer,
		geometry_index_buffer,
		geometry_adjindex_buffer,
		geometry_vertex_buffer_view,
		geometry_index_buffer_view,
		geometry_adj_index_buffer_view
	);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	if_buffer_created = true;
	return g_Succeed;
}
bool GeometryBasic::CheckIfCreateSucceed()
{
	LResult check_error;
	bool if_vbuffer_load_succeed = geometry_vertex_buffer.GetResourceData()->CheckIfResourceLoadFinish();
	bool if_ibuffer_load_succeed = geometry_index_buffer.GetResourceData()->CheckIfResourceLoadFinish();
	bool if_iadjbuffer_load_succeed = true;
	if (if_create_adj)
	{
		if_iadjbuffer_load_succeed = geometry_adjindex_buffer.GetResourceData()->CheckIfResourceLoadFinish();
	}
	if (if_vbuffer_load_succeed && if_ibuffer_load_succeed && if_iadjbuffer_load_succeed)
	{
		return true;
	}
	return false;
}
GeometryBasic::~GeometryBasic()
{
}
#pragma once
#include"ILunarGraphicCore.h"
//shader输入布局
class LRootSignature
{
	luna::LString m_file_name;
	LPtrBasic m_root_signature_data;
public:
	LRootSignature(const luna::LString& file_name, LSharedObject* parent);
	~LRootSignature()
	{
	};
	luna::LResult Create();
};
//渲染管线
class LPipelineStateObject
{
	luna::LString m_file_name;
	LPtrBasic m_pipeline_state_data;
public:
	LPipelineStateObject(const luna::LString& file_name, LSharedObject* parent);
	~LPipelineStateObject()
	{
	};
	luna::LResult Create();
};
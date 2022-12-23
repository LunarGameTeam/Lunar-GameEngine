#pragma once
#include "fbx_loader_common.h"
namespace luna::lfbx
{
	LFbxImportLog* LFbxImportLog::mInstance = new LFbxImportLog();
	void LFbxImportLog::AddLog(const LString log, ...)
	{
		LString _Result;
		va_list _ArgList;
		va_start(_ArgList, log);
		for (int32_t i = 0; i < log.Length(); ++i)
		{
			if (log[i] != '%')
			{
				_Result.PushBack(log[i]);
				continue;
			}
			i += 1;
			if (i >= log.Length())
			{
				break;
			}
			switch (log[i])
			{
			case 'd':
			{
				int int_value = va_arg(_ArgList, int);
				_Result.Append(std::to_string(int_value));
				break;
			}
			case 's':
			{
				LString string_value = va_arg(_ArgList, char*);
				_Result.Append(string_value);
				break;
			}
			default:
			{
				break;
			}
			}
		}
		mLogData.push_back(_Result);
	}
	std::shared_ptr<LFbxDataBase> LFbxLoaderBase::ParsingData(const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager)
	{
		return ParsingDataImpl(sceneNodes, pNode, pManager);
	}
}
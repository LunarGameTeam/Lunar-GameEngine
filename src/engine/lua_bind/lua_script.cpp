#include "lua_script.h"

namespace luna
{


void LuaScript::OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
{
	m_content = LString((char*)file->GetData().data(), (char *)file->GetData().data() + file->GetData().size());
}

const LString &LuaScript::GetScriptContent()
{
	return m_content;
}

}
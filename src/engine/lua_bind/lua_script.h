#pragma once

#include "private_lua.h"

#include "core/misc/string.h"
#include "core/asset/lasset.h"

namespace luna
{

class LuaScript : public LBasicAsset
{
public:
	void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file) override;
	const LString &GetScriptContent();
private:
	LString m_content;



};


}
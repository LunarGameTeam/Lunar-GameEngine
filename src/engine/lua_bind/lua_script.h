#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>
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
#pragma once

#include "core/math/math.h"
#include "legacy_render/asset/texture.h"
#include "legacy_render/private_render.h"
#include "core/misc/string.h"



namespace luna
{
namespace legacy_render
{

class LEGACY_RENDER_API Texture2D : public Texture 
{
public:
	void OnAssetFileLoad(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
	unsigned char *GetData() override;
	size_t GetDataSize() override;


private:
	unsigned char *m_data;
};


}
}

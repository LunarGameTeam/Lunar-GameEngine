#pragma once
#include "core/misc/container.h"
#include "core/asset/asset.h"

#include "legacy_render/private_render.h"
#include "legacy_render/interface/i_model.h"
#include "legacy_render/asset/texture.h"
#include "legacy_render/d3d11/shader.h"

#include <d3d11.h>

namespace luna
{
namespace legacy_render
{


class LEGACY_RENDER_API Material : public LBasicAsset
{
public:
	void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file) override;

public:

private:
	LSharedPtr<Texture> m_texture;
	LSharedPtr<Dx11Shader> m_shader;

};

}
}

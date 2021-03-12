#include "material.h"
#include "core/asset/asset.h"
#include "core/asset/asset_subsystem.h"
#include "legacy_render/asset/material.h"
#include "legacy_render/d3d11/shader.h"

namespace luna
{
namespace legacy_render 
{

void Material::OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
{
	static auto* asset_sys = gEngine->GetSubsystem<AssetSubsystem>();
	m_shader = asset_sys->LoadAsset<legacy_render::Dx11Shader>(meta->m_value["Shader"].asString().c_str());
	m_texture = asset_sys->LoadAsset<legacy_render::Texture2D>(meta->m_value["Texture"].asString().c_str());
}


}
}
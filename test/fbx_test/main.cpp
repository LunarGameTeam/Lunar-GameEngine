#include "lib/fbx_helper/fbx_importer_func.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "core/serialization/serialization.h"
#include "render/asset/material_template.h"
int main()
{
	gEngine = luna::TCreateObject<luna::LunaCore>();
	gEngine->LoadModule<luna::FileSystem>();
	gEngine->LoadModule<luna::AssetSystem>();
	luna::g_file_sys->OnInit();
	luna::IPlatformFileManager* manager = luna::g_file_sys->GetPlatformFileManager();
	luna::render::MeshAsset* m_render_mesh = luna::TCreateObject<luna::render::MeshAsset>();
	m_render_mesh->ResetUUID();
	luna::ImportFbxToLunaMesh("E:/LunarEngine/lunar-engine-branch/Lunar-GameEngine/assets/built-in/fbx/sphere.FBX", m_render_mesh);
	luna::g_asset_sys->SaveAsset(m_render_mesh, "E:/LunarEngine/lunar-engine-branch/Lunar-GameEngine/assets/built-in/sphere.lmesh");
	//auto mesh = luna::g_asset_sys->LoadAsset<luna::render::MeshAsset>("E:/LunarEngine/lunar-engine-branch/Lunar-GameEngine/assets/built-in/sphere.lmesh");
	return 0;
}
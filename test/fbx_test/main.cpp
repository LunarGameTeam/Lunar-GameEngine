#include "lib/fbx_helper/fbx_importer_func.h"
#include "lib/asset_import/mesh_asset_import.h"
#include "core/file/file_subsystem.h"
#include "core/Asset/asset_subsystem.h"
#include "core/Serialization/serialization.h"
#include "Graphics/Asset/material_template.h"
int main()
{
	//gEngine = luna::TCreateObject<luna::LunaCore>();
	//gEngine->LoadModule<luna::FileSystem>();
	//gEngine->LoadModule<luna::AssetSystem>();
	//luna::g_file_sys->OnInit();
	//luna::IPlatformFileManager* manager = luna::g_file_sys->GetPlatformFileManager();
	//luna::render::MeshAsset* m_render_mesh = luna::TCreateObject<luna::render::MeshAsset>();
	//m_render_mesh->ResetUUID();
	luna::resimport::LImportScene importScene;
	luna::lfbx::ImportFbxToLunaMesh("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/fbx/inside_sphere.FBX", importScene);
	importScene.ConvertDataAxisAndUnit(luna::resimport::LImportAxisType::ImportAxisYupLeftHand, luna::resimport::LImportUnitType::ImportUnitMeter);
	importScene.PostProcessData();
	luna::assetimport::LMeshAssetImport meshAssetImporter;
	luna::assetimport::LAssetPack assetPackData;
	meshAssetImporter.ParsingImportScene(importScene, assetPackData);
	assetPackData.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in");
	//luna::g_asset_sys->SaveAsset(m_render_mesh, "E:/LunarEngine/lunar-engine-branch/Lunar-GameEngine/assets/built-in/sphere.lmesh");
	//auto mesh = luna::g_asset_sys->LoadAsset<luna::render::MeshAsset>("E:/LunarEngine/lunar-engine-branch/Lunar-GameEngine/assets/built-in/sphere.lmesh");
	return 0;
}
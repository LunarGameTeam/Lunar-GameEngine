#include "lib/fbx_helper/fbx_importer_func.h"
#include "lib/obj_helper/obj_importer_func.h"
#include "lib/gltf_helper/gltf_importer_func.h"
#include "lib/asset_import/mesh_asset_import.h"
#include"render/asset/mesh_asset.h"
int main()
{
	//gEngine = luna::TCreateObject<luna::LunaCore>();
	//gEngine->LoadModule<luna::FileSystem>();
	//gEngine->LoadModule<luna::AssetSystem>();
	//luna::g_file_sys->OnInit();
	//luna::IPlatformFileManager* manager = luna::g_file_sys->GetPlatformFileManager();
	//luna::render::MeshAsset* m_render_mesh = luna::TCreateObject<luna::render::MeshAsset>();
	//m_render_mesh->ResetUUID();
	
	//gltf test
	luna::resimport::LImportScene importSceneGltf;
	luna::lgltf::ImportGltfToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/gltf/scene.gltf", importSceneGltf);
	importSceneGltf.PostProcessData();
	luna::assetimport::LMeshAssetImport meshAssetImporterGltf;
	luna::assetimport::LAssetPack assetPackDataGltf;
	meshAssetImporterGltf.ParsingImportScene(importSceneGltf, assetPackDataGltf);
	assetPackDataGltf.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in", "Material_sphere");
	//obj test
	luna::resimport::LImportScene importSceneObj;
	luna::lobj::ImportObjToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/inside_sphere.obj", importSceneObj);
	importSceneObj.PostProcessData();
	luna::assetimport::LMeshAssetImport meshAssetImporterObj;
	luna::assetimport::LAssetPack assetPackDataObj;
	meshAssetImporterObj.ParsingImportScene(importSceneObj, assetPackDataObj);
	assetPackDataObj.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in", "inside_sphere");
	//fbx test
	luna::resimport::LImportScene importScene;
	luna::lfbx::ImportFbxToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/fbx/inside_sphere.FBX", importScene);
	importScene.ConvertDataAxisAndUnit(luna::resimport::LImportAxisType::ImportAxisYupLeftHand, luna::resimport::LImportUnitType::ImportUnitMeter);
	importScene.PostProcessData();
	luna::assetimport::LMeshAssetImport meshAssetImporter;
	luna::assetimport::LAssetPack assetPackData;
	meshAssetImporter.ParsingImportScene(importScene, assetPackData);
	assetPackData.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in","inside_sphere");
	//asset load test
	luna::render::MeshAsset* asset_check = luna::sAssetModule->LoadAsset<luna::render::MeshAsset>("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/inside_sphere.lmesh");
	return 0;
}
#include "Lib/FbxParser/FbxImporterFunc.h"
#include "Lib/ObjParser/ObjImporterFunc.h"
#include "Lib/GltfParser/GltfImporterFunc.h"
#include "Lib/AssetImport/MeshAssetImport.h"
#include "Graphics/Asset/MeshAsset.h"
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
	//luna::resimport::LImportScene importSceneGltf;
	//luna::lgltf::ImportGltfToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/gltf/scene.gltf", importSceneGltf);
	//importSceneGltf.PostProcessData();
	//luna::assetimport::LMeshAssetImport meshAssetImporterGltf;
	//luna::assetimport::LAssetPack assetPackDataGltf;
	//meshAssetImporterGltf.ParsingImportScene(importSceneGltf, assetPackDataGltf);
	//assetPackDataGltf.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in", "Material_sphere");
	//obj test
	LArray<luna::LString> allFiles;
	allFiles.push_back("Box");
	allFiles.push_back("InsideCube");
	allFiles.push_back("InsideSphere");
	allFiles.push_back("Plane");
	allFiles.push_back("Sphere");
	for(int32_t i = 0; i < allFiles.size(); ++i)
	{
		luna::resimport::LImportScene importSceneObj;
		luna::lobj::ImportObjToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/Geometry/" + allFiles[i] + ".obj", importSceneObj);
		importSceneObj.PostProcessData();
		luna::assetimport::LMeshAssetImport meshAssetImporterObj;
		luna::assetimport::LAssetPack assetPackDataObj;
		meshAssetImporterObj.ParsingImportScene(importSceneObj, assetPackDataObj);
		assetPackDataObj.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/Geometry/", allFiles[i]);
	}
	
	//fbx test
	//luna::resimport::LImportScene importScene;
	//luna::lfbx::ImportFbxToLunaResource("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/fbx/inside_sphere.FBX", importScene);
	//importScene.ConvertDataAxisAndUnit(luna::resimport::LImportAxisType::ImportAxisYupLeftHand, luna::resimport::LImportUnitType::ImportUnitMeter);
	//importScene.PostProcessData();
	//luna::assetimport::LMeshAssetImport meshAssetImporter;
	//luna::assetimport::LAssetPack assetPackData;
	//meshAssetImporter.ParsingImportScene(importScene, assetPackData);
	//assetPackData.SerializeAllAsset("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in","inside_sphere");
	//asset load test
	//luna::render::MeshAsset* asset_check = luna::sAssetModule->LoadAsset<luna::render::MeshAsset>("C:/LunaEngine1.1/Lunar-GameEngine/assets/built-in/inside_sphere.lmesh");
	return 0;
}
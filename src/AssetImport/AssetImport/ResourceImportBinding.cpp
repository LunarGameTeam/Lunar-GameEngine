#pragma once
#include "AssetImport/FbxParser/FbxImporterFunc.h"

#include "AssetImport/ObjParser/ObjImporterFunc.h"

#include "AssetImport/GltfParser/GltfImporterFunc.h"

#include"AssetImport/ResourceImportBinding.h"
#include"AssetImport/MeshAssetImport.h"
namespace luna
{

	void ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	)
	{
		luna::asset::LImportScene importScene;
		bool resLoaded = false;
		if (fileExtension == ".obj" || fileExtension == ".OBJ")
		{
			resLoaded = true;
			luna::asset::ImportObjToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}
		if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
			luna::asset::ImportFbxToLunaResource(filePath, importScene);
			importScene.ConvertDataAxisAndUnit(luna::asset::LImportAxisType::ImportAxisYupLeftHand, luna::asset::LImportUnitType::ImportUnitMeter);
			importScene.PostProcessData();
		}
		if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
			luna::asset::ImportGltfToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}

		luna::asset::LSingleMeshAndAnimationAssetImport commonAssetImporter;
		luna::asset::LAssetPack assetPackData;
		commonAssetImporter.ParsingImportScene(resSavePath,importScene, assetPackData);
		assetPackData.SerializeAllAsset(resSavePath, fileName);
	}

	void ImportFileToLunaSceneResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension,
		asset::LSceneAssetNodeMessage* nodeMessage
	)
	{
		luna::asset::LImportScene importScene;
		bool resLoaded = false;
		if (fileExtension == ".obj" || fileExtension == ".OBJ")
		{
			resLoaded = true;
			luna::asset::ImportObjToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}
		if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
			luna::asset::ImportFbxToLunaResource(filePath, importScene);
			importScene.ConvertDataAxisAndUnit(luna::asset::LImportAxisType::ImportAxisYupLeftHand, luna::asset::LImportUnitType::ImportUnitMeter);
			importScene.PostProcessData();
		}
		if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
			luna::asset::ImportGltfToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}

		luna::asset::LSceneAssetImport commonAssetImporter;
		luna::asset::LAssetPack assetPackData;
		commonAssetImporter.ParsingImportScene(resSavePath, importScene, assetPackData, nodeMessage);
		LString pathGlobel = sPlatformModule->GetProjectDir() + resSavePath;
		assetPackData.SerializeAllAssetToDemoScene(pathGlobel);
	}
	STATIC_INIT(ResourceImport)
	{
		BindingModule* resourceParseModule = BindingModule::Get("luna.editor");
		resourceParseModule->AddMethod<&ImportFileToLunaResource>("import_mesh");
		resourceParseModule->AddMethod<&ImportFileToLunaSceneResource>("import_scene_res");
		resourceParseModule->Init();
	};
}
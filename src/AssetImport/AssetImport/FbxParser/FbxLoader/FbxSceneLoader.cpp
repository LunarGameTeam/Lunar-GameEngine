#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxSceneLoader.h"
#include "AssetImport/FbxParser/FbxLoader/FbxMeshLoader.h"
#include "AssetImport/FbxParser/FbxLoader/FbxSkeletonLoader.h"
#include "AssetImport/FbxParser/FbxLoader/FbxAnimationLoader.h"
#include "AssetImport/FbxParser/FbxLoader/FbxMaterialLoader.h"
#include "AssetImport/FbxParser/FbxLoader/FbxCameraLoader.h"
namespace luna::lfbx
{
	LFbxLoaderFactory::LFbxLoaderFactory()
	{
		mLoaders.insert(std::pair<LFbxDataType, std::shared_ptr<LFbxLoaderBase>>(LFbxDataType::FbxMeshData, std::make_shared<LFbxLoaderMesh>()));
	}

	std::shared_ptr<LFbxDataBase> LFbxLoaderFactory::LoadFbxData(LFbxDataType type, const LArray<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager)
	{
		auto needLoader = mLoaders.find(type);
		if (needLoader == mLoaders.end())
		{
			return nullptr;
		}
		return needLoader->second->ParsingData(sceneNodes, pNode, pManager);
	}

	void LFbxLoaderHelper::LoadFbxFile(const char* pFilename, LFbxSceneData& sceneOut)
	{
		InitializeSdkObjects(lSdkManager, lScene);
		LoadScene(lSdkManager, lScene, pFilename);
		InitSceneMessage(sceneOut);
		auto root_node = lScene->GetRootNode();
		std::unordered_map<LFbxDataType, LArray<NodeDataPack>> nodeData;
		ProcessNode(size_t(-1),root_node, sceneOut, nodeData);
		for (auto& eachDataGroup : nodeData)
		{
			for (auto &eachData : eachDataGroup.second)
			{
				std::shared_ptr<LFbxDataBase> newImportData = singleLoaderInterface->LoadFbxData(eachDataGroup.first, sceneOut.mNodes, eachData.pNode, lSdkManager);
				if (newImportData != nullptr)
				{
					sceneOut.mNodes[eachData.nodeIndex].mNodeData.insert(std::pair<LFbxDataType, size_t>(eachDataGroup.first, sceneOut.mDatas.size()));
					newImportData->SetNodeIndex(eachData.nodeIndex);
					sceneOut.mDatas.push_back(newImportData);
				}
			}
		}
		DestroySdkObjects(lSdkManager, true);
	}

	void LFbxLoaderHelper::DestroySdkObjects(fbxsdk::FbxManager* pManager, bool pExitStatus)
	{
		//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) LFbxImportLog::GetInstance()->AddLog("Program Success!\n");
	}

	void LFbxLoaderHelper::InitializeSdkObjects(fbxsdk::FbxManager*& pManager, FbxScene*& pScene)
	{
		//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
		pManager = fbxsdk::FbxManager::Create();
		if (!pManager)
		{
			LFbxImportLog::GetInstance()->AddLog("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else LFbxImportLog::GetInstance()->AddLog("Autodesk FBX SDK version %s\n", pManager->GetVersion());

		//Create an IOSettings object. This object holds all import/export settings.
		
		fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(ios);

		//Load plugins from the executable directory (optional)
		//fbxsdk::FbxString lPath = FbxGetApplicationDirectory();
		//pManager->LoadPluginsDirectory(lPath.Buffer());

		//Create an FBX scene. This object holds most objects imported/exported from/to files.
		pScene = fbxsdk::FbxScene::Create(pManager, "My Scene");
		if (!pScene)
		{
			LFbxImportLog::GetInstance()->AddLog("Error: Unable to create FBX scene!\n");
			exit(1);
		}
	}
#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif
	bool LFbxLoaderHelper::LoadScene(fbxsdk::FbxManager* pManager, fbxsdk::FbxDocument* pScene, const char* pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		//int lFileFormat = -1;
		int lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		fbxsdk::FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(pManager, "");

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if (!lImportStatus)
		{
			fbxsdk::FbxString error = lImporter->GetStatus().GetErrorString();
			LFbxImportLog::GetInstance()->AddLog("Call to FbxImporter::Initialize() failed.\n");
			LFbxImportLog::GetInstance()->AddLog("Error returned: %s\n\n", error.Buffer());

			if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				LFbxImportLog::GetInstance()->AddLog("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				LFbxImportLog::GetInstance()->AddLog("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		LFbxImportLog::GetInstance()->AddLog("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

		if (lImporter->IsFBX())
		{
			LFbxImportLog::GetInstance()->AddLog("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.

			LFbxImportLog::GetInstance()->AddLog("Animation Stack Information\n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			LFbxImportLog::GetInstance()->AddLog("    Number of Animation Stacks: %d\n", lAnimStackCount);
			LFbxImportLog::GetInstance()->AddLog("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			LFbxImportLog::GetInstance()->AddLog("\n");

			for (int i = 0; i < lAnimStackCount; i++)
			{
				fbxsdk::FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				LFbxImportLog::GetInstance()->AddLog("    Animation Stack %d\n", i);
				LFbxImportLog::GetInstance()->AddLog("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
				LFbxImportLog::GetInstance()->AddLog("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				LFbxImportLog::GetInstance()->AddLog("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

				// Set the value of the import state to false if the animation stack should be not
				// be imported. 
				LFbxImportLog::GetInstance()->AddLog("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
				LFbxImportLog::GetInstance()->AddLog("\n");
			}

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
			IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
			IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
			IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
			IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
			IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
			IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);
		if (lStatus == false && lImporter->GetStatus() == fbxsdk::FbxStatus::ePasswordError)
		{
			LFbxImportLog::GetInstance()->AddLog("Please enter password: ");

			lPassword[0] = '\0';

			FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
				scanf("%s", lPassword);
			FBXSDK_CRT_SECURE_NO_WARNING_END

				fbxsdk::FbxString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(pScene);

			if (lStatus == false && lImporter->GetStatus() == fbxsdk::FbxStatus::ePasswordError)
			{
				LFbxImportLog::GetInstance()->AddLog("\nPassword is wrong, import aborted.\n");
			}
		}

		if (!lStatus || (lImporter->GetStatus() != fbxsdk::FbxStatus::eSuccess))
		{
			LFbxImportLog::GetInstance()->AddLog("********************************************************************************\n");
			if (lStatus)
			{
				LFbxImportLog::GetInstance()->AddLog("WARNING:\n");
				LFbxImportLog::GetInstance()->AddLog("   The importer was able to read the file but with errors.\n");
				LFbxImportLog::GetInstance()->AddLog("   Loaded scene may be incomplete.\n\n");
			}
			else
			{
				LFbxImportLog::GetInstance()->AddLog("Importer failed to load the file!\n\n");
			}

			if (lImporter->GetStatus() != fbxsdk::FbxStatus::eSuccess)
				LFbxImportLog::GetInstance()->AddLog("   Last error message: %s\n", lImporter->GetStatus().GetErrorString());

			fbxsdk::FbxArray<fbxsdk::FbxString*> history;
			lImporter->GetStatus().GetErrorStringHistory(history);
			if (history.GetCount() > 1)
			{
				LFbxImportLog::GetInstance()->AddLog("   Error history stack:\n");
				for (int i = 0; i < history.GetCount(); i++)
				{
					LFbxImportLog::GetInstance()->AddLog("      %s\n", history[i]->Buffer());
				}
			}
			fbxsdk::FbxArrayDelete<fbxsdk::FbxString*>(history);
			LFbxImportLog::GetInstance()->AddLog("********************************************************************************\n");
		}

		// Destroy the importer.
		lImporter->Destroy();

		return lStatus;
	}

	void LFbxLoaderHelper::InitSceneMessage(LFbxSceneData& sceneOut)
	{
		//查询制作fbx文件的软件的坐标系及单位信息
		int softwareAxisType = lScene->GetGlobalSettings().GetOriginalUpAxis();
		fbxsdk::FbxSystemUnit softwareUnit = lScene->GetGlobalSettings().GetOriginalSystemUnit();
		fbxsdk::FbxAxisSystem softwareAxis = lScene->GetGlobalSettings().GetAxisSystem();
		switch (softwareAxisType)
		{
		//fbx制作软件的默认坐标系只有两种，即MayaYUp和MayaZUp，其中MayaZUp为3dsmax的默认坐标系。其他软件导出的fbx坐标轴暂时先不管
		case 1:
		{
			softwareAxis = fbxsdk::FbxAxisSystem::MayaYUp;
		}
		case 2:
		{
			softwareAxis = fbxsdk::FbxAxisSystem::MayaZUp;
		}
		default:
			break;
		}
		//fbx导入时，不能让场景使用fbx文件的坐标轴，因为目前已知的部分软件(max,maya,ue)，都无法正确的把坐标系从自己软件的坐标轴转换成fbx文件指定的坐标轴(只能修改根节点)
		sceneOut.mAxis = softwareAxis;
		sceneOut.mUnit = softwareUnit;
		//查询fbx文件的坐标系及单位信息
		fbxsdk::FbxAxisSystem fileAxis = lScene->GetGlobalSettings().GetAxisSystem();
		fbxsdk::FbxSystemUnit fileUnit = lScene->GetGlobalSettings().GetSystemUnit();
		//恢复fbx原始软件的坐标系与单位
		if (softwareAxis != fileAxis)
		{
			softwareAxis.ConvertScene(lScene);
		}
		if (softwareUnit != fileUnit)
		{
			softwareUnit.ConvertScene(lScene);
		}
	}

	void LFbxLoaderHelper::ComputeLclTransform(
		fbxsdk::FbxNode* pNode,
		FbxDouble3& lclPosition,
		FbxDouble3& lclRotation,
		FbxDouble3& lclScale,
		fbxsdk::FbxAMatrix& globalTransform
	)
	{
		lclPosition = pNode->LclTranslation;
		lclRotation = pNode->LclRotation;
		lclScale = pNode->LclScaling;

		globalTransform = lScene->GetAnimationEvaluator()->GetNodeGlobalTransform(pNode);
	}

	void LFbxLoaderHelper::ProcessNode(
		size_t nodeParent,
		fbxsdk::FbxNode* pNode,
		LFbxSceneData& scene_out,
		std::unordered_map<LFbxDataType, LArray<NodeDataPack>>& nodeData
	)
	{
		size_t node_index = scene_out.mNodes.size();
		LFbxNodeBase &newNode = scene_out.mNodes.emplace_back();
		newNode.mName = pNode->GetName();
		newNode.mIndex = node_index;
		newNode.mParent = nodeParent;
		ComputeLclTransform(pNode,newNode.mLocalTranslation, newNode.mLocalRotation, newNode.mLocalScaling, newNode.mGlobelTransform);
		if (pNode->GetNodeAttribute())
		{
			switch (pNode->GetNodeAttribute()->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
			{
				NodeDataPack newDataPack;
				newDataPack.nodeIndex = node_index;
				newDataPack.pNode = pNode;
				nodeData[LFbxDataType::FbxMeshData].push_back(newDataPack);
				//nodeData[LFbxDataType::FbxMaterialData].push_back(newDataPack);
			}
			break;
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
		{
			size_t nextId = scene_out.mNodes.size();
			scene_out.mNodes[newNode.mIndex].mChild.push_back(nextId);
			ProcessNode(newNode.mIndex,pNode->GetChild(i), scene_out, nodeData);
		}
	}


}
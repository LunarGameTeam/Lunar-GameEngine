#define FBX_EXPORT
#include "lib/fbx_helper/fbx_importer.h"
namespace luna
{
	void LFbxImporterHelper::LoadFbxFile(const char* pFilename, LFbxSceneData& scene_out)
	{
		InitializeSdkObjects(lSdkManager, lScene);
		LoadScene(lSdkManager, lScene, pFilename);
		InitSceneMessage();
		auto root_node = lScene->GetRootNode();
		ProcessNode(root_node, scene_out);
		DestroySdkObjects(lSdkManager, true);
	}

	void LFbxImporterHelper::DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
	{
		//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
		if (pManager) pManager->Destroy();
		if (pExitStatus) FBXSDK_printf("Program Success!\n");
	}

	void LFbxImporterHelper::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
	{
		//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
		pManager = FbxManager::Create();
		if (!pManager)
		{
			FBXSDK_printf("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

		//Create an IOSettings object. This object holds all import/export settings.
		FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
		pManager->SetIOSettings(ios);

		//Load plugins from the executable directory (optional)
		FbxString lPath = FbxGetApplicationDirectory();
		pManager->LoadPluginsDirectory(lPath.Buffer());

		//Create an FBX scene. This object holds most objects imported/exported from/to files.
		pScene = FbxScene::Create(pManager, "My Scene");
		if (!pScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}
	}
#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif
	bool LFbxImporterHelper::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor, lSDKMinor, lSDKRevision;
		//int lFileFormat = -1;
		int lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(pManager, "");

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if (!lImportStatus)
		{
			FbxString error = lImporter->GetStatus().GetErrorString();
			FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
			FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

			if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

		if (lImporter->IsFBX())
		{
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.

			FBXSDK_printf("Animation Stack Information\n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
			FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			FBXSDK_printf("\n");

			for (int i = 0; i < lAnimStackCount; i++)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				FBXSDK_printf("    Animation Stack %d\n", i);
				FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
				FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

				// Set the value of the import state to false if the animation stack should be not
				// be imported. 
				FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
				FBXSDK_printf("\n");
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
		if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("Please enter password: ");

			lPassword[0] = '\0';

			FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
				scanf("%s", lPassword);
			FBXSDK_CRT_SECURE_NO_WARNING_END

				FbxString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(pScene);

			if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
			{
				FBXSDK_printf("\nPassword is wrong, import aborted.\n");
			}
		}

		if (!lStatus || (lImporter->GetStatus() != FbxStatus::eSuccess))
		{
			FBXSDK_printf("********************************************************************************\n");
			if (lStatus)
			{
				FBXSDK_printf("WARNING:\n");
				FBXSDK_printf("   The importer was able to read the file but with errors.\n");
				FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
			}
			else
			{
				FBXSDK_printf("Importer failed to load the file!\n\n");
			}

			if (lImporter->GetStatus() != FbxStatus::eSuccess)
				FBXSDK_printf("   Last error message: %s\n", lImporter->GetStatus().GetErrorString());

			FbxArray<FbxString*> history;
			lImporter->GetStatus().GetErrorStringHistory(history);
			if (history.GetCount() > 1)
			{
				FBXSDK_printf("   Error history stack:\n");
				for (int i = 0; i < history.GetCount(); i++)
				{
					FBXSDK_printf("      %s\n", history[i]->Buffer());
				}
			}
			FbxArrayDelete<FbxString*>(history);
			FBXSDK_printf("********************************************************************************\n");
		}

		// Destroy the importer.
		lImporter->Destroy();

		return lStatus;
	}

	void LFbxImporterHelper::InitSceneMessage(bool change_unit)
	{
		//查询fbx的坐标系及单位信息
		auto SoftwareAxisSystem = lScene->GetGlobalSettings().GetOriginalUpAxis();
		FbxSystemUnit SoftwareUnitSystem = lScene->GetGlobalSettings().GetOriginalSystemUnit();
		FbxSystemUnit FileUnitSystem = lScene->GetGlobalSettings().GetSystemUnit();
		//恢复fbx原始软件的坐标系
		switch (SoftwareAxisSystem)
		{
		case 1:
		{
			auto standered_axis = FbxAxisSystem::MayaYUp;
			standered_axis.ConvertScene(lScene);
		}
		case 2:
		{
			auto standered_axis = FbxAxisSystem::MayaZUp;
			standered_axis.ConvertScene(lScene);
		}
		default:
			break;
		}
		//恢复fbx原始软件的单位
		if (SoftwareUnitSystem != FileUnitSystem)
		{
			SoftwareUnitSystem.ConvertScene(lScene);
		}
	}

	void LFbxImporterHelper::ReadVertex(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, LVector3f* pVertex)
	{
		FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
		FbxVector4 now_point = globel_matrix.MultT(pCtrlPoint[ctrlPointIndex]);
		pVertex->x() = static_cast<float>(now_point[0]);
		pVertex->y() = static_cast<float>(now_point[1]);
		pVertex->z() = static_cast<float>(now_point[2]);
	}

	void LFbxImporterHelper::ReadColor(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, LVector4f* pColor)
	{
		if (pMesh->GetElementVertexColorCount() < 1)
		{
			return;
		}
		FbxGeometryElementVertexColor* pVertexColor = pMesh->GetElementVertexColor(0);
		switch (pVertexColor->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (pVertexColor->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				pColor->x() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mRed);
				pColor->y() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen);
				pColor->z() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue);
				pColor->w() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mAlpha);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = pVertexColor->GetIndexArray().GetAt(ctrlPointIndex);
				pColor->x() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mRed);
				pColor->y() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mGreen);
				pColor->z() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mBlue);
				pColor->w() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mAlpha);
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (pVertexColor->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				pColor->x() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(vertexCounter).mRed);
				pColor->y() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(vertexCounter).mGreen);
				pColor->z() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(vertexCounter).mBlue);
				pColor->w() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(vertexCounter).mAlpha);
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = pVertexColor->GetIndexArray().GetAt(vertexCounter);
				pColor->x() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mRed);
				pColor->y() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mGreen);
				pColor->z() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mBlue);
				pColor->w() = static_cast<float>(pVertexColor->GetDirectArray().GetAt(id).mAlpha);
			}
			break;
			default:
				break;
			}
		}
		break;
		}
	}

	void LFbxImporterHelper::ReadUV(FbxMesh* pMesh, int ctrlPointIndex, int textureUVIndex, int uvLayer, LVector2f* pUV)
	{
		if (uvLayer >= 4 || pMesh->GetElementUVCount() <= uvLayer)
		{
			return;
		}

		FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(uvLayer);

		switch (pVertexUV->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (pVertexUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				pUV->x() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[0]);
				pUV->y() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[1]);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = pVertexUV->GetIndexArray().GetAt(ctrlPointIndex);
				pUV->x() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(id)[0]);
				pUV->y() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(id)[1]);
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (pVertexUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			case FbxGeometryElement::eIndexToDirect:
			{
				pUV->x() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(textureUVIndex)[0]);
				pUV->y() = static_cast<float>(pVertexUV->GetDirectArray().GetAt(textureUVIndex)[1]);
			}
			break;

			default:
				break;
			}
		}
		break;
		}
	}

	void LFbxImporterHelper::ReadNormal(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, LVector3f* pNormal)
	{
		if (pMesh->GetElementNormalCount() < 1)
		{
			return;
		}
		FbxVector4 now_normal;
		FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);
		switch (leNormal->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (leNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				now_normal = leNormal->GetDirectArray().GetAt(ctrlPointIndex);
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = leNormal->GetIndexArray().GetAt(ctrlPointIndex);
				now_normal = leNormal->GetDirectArray().GetAt(id);
			}
			break;

			default:
				break;
			}
		}
		break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (leNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				now_normal = leNormal->GetDirectArray().GetAt(vertexCounter);
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = leNormal->GetIndexArray().GetAt(vertexCounter);
				now_normal = leNormal->GetDirectArray().GetAt(id);
			}
			break;
			default:
				break;
			}
		}
		break;
		}
		now_normal = globel_matrix.MultT(now_normal);
		pNormal->x() = static_cast<float>(now_normal[0]);
		pNormal->y() = static_cast<float>(now_normal[1]);
		pNormal->z() = static_cast<float>(now_normal[2]);
	}

	void LFbxImporterHelper::ReadTangent(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, int vertecCounter, LVector4f* pTangent)
	{
		if (pMesh->GetElementTangentCount() < 1)
		{
			return;
		}
		FbxVector4 now_tangent;
		FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(0);

		switch (leTangent->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (leTangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				now_tangent = leTangent->GetDirectArray().GetAt(ctrlPointIndex);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = leTangent->GetIndexArray().GetAt(ctrlPointIndex);
				now_tangent = leTangent->GetDirectArray().GetAt(id);
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (leTangent->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				now_tangent = leTangent->GetDirectArray().GetAt(vertecCounter);
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = leTangent->GetIndexArray().GetAt(vertecCounter);
				now_tangent = leTangent->GetDirectArray().GetAt(id);
			}
			break;

			default:
				break;
			}
		}
		break;
		}
		now_tangent = globel_matrix.MultT(now_tangent);
		pTangent->x() = static_cast<float>(now_tangent[0]);
		pTangent->y() = static_cast<float>(now_tangent[1]);
		pTangent->z() = static_cast<float>(now_tangent[2]);
		pTangent->w() = 1.0f;
		//todo:计算切线朝向
		/*
		TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
		TempValue = TotalMatrixForNormal.MultT(TempValue);
		FVector TangentY = -FFbxConvert::ConvertDir(TempValue);
		VertexInstanceBinormalSigns[AddedVertexInstanceId] = FbxGetBasisDeterminantSign(TangentX.GetSafeNormal(), TangentY.GetSafeNormal(), TangentZ.GetSafeNormal());
		*/
	}

	void LFbxImporterHelper::ReadMaterialIndex(FbxMesh* pMesh, int faceIndex, uint32_t* mat_id)
	{
		auto& pMaterialIndices = pMesh->GetElementMaterial()->GetIndexArray();
		auto materialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
		*mat_id = pMaterialIndices.GetAt(faceIndex);
	}

	LString LFbxImporterHelper::GetFbxObjectName(const FbxObject* Object)
	{
		if (!Object)
		{
			return LString();
		}
		LString ObjName = Object->GetName();
		return ObjName;
	}

	FbxAMatrix LFbxImporterHelper::ComputeNodeMatrix(FbxNode* Node)
	{
		FbxAMatrix Geometry;
		FbxVector4 Translation, Rotation, Scaling;
		Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
		Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
		Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
		Geometry.SetT(Translation);
		Geometry.SetR(Rotation);
		Geometry.SetS(Scaling);
		//For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
		FbxAMatrix& GlobalTransform = lScene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);
		//We must always add the geometric transform. Only Max use the geometric transform which is an offset to the local transform of the node
		FbxAMatrix NodeMatrix = GlobalTransform * Geometry;
		return NodeMatrix;
	}

	void LFbxImporterHelper::ProcessMesh(FbxNode* pNode, FBXMeshNode& new_mesh)
	{
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			return;
		}
		//计算当前节点的全局变换矩阵
		FbxAMatrix TotalMatrix;
		FbxAMatrix TotalMatrixForNormal;
		TotalMatrix = ComputeNodeMatrix(pNode);
		TotalMatrixForNormal = TotalMatrix.Inverse();
		TotalMatrixForNormal = TotalMatrixForNormal.Transpose();
		//记录所有材质的名字
		LString mesh_name = GetFbxObjectName(pNode);
		new_mesh.submesh_name = mesh_name;
		if (pMesh->GetElementMaterial())
		{
			const int32_t MaterialCount = pNode->GetMaterialCount();
			for (int32_t MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
			{
				FbxSurfaceMaterial* FbxMaterial = pNode->GetMaterial(MaterialIndex);
				new_mesh.material_names.push_back(GetFbxObjectName(FbxMaterial));
			}
			auto materialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
			if (materialMappingMode == FbxGeometryElement::eAllSame)
			{
				new_mesh.if_divide_by_mat = false;
			}
			else
			{
				new_mesh.if_divide_by_mat = true;
			}
		}
		else
		{
			new_mesh.material_names.push_back("mat_" + mesh_name);
			new_mesh.if_divide_by_mat = false;
		}
		//转换mesh的几何信息(三角化，平滑组)
		FbxGeometryConverter SDKGeometryConverter(lSdkManager);
		int32_t LayerSmoothingCount = pMesh->GetLayerCount(FbxLayerElement::eSmoothing);
		for (int32_t i = 0; i < LayerSmoothingCount; i++)
		{
			FbxLayerElementSmoothing const* SmoothingInfo = pMesh->GetLayer(0)->GetSmoothing();
			if (SmoothingInfo && SmoothingInfo->GetMappingMode() != FbxLayerElement::eByPolygon)
			{
				SDKGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(pMesh, i);
			}
		}
		if (!pMesh->IsTriangleMesh())
		{
			const bool bReplace = true;
			FbxNodeAttribute* ConvertedNode = SDKGeometryConverter.Triangulate(pMesh, bReplace);
			if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				pMesh = (fbxsdk::FbxMesh*)ConvertedNode;
			}
			else
			{
				//TODO add an error message
				//AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, FText::Format(LOCTEXT("Error_FailedToTriangulate", "Unable to triangulate mesh '{0}'"), FText::FromString(Mesh->GetName()))), FFbxErrors::Generic_Mesh_TriangulationFailed);
				return; // not clean, missing some dealloc
			}
		}
		//加载所有的面片信息
		int triangleCount = pMesh->GetPolygonCount();
		int vertexCounter = 0;
		new_mesh.mesh_face_data.resize(triangleCount);
		int32_t uv_count = 0;
		for (int i = 0; i < triangleCount; ++i)
		{
			FBXMeshFace& new_value = new_mesh.mesh_face_data[i];
			if (new_mesh.if_divide_by_mat)
			{
				ReadMaterialIndex(pMesh, i, &new_value.mat_id);
			}
			int face_verte_size = pMesh->GetPolygonSize(i);
			new_value.FBXMeshVertex.resize(face_verte_size);
			new_value.FBXMeshVertexIndex.resize(face_verte_size);
			for (int j = 0; j < face_verte_size; j++)
			{
				render::BaseVertex& per_vert = new_value.FBXMeshVertex[j];
				uint32_t& vert_id = new_value.FBXMeshVertexIndex[j];
				vert_id = pMesh->GetPolygonVertex(i, j);
				ReadVertex(TotalMatrix, pMesh, vert_id, &per_vert.pos);
				ReadColor(pMesh, vert_id, vertexCounter, &per_vert.color);
				for (int k = 0; k < 4; ++k)
				{
					per_vert.uv[k].x() = 0;
					per_vert.uv[k].y() = 0;
				}
				int uv_channel_count = pMesh->GetElementUVCount();
				for (int k = 0; k < uv_channel_count; ++k)
				{
					//uv通道的id需要从名字里获取
					FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(k);
					LString uv_channel_name = pVertexUV->GetName();
					int channel_id = k;
					if (uv_channel_name.Find("UVChannel_") != std::string::npos)
					{
						LString channel_id_str = uv_channel_name.Substr(10, uv_channel_name.Length() - 10);
						channel_id = atoi(channel_id_str.c_str()) - 1;
					}
					if (channel_id < 4)
					{
						ReadUV(pMesh, vert_id, pMesh->GetTextureUVIndex(i, j), channel_id, &per_vert.uv[channel_id]);
						if (uv_count < channel_id + 1)
						{
							uv_count = channel_id + 1;
						}
					}
				}
				ReadNormal(TotalMatrixForNormal, pMesh, vert_id, vertexCounter, &per_vert.normal);
				ReadTangent(TotalMatrixForNormal, pMesh, vert_id, vertexCounter, &per_vert.tangent);
				vertexCounter++;
			}
		}
		new_mesh.uv_channel_size = uv_count;
	}

	void LFbxImporterHelper::ProcessNode(FbxNode* pNode, LFbxSceneData& scene_out)
	{
		if (pNode->GetNodeAttribute())
		{
			switch (pNode->GetNodeAttribute()->GetAttributeType())
			{
			case FbxNodeAttribute::eMesh:
			{
				FBXMeshNode new_mesh;
				ProcessMesh(pNode, new_mesh);
				scene_out.mesh_data.push_back(new_mesh);
			}

			break;
			}
		}
		for (int i = 0; i < pNode->GetChildCount(); ++i)
		{
			ProcessNode(pNode->GetChild(i), scene_out);
		}
	}

	void LFbxMeshAssetHelper::ExchangeFbxSceneToLunaMesh(const LFbxSceneData& scene_in, render::MeshAsset* m_mesh)
	{
		LVector<SimpleMesh> all_mesh;
		for (auto& each_mesh_node : scene_in.mesh_data)
		{
			if (each_mesh_node.if_divide_by_mat)
			{
				LVector<FBXMeshNode> mesh_node_out;
				DivideFbxMesh(each_mesh_node, mesh_node_out);
				for (auto& each_mesh_node_divide : mesh_node_out)
				{
					SimpleMesh simple_mesh;
					simple_mesh.material_using = each_mesh_node_divide.material_names[0];
					simple_mesh.submesh_name = each_mesh_node_divide.submesh_name;
					ProfileFbxMesh(each_mesh_node_divide, simple_mesh);
					PostProcessingMesh(simple_mesh);
					all_mesh.push_back(simple_mesh);
				}
			}
			else
			{
				SimpleMesh simple_mesh;
				simple_mesh.material_using = each_mesh_node.material_names[0];
				simple_mesh.submesh_name = each_mesh_node.submesh_name;
				ProfileFbxMesh(each_mesh_node, simple_mesh);
				PostProcessingMesh(simple_mesh);
				all_mesh.push_back(simple_mesh);
			}
		}
		CombineLunaMesh(all_mesh, m_mesh);
	}

	void LFbxMeshAssetHelper::DivideFbxMesh(const FBXMeshNode& mesh_node_in, LVector<FBXMeshNode>& mesh_node_out)
	{
		int32_t mat_size = mesh_node_in.material_names.size();
		mesh_node_out.resize(mat_size);
		for (int32_t mesh_id = 0; mesh_id < mat_size; ++mesh_id)
		{
			auto& each_mesh = mesh_node_out[mesh_id];
			each_mesh.if_divide_by_mat = false;
			each_mesh.uv_channel_size = mesh_node_in.uv_channel_size;
			each_mesh.material_names.push_back(mesh_node_in.material_names[mesh_id]);
			each_mesh.submesh_name = mesh_node_in.submesh_name;
		}
		for (auto& each_face : mesh_node_in.mesh_face_data)
		{
			uint32_t mesh_id = each_face.mat_id;
			auto new_face = each_face;
			new_face.mat_id = 0;
			mesh_node_out[mesh_id].mesh_face_data.push_back(new_face);
		}
		for (auto each_mesh = mesh_node_out.begin(); each_mesh != mesh_node_out.end();)
		{
			if (each_mesh->mesh_face_data.size() == 0)
			{
				each_mesh = mesh_node_out.erase(each_mesh);
			}
			else
			{
				each_mesh++;
			}
		}
	}

	bool LFbxMeshAssetHelper::IfEqual(const LVector2f& in_a, const LVector2f& in_b)
	{
		if (abs(in_a.x() - in_b.x()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.y() - in_b.y()) > kinda_small_number)
		{
			return false;
		}
		return true;
	}

	bool LFbxMeshAssetHelper::IfEqual(const LVector3f& in_a, const LVector3f& in_b)
	{
		if (abs(in_a.x() - in_b.x()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.y() - in_b.y()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.z() - in_b.z()) > kinda_small_number)
		{
			return false;
		}
		return true;
	}

	bool LFbxMeshAssetHelper::IfEqual(const LVector4f& in_a, const LVector4f& in_b)
	{
		if (abs(in_a.x() - in_b.x()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.y() - in_b.y()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.z() - in_b.z()) > kinda_small_number)
		{
			return false;
		}
		if (abs(in_a.w() - in_b.w()) > kinda_small_number)
		{
			return false;
		}
		return true;
	}

	bool LFbxMeshAssetHelper::IfEqual(
		const render::BaseVertex& in_a,
		const render::BaseVertex& in_b,
		const int32_t& uv_channel_size,
		const bool& if_color
	)
	{
		if (!IfEqual(in_a.normal, in_b.normal))
		{
			return false;
		}
		for (int i = 0; i < uv_channel_size; ++i)
		{
			if (!IfEqual(in_a.uv[i], in_b.uv[i]))
			{
				return false;
			}
		}
		if (if_color && (!IfEqual(in_a.color, in_b.color)))
		{
			return false;
		}
		return true;
	}

	void LFbxMeshAssetHelper::ProfileFbxMesh(const FBXMeshNode& mesh_node_in, SimpleMesh& simple_mesh)
	{
		LUnorderedMap<int32_t, LVector<uint32_t>> vertex_repeat_check;
		for (auto& each_triangle : mesh_node_in.mesh_face_data)
		{
			for (uint32_t vert_turn = 0; vert_turn < 3; ++vert_turn)
			{
				//当前面的每个顶点信息
				uint32_t control_point_index = each_triangle.FBXMeshVertexIndex[vert_turn];
				auto& compare_vert_data = each_triangle.FBXMeshVertex[vert_turn];
				//先检查当前的点是否已经被记录过了，已存在的点只需要加入索引
				bool if_have_same_vert = false;
				if (vertex_repeat_check.find(control_point_index) != vertex_repeat_check.end())
				{
					auto& vert_with_same_control = vertex_repeat_check[control_point_index];
					for (uint32_t vert_check_index : vert_with_same_control)
					{
						auto& vert_check_data = simple_mesh.vertex[vert_check_index];
						if (IfEqual(compare_vert_data, vert_check_data, mesh_node_in.uv_channel_size))
						{
							simple_mesh.indices.push_back(vert_check_index);
							if_have_same_vert = true;
							break;
						}
					}
				}
				//未被记录的点需要加入新的顶点
				if (!if_have_same_vert)
				{
					uint32_t real_point_size = simple_mesh.vertex.size();
					simple_mesh.vertex.push_back(compare_vert_data);
					simple_mesh.indices.push_back(real_point_size);
					vertex_repeat_check[control_point_index].push_back(real_point_size);
				}
			}
		}
	}

	void LFbxMeshAssetHelper::PostProcessingMesh(SimpleMesh& mesh_in)
	{
		//http://www.mikktspace.com/
		ComputeMikkTspace(mesh_in);
		//https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html
		ComputeVertexCache(mesh_in);
	}

	void LFbxMeshAssetHelper::CombineLunaMesh(LVector<SimpleMesh>& simple_mesh, render::MeshAsset* mesh_out)
	{
		for (int32_t i = 0; i < simple_mesh.size(); ++i)
		{
			render::SubMesh* sub_mesh = TCreateObject<render::SubMesh>();
			mesh_out->m_sub_meshes.PushBack(sub_mesh);
		}
		for (int32_t sub_index = 0; sub_index < simple_mesh.size(); ++sub_index)
		{
			auto& submesh_info = simple_mesh[sub_index];
			mesh_out->m_sub_meshes[sub_index]->vertex_size = submesh_info.vertex.size();
			mesh_out->m_sub_meshes[sub_index]->index_size = submesh_info.indices.size();
			mesh_out->m_sub_meshes[sub_index]->SetObjectName(submesh_info.submesh_name);
			std::swap(mesh_out->m_sub_meshes[sub_index]->vertices, submesh_info.vertex);
			std::swap(mesh_out->m_sub_meshes[sub_index]->indices, submesh_info.indices);
		}
	}

	static int MikkGetNumFaces(const SMikkTSpaceContext* Context)
	{
		SimpleMesh* UserData = (SimpleMesh*)(Context->m_pUserData);
		return UserData->indices.size() / 3;
	}

	static int MikkGetNumVertsOfFace(const SMikkTSpaceContext* Context, const int FaceIdx)
	{
		// All of our meshes are triangles.
		return 3;
	}

	static void MikkGetPosition(const SMikkTSpaceContext* Context, float Position[3], const int FaceIdx, const int VertIdx)
	{
		SimpleMesh* UserData = (SimpleMesh*)(Context->m_pUserData);
		const LVector3f& VertexPosition = UserData->vertex[UserData->indices[FaceIdx * 3 + VertIdx]].pos;
		Position[0] = VertexPosition.x();
		Position[1] = VertexPosition.y();
		Position[2] = VertexPosition.z();
	}

	static void MikkGetNormal(const SMikkTSpaceContext* Context, float Normal[3], const int FaceIdx, const int VertIdx)
	{
		SimpleMesh* UserData = (SimpleMesh*)(Context->m_pUserData);
		const LVector3f& VertexNormal = UserData->vertex[UserData->indices[FaceIdx * 3 + VertIdx]].normal;
		Normal[0] = VertexNormal.x();
		Normal[1] = VertexNormal.y();
		Normal[2] = VertexNormal.z();
	}

	static void MikkSetTSpaceBasic(const SMikkTSpaceContext* Context, const float Tangent[3], const float BitangentSign, const int FaceIdx, const int VertIdx)
	{
		SimpleMesh* UserData = (SimpleMesh*)(Context->m_pUserData);
		LVector4f& VertexTangent = UserData->vertex[UserData->indices[FaceIdx * 3 + VertIdx]].tangent;
		VertexTangent.x() = Tangent[0];
		VertexTangent.y() = Tangent[1];
		VertexTangent.z() = Tangent[2];
		VertexTangent.w() = BitangentSign;
	}

	static void MikkGetTexCoord(const SMikkTSpaceContext* Context, float UV[2], const int FaceIdx, const int VertIdx)
	{
		SimpleMesh* UserData = (SimpleMesh*)(Context->m_pUserData);
		const LVector2f& TexCoord = UserData->vertex[UserData->indices[FaceIdx * 3 + VertIdx]].uv[0];
		UV[0] = TexCoord.x();
		UV[1] = TexCoord.y();
	}

	void LFbxMeshAssetHelper::ComputeMikkTspace(SimpleMesh& simple_mesh)
	{
		SMikkTSpaceInterface MikkTInterface;
		MikkTInterface.m_getNormal = MikkGetNormal;
		MikkTInterface.m_getNumFaces = MikkGetNumFaces;
		MikkTInterface.m_getNumVerticesOfFace = MikkGetNumVertsOfFace;
		MikkTInterface.m_getPosition = MikkGetPosition;
		MikkTInterface.m_getTexCoord = MikkGetTexCoord;
		MikkTInterface.m_setTSpaceBasic = MikkSetTSpaceBasic;
		MikkTInterface.m_setTSpace = nullptr;

		SMikkTSpaceContext MikkTContext;
		MikkTContext.m_pInterface = &MikkTInterface;
		MikkTContext.m_pUserData = (void*)(&simple_mesh);
		genTangSpaceDefault(&MikkTContext);
	}

	void LFbxMeshAssetHelper::ComputeVertexCache(SimpleMesh& simple_mesh)
	{
		int32_t original_index_size = simple_mesh.indices.size();
		int32_t original_vertex_size = simple_mesh.vertex.size();
		uint32_t* chunk_index_list = new uint32_t[original_index_size];
		//调用第三方库整理索引缓冲区的顺序
		Forsyth::OptimizeFaces(simple_mesh.indices.data(), original_index_size, simple_mesh.vertex.size(), chunk_index_list, (uint16_t)32);
		//记录旧的顶点顺序
		LVector<render::BaseVertex> original_vertex_list;
		original_vertex_list.resize(original_vertex_size);
		std::swap(original_vertex_list, simple_mesh.vertex);
		//创建一个记录新顶点顺序的索引表
		LVector<int32_t> cache_index_list;
		cache_index_list.resize(original_vertex_size);
		for (int32_t i = 0; i < original_vertex_size; ++i)
		{
			cache_index_list[i] = -1;
		}
		//调整顶点顺序
		int32_t NextAvailableIndex = 0;
		for (int32_t Index = 0; Index < original_index_size; Index++)
		{
			const int32_t original_index = chunk_index_list[Index];
			const int32_t cache_index = cache_index_list[original_index];
			if (cache_index == -1)
			{
				chunk_index_list[Index] = NextAvailableIndex;
				cache_index_list[original_index] = NextAvailableIndex;
				NextAvailableIndex++;
			}
			else
			{
				chunk_index_list[Index] = cache_index;
			}
			simple_mesh.vertex[chunk_index_list[Index]] = original_vertex_list[original_index];
		}
		for (int32_t Index = 0; Index < original_index_size; Index++)
		{
			simple_mesh.indices[Index] = chunk_index_list[Index];
		}
		delete[] chunk_index_list;
	}
}
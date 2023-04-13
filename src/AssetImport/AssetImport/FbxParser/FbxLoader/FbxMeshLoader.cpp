#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxMeshLoader.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	void LFbxDataMesh::AddControlPoint(const FbxVector4& point)
	{
		mControlPoints.push_back(point);
	}

	void LFbxDataMesh::SetNormalDataType(
		FbxLayerElement::EMappingMode mapMode,
		FbxLayerElement::EReferenceMode refMode,
		FbxLayerElementTemplate<FbxVector4>* layerData
	)
	{
		mNormalLayer.mMapMode = mapMode;
		mNormalLayer.mRefMode = refMode;
		mNormalLayer.mLayerMapRefData = layerData;
	}

	void LFbxDataMesh::AddNormal(const FbxVector4& normal)
	{
		mNormalLayer.mLayerData.push_back(normal);
	}

	void LFbxDataMesh::SetTangentDataType(
		FbxLayerElement::EMappingMode mapMode,
		FbxLayerElement::EReferenceMode refMode,
		FbxLayerElementTemplate<FbxVector4>* layerData
	)
	{
		mTangentLayer.mMapMode = mapMode;
		mTangentLayer.mRefMode = refMode;
		mTangentLayer.mLayerMapRefData = layerData;
	}

	void LFbxDataMesh::AddTangent(const FbxVector4& tangent)
	{
		mTangentLayer.mLayerData.push_back(tangent);
	}

	void LFbxDataMesh::SetColorDataType(
		FbxLayerElement::EMappingMode mapMode,
		FbxLayerElement::EReferenceMode refMode,
		FbxLayerElementTemplate<FbxColor>* layerData
	)
	{
		mColorLayer.mMapMode = mapMode;
		mColorLayer.mRefMode = refMode;
		mColorLayer.mLayerMapRefData = layerData;
	}

	void LFbxDataMesh::AddColor(const FbxColor& color)
	{
		mColorLayer.mLayerData.push_back(color);
	}

	void LFbxDataMesh::SetUvDataType(
		int32_t channel,
		FbxLayerElement::EMappingMode mapMode,
		FbxLayerElement::EReferenceMode refMode,
		FbxLayerElementTemplate<FbxVector2>* layerData
	)
	{
		while (channel >= mUvLayer.size())
		{
			mUvLayer.emplace_back();
		}
		mUvLayer[channel].mMapMode = mapMode;
		mUvLayer[channel].mRefMode = refMode;
		mUvLayer[channel].mLayerMapRefData = layerData;
	}

	void LFbxDataMesh::AddUv(const FbxVector2& uv, int32_t channel)
	{
		assert(channel < (int32_t)mUvLayer.size());
		mUvLayer[channel].mLayerData.push_back(uv);
	}

	void LFbxDataMesh::AddFace(const FbxFaceData& face_data)
	{
		mFace.push_back(face_data);
	}

	const FbxColor LFbxDataMesh::GetColorByIndex(const size_t vertIndex, const size_t polygenIndex) const
	{
		FbxColor newColor;
		newColor.mRed = 0;
		newColor.mGreen = 0;
		newColor.mBlue = 0;
		newColor.mAlpha = 0;
		if (mColorLayer.mLayerData.size() < 1)
		{
			return newColor;
		}
		switch (mColorLayer.mMapMode)
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (mColorLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newColor = mColorLayer.mLayerData[vertIndex];
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mColorLayer.mLayerMapRefData->GetIndexArray().GetAt(vertIndex);
				newColor = mColorLayer.mLayerData[id];
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (mColorLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newColor = mColorLayer.mLayerData[polygenIndex];
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mColorLayer.mLayerMapRefData->GetIndexArray().GetAt(polygenIndex);
				newColor = mColorLayer.mLayerData[id];
			}
			break;
			default:
				break;
			}
		}
		break;
		}
		return newColor;
	}

	const FbxVector4 LFbxDataMesh::GetNormalByIndex(const size_t vertIndex, const size_t polygenIndex) const
	{
		FbxVector4 newNormal;
		newNormal.mData[0] = 0;
		newNormal.mData[1] = 0;
		newNormal.mData[2] = 0;
		newNormal.mData[3] = 0;
		if (mNormalLayer.mLayerData.size() < 1)
		{
			return newNormal;
		}
		switch (mNormalLayer.mMapMode)
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (mNormalLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newNormal = mNormalLayer.mLayerData[vertIndex];
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mNormalLayer.mLayerMapRefData->GetIndexArray().GetAt(vertIndex);
				newNormal = mNormalLayer.mLayerData[id];
			}
			break;

			default:
				break;
			}
		}
		break;
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (mNormalLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newNormal = mNormalLayer.mLayerData[polygenIndex];
			}
			break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mNormalLayer.mLayerMapRefData->GetIndexArray().GetAt(polygenIndex);
				newNormal = mNormalLayer.mLayerData[id];
			}
			break;
			default:
				break;
			}
		}
		break;
		}
		return newNormal;
	}

	const FbxVector4 LFbxDataMesh::GetTangentByIndex(const size_t vertIndex, const size_t polygenIndex) const
	{
		FbxVector4 newTangent;
		newTangent.mData[0] = 0;
		newTangent.mData[1] = 0;
		newTangent.mData[2] = 0;
		newTangent.mData[3] = 0;
		if (mNormalLayer.mLayerData.size() < 1)
		{
			return newTangent;
		}

		switch (mTangentLayer.mMapMode)
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (mTangentLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newTangent = mTangentLayer.mLayerData[vertIndex];
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mTangentLayer.mLayerMapRefData->GetIndexArray().GetAt(vertIndex);
				newTangent = mTangentLayer.mLayerData[id];
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (mTangentLayer.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newTangent = mTangentLayer.mLayerData[polygenIndex];
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = mTangentLayer.mLayerMapRefData->GetIndexArray().GetAt(polygenIndex);
				newTangent = mTangentLayer.mLayerData[id];
			}
			break;

			default:
				break;
			}
		}
		break;
		}
		//todo:计算切线朝向
		/*
		TempValue = LayerElementBinormal->GetDirectArray().GetAt(BinormalValueIndex);
		TempValue = TotalMatrixForNormal.MultT(TempValue);
		FVector TangentY = -FFbxConvert::ConvertDir(TempValue);
		VertexInstanceBinormalSigns[AddedVertexInstanceId] = FbxGetBasisDeterminantSign(TangentX.GetSafeNormal(), TangentY.GetSafeNormal(), TangentZ.GetSafeNormal());
		*/
		return newTangent;
	}

	const FbxVector2 LFbxDataMesh::GetUvByIndex(const size_t vertIndex, const size_t polygenTextureIndex, const size_t uv_channel) const
	{
		FbxVector2 newUv;
		newUv.mData[0] = 0;
		newUv.mData[1] = 0;
		if (uv_channel >= mUvLayer.size())
		{
			return newUv;
		}
		const LFbxLayerData<FbxVector2>& pVertexUV = mUvLayer[uv_channel];
		switch (pVertexUV.mMapMode)
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (pVertexUV.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			{
				newUv = pVertexUV.mLayerData[vertIndex];
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				int id = pVertexUV.mLayerMapRefData->GetIndexArray().GetAt(vertIndex);
				newUv = pVertexUV.mLayerData[id];
			}
			break;

			default:
				break;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (pVertexUV.mRefMode)
			{
			case FbxGeometryElement::eDirect:
			case FbxGeometryElement::eIndexToDirect:
			{
				newUv = pVertexUV.mLayerData[polygenTextureIndex];
			}
			break;

			default:
				break;
			}
		}
		break;
		}
		return newUv;
	}

	int32_t LFbxDataMesh::TryAddBone(const LString& boneName)
	{
		auto existBone = mBindBoneNameRef.find(boneName);
		if (existBone != mBindBoneNameRef.end())
		{
			return existBone->second;
		}
		int32_t newIndex = mBindBoneName.size();
		mBindBoneName.push_back(boneName);
		FbxAMatrix emptyMat;
		emptyMat.SetIdentity();
		mPoseMatrix.push_back(emptyMat);
		mBindBoneControlData.push_back(FbxClusterControlPointData());
		mBindBoneNameRef.insert({ boneName,newIndex});
		return newIndex;
	}

	void LFbxDataMesh::SetBonePoseMatrix(int32_t BoneIndex, const FbxAMatrix& linkMatrix)
	{
		mPoseMatrix[BoneIndex] = linkMatrix;
	}

	void LFbxDataMesh::SetBoneControlData(int32_t BoneIndex, int32_t CtrlPointCount, int* pCtrlPointIndices, double* pCtrlPointWeights)
	{
		mBindBoneControlData[BoneIndex].mCtrlPointCount = CtrlPointCount;
		for(int32_t i = 0; i < CtrlPointCount; ++i)
		{
			mBindBoneControlData[BoneIndex].mIndices.push_back(pCtrlPointIndices[i]);
			mBindBoneControlData[BoneIndex].mWeights.push_back(pCtrlPointWeights[i]);
		}
	}

	void LFbxDataMesh::GenerateBindPoseToMesh(const FbxSkin* pSkin)
	{
		if (!pSkin)
		{
			return;
		}
		FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;
		int clusterCount = pSkin->GetClusterCount();
		// 处理当前Skin中的每个Cluster（对应到Skeleton）
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			const FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			//剔除不符合条件的skin cluster
			LString clusterUserId = pCluster->GetUserDataID();
			if (!pCluster || clusterUserId == "Maya_ClusterHint" || clusterUserId == "CompensationCluster")
			{
				continue;
			}
			//获取cluster链接的骨骼节点
			const FbxNode* pLinkNode = pCluster->GetLink();
			if (!pLinkNode)
			{
				continue;
			}
			LString skeletonBoneName = pLinkNode->GetName();
			//添加一个mesh引用的骨骼
			int32_t skeletonIndex = TryAddBone(skeletonBoneName);
			if (skeletonIndex < 0)
			{
				continue;
			}
			//得到每个Cluster里骨骼和mesh的坐标系变换
			FbxAMatrix transformMatrix, transformLinkMatrix;
			pCluster->GetTransformMatrix(transformMatrix);
			pCluster->GetTransformLinkMatrix(transformLinkMatrix);
			FbxAMatrix poseMat = transformLinkMatrix.Inverse() * transformMatrix;
			SetBonePoseMatrix(skeletonIndex, poseMat);

			//这里把每个骨骼影响到的顶点的蒙皮信息存储下来
			int32_t ctrlPointCount = pCluster->GetControlPointIndicesCount();
			int* pCtrlPointIndices = pCluster->GetControlPointIndices();
			double* pCtrlPointWeights = pCluster->GetControlPointWeights();
			SetBoneControlData(skeletonIndex, ctrlPointCount, pCtrlPointIndices, pCtrlPointWeights);
		}
	}

	LString LFbxDataMesh::GetRefBoneNameByIndex(int32_t BoneIndex) const
	{
		return mBindBoneName[BoneIndex];
	}

	FbxAMatrix LFbxDataMesh::GetBonePoseByIndex(int32_t BoneIndex) const
	{
		return mPoseMatrix[BoneIndex];
	}

	const FbxClusterControlPointData& LFbxDataMesh::GetBoneControlVertexDataByIndex(int32_t BoneIndex) const
	{
		return mBindBoneControlData[BoneIndex];
	}

	std::shared_ptr<LFbxDataBase> LFbxLoaderMesh::ParsingDataImpl(const LArray<LFbxNodeBase>& sceneNodes, FbxNode* pNode, LFbxLoadContext context)
	{
		std::shared_ptr<LFbxDataMesh> newMesh = std::make_shared<LFbxDataMesh>();
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			return nullptr;
		}
		//获取几何体的matrix
		FbxAMatrix Geometry;
		FbxVector4 Translation, Rotation, Scaling;
		Translation = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		Rotation = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		Scaling = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		Geometry.SetT(Translation);
		Geometry.SetR(Rotation);
		Geometry.SetS(Scaling);
		newMesh->SetMatrixGeometry(Geometry);
		//设置材质引用信息
		if (pMesh->GetElementMaterial())
		{
			newMesh->SetMaterialDataType(pMesh->GetElementMaterial()->GetMappingMode());
			newMesh->SetMaterialRefCount(pNode->GetMaterialCount());
		}
		//转换mesh的几何信息(三角化，平滑组)
		FbxGeometryConverter SDKGeometryConverter(context.lSdkManager);
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
				pMesh = (FbxMesh*)ConvertedNode;
			}
			else
			{
				LFbxImportLog::GetInstance()->AddLog("FBX face change to triangle face error");
				return nullptr;
			}
		}
		//拷贝控制点
		FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
		int allControlPointCount = pMesh->GetControlPointsCount();
		for (int32_t i = 0; i < allControlPointCount; ++i)
		{
			newMesh->AddControlPoint(pCtrlPoint[i]);
		}
		//检查是否有蒙皮或者变形信息
		int deformerCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < deformerCount; ++i)
		{
			const FbxSkin* pFBXSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
			if (pFBXSkin == nullptr)
			{
				continue;
			}
			newMesh->GenerateBindPoseToMesh(pFBXSkin);
		}

		//拷贝法线
		FbxGeometryElementNormal* leNormal = nullptr;
		if (pMesh->GetElementNormalCount() >= 1)
		{
			leNormal = pMesh->GetElementNormal(0);
		}
		if (leNormal != nullptr)
		{
			newMesh->SetNormalDataType(leNormal->GetMappingMode(), leNormal->GetReferenceMode(), leNormal);
			FbxLayerElementArrayTemplate<FbxVector4> &normalArray = leNormal->GetDirectArray();
			for (int32_t i = 0; i < normalArray.GetCount(); ++i)
			{
				newMesh->AddNormal(normalArray[i]);
			}
		}
		//拷贝切线
		FbxGeometryElementTangent* leTangent = nullptr;
		if (pMesh->GetElementTangentCount() >= 1)
		{
			leTangent = pMesh->GetElementTangent(0);
		}
		if (leTangent != nullptr)
		{
			newMesh->SetTangentDataType(leTangent->GetMappingMode(), leTangent->GetReferenceMode(), leTangent);
			FbxLayerElementArrayTemplate<FbxVector4> &tangentArray = leTangent->GetDirectArray();
			for (int32_t i = 0; i < tangentArray.GetCount(); ++i)
			{
				newMesh->AddTangent(tangentArray[i]);
			}
		}
		//拷贝顶点色
		FbxGeometryElementVertexColor* leColor = nullptr;
		if (pMesh->GetElementVertexColorCount() >= 1)
		{
			leColor = pMesh->GetElementVertexColor(0);
		}
		if (leColor != nullptr)
		{
			newMesh->SetColorDataType(leColor->GetMappingMode(), leColor->GetReferenceMode(), leColor);
			FbxLayerElementArrayTemplate<FbxColor> &colorArray = leColor->GetDirectArray();
			for (int32_t i = 0; i < colorArray.GetCount(); ++i)
			{
				newMesh->AddColor(colorArray[i]);
			}
		}
		//拷贝uv
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
			newMesh->SetUvDataType(channel_id,pVertexUV->GetMappingMode(), pVertexUV->GetReferenceMode(), pVertexUV);
			FbxLayerElementArrayTemplate<FbxVector2> &uvArray = pVertexUV->GetDirectArray();
			for (int32_t i = 0; i < uvArray.GetCount(); ++i)
			{
				newMesh->AddUv(uvArray[i],k);
			}
		}

		//加载所有的面片信息
		int triangleCount = pMesh->GetPolygonCount();
		int32_t uv_count = 0;
		for (int triangleId = 0; triangleId < triangleCount; ++triangleId)
		{
			FbxFaceData newFace;
			if (newMesh->GetMaterialRefType() == FbxLayerElement::EMappingMode::eAllSame)
			{
				newFace.mMaterialIndex = 0;
			}
			else
			{
				newFace.mMaterialIndex = pMesh->GetElementMaterial()->GetIndexArray()[triangleId];
			}
			if(newMesh->GetMaterialCount() < newFace.mMaterialIndex + 1)
			{
				newMesh->SetMaterialCount(newFace.mMaterialIndex + 1);
			}
			int face_verte_size = pMesh->GetPolygonSize(triangleId);
			for (int triangleIndexId = 0; triangleIndexId < face_verte_size; triangleIndexId++)
			{
				uint32_t vertId = pMesh->GetPolygonVertex(triangleId, triangleIndexId);
				uint32_t textureUvId = pMesh->GetTextureUVIndex(triangleId, triangleIndexId);
				newFace.mIndex.push_back(vertId);
				newFace.mTextureUvIndex.push_back(textureUvId);
			}
			newMesh->AddFace(newFace);
		}
		return newMesh;
	};
}
#pragma once
#include "fbx_mesh_loader.h"
namespace luna::lfbx
{
	void LFbxDataMesh::AddControlPoint(const fbxsdk::FbxVector4& point)
	{
		mControlPoints.push_back(point);
	}

	void LFbxDataMesh::SetNormalDataType(
		fbxsdk::FbxLayerElement::EMappingMode mapMode,
		fbxsdk::FbxLayerElement::EReferenceMode refMode
	)
	{
		mNormalLayer.mMapMode = mapMode;
		mNormalLayer.mRefMode = refMode;
	}

	void LFbxDataMesh::AddNormal(const fbxsdk::FbxVector4& normal)
	{
		mNormalLayer.mLayerData.push_back(normal);
	}

	void LFbxDataMesh::SetTangentDataType(
		fbxsdk::FbxLayerElement::EMappingMode mapMode,
		fbxsdk::FbxLayerElement::EReferenceMode refMode
	)
	{
		mTangentLayer.mMapMode = mapMode;
		mTangentLayer.mRefMode = refMode;
	}

	void LFbxDataMesh::AddTangent(const fbxsdk::FbxVector4& tangent)
	{
		mTangentLayer.mLayerData.push_back(tangent);
	}

	void LFbxDataMesh::SetColorDataType(
		fbxsdk::FbxLayerElement::EMappingMode mapMode,
		fbxsdk::FbxLayerElement::EReferenceMode refMode
	)
	{
		mColorLayer.mMapMode = mapMode;
		mColorLayer.mRefMode = refMode;
	}

	void LFbxDataMesh::AddColor(const fbxsdk::FbxColor& color)
	{
		mColorLayer.mLayerData.push_back(color);
	}

	void LFbxDataMesh::SetUvDataType(
		int32_t channel,
		fbxsdk::FbxLayerElement::EMappingMode mapMode,
		fbxsdk::FbxLayerElement::EReferenceMode refMode
	)
	{
		while (channel >= mUvLayer.size())
		{
			mUvLayer.emplace_back();
		}
		mUvLayer[channel].mMapMode = mapMode;
		mUvLayer[channel].mRefMode = refMode;
	}

	void LFbxDataMesh::AddUv(const fbxsdk::FbxVector2& uv, int32_t channel)
	{
		assert(channel < (int32_t)mUvLayer.size());
		mUvLayer[channel].mLayerData.push_back(uv);
	}

	void LFbxDataMesh::AddFace(const FbxFaceData& face_data)
	{
		mFace.push_back(face_data);
	}

	std::shared_ptr<LFbxDataBase> LFbxLoaderMesh::ParsingDataImpl(const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager)
	{
		std::shared_ptr<LFbxDataMesh> newMesh = std::make_shared<LFbxDataMesh>();
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			return nullptr;
		}
		//设置材质引用信息
		if (pMesh->GetElementMaterial())
		{
			newMesh->SetMaterialDataType(pMesh->GetElementMaterial()->GetMappingMode());
			newMesh->SetMaterialRefCount(pNode->GetMaterialCount());
		}
		//转换mesh的几何信息(三角化，平滑组)
		FbxGeometryConverter SDKGeometryConverter(pManager);
		int32_t LayerSmoothingCount = pMesh->GetLayerCount(fbxsdk::FbxLayerElement::eSmoothing);
		for (int32_t i = 0; i < LayerSmoothingCount; i++)
		{
			fbxsdk::FbxLayerElementSmoothing const* SmoothingInfo = pMesh->GetLayer(0)->GetSmoothing();
			if (SmoothingInfo && SmoothingInfo->GetMappingMode() != fbxsdk::FbxLayerElement::eByPolygon)
			{
				SDKGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(pMesh, i);
			}
		}
		if (!pMesh->IsTriangleMesh())
		{
			const bool bReplace = true;
			fbxsdk::FbxNodeAttribute* ConvertedNode = SDKGeometryConverter.Triangulate(pMesh, bReplace);
			if (ConvertedNode != NULL && ConvertedNode->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh)
			{
				pMesh = (fbxsdk::FbxMesh*)ConvertedNode;
			}
			else
			{
				LFbxImportLog::GetInstance()->AddLog("FBX face change to triangle face error");
				return nullptr;
			}
		}
		//拷贝控制点
		fbxsdk::FbxVector4* pCtrlPoint = pMesh->GetControlPoints();
		int allControlPointCount = pMesh->GetControlPointsCount();
		for (int32_t i = 0; i < allControlPointCount; ++i)
		{
			newMesh->AddControlPoint(pCtrlPoint[i]);
		}
		//拷贝法线
		fbxsdk::FbxGeometryElementNormal* leNormal = nullptr;
		if (pMesh->GetElementNormalCount() >= 1)
		{
			leNormal = pMesh->GetElementNormal(0);
		}
		if (leNormal != nullptr)
		{
			newMesh->SetNormalDataType(leNormal->GetMappingMode(), leNormal->GetReferenceMode());
			fbxsdk::FbxLayerElementArrayTemplate<fbxsdk::FbxVector4> &normalArray = leNormal->GetDirectArray();
			for (int32_t i = 0; i < normalArray.GetCount(); ++i)
			{
				newMesh->AddNormal(normalArray[i]);
			}
		}
		//拷贝切线
		fbxsdk::FbxGeometryElementTangent* leTangent = nullptr;
		if (pMesh->GetElementTangentCount() >= 1)
		{
			leTangent = pMesh->GetElementTangent(0);
		}
		if (leTangent != nullptr)
		{
			newMesh->SetTangentDataType(leTangent->GetMappingMode(), leTangent->GetReferenceMode());
			fbxsdk::FbxLayerElementArrayTemplate<fbxsdk::FbxVector4> &tangentArray = leTangent->GetDirectArray();
			for (int32_t i = 0; i < tangentArray.GetCount(); ++i)
			{
				newMesh->AddTangent(tangentArray[i]);
			}
		}
		//拷贝顶点色
		fbxsdk::FbxGeometryElementVertexColor* leColor = nullptr;
		if (pMesh->GetElementVertexColorCount() >= 1)
		{
			leColor = pMesh->GetElementVertexColor(0);
		}
		if (leColor != nullptr)
		{
			newMesh->SetColorDataType(leColor->GetMappingMode(), leColor->GetReferenceMode());
			fbxsdk::FbxLayerElementArrayTemplate<fbxsdk::FbxColor> &colorArray = leColor->GetDirectArray();
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
			fbxsdk::FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(k);
			LString uv_channel_name = pVertexUV->GetName();
			int channel_id = k;
			if (uv_channel_name.Find("UVChannel_") != std::string::npos)
			{
				LString channel_id_str = uv_channel_name.Substr(10, uv_channel_name.Length() - 10);
				channel_id = atoi(channel_id_str.c_str()) - 1;
			}
			newMesh->SetUvDataType(channel_id,pVertexUV->GetMappingMode(), pVertexUV->GetReferenceMode());
			fbxsdk::FbxLayerElementArrayTemplate<fbxsdk::FbxVector2> &uvArray = pVertexUV->GetDirectArray();
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
			if (newMesh->GetMaterialRefType() == fbxsdk::FbxLayerElement::EMappingMode::eAllSame)
			{
				newFace.mMaterialIndex = 0;
			}
			else
			{
				newFace.mMaterialIndex = pMesh->GetElementMaterial()->GetIndexArray()[triangleId];
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
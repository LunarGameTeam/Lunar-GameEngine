#pragma once
#include"fbx_loader_common.h"
namespace luna::lfbx
{
	template<typename layerDataType>
	struct LFbxLayerData
	{
		fbxsdk::FbxLayerElement::EMappingMode mMapMode;
		fbxsdk::FbxLayerElement::EReferenceMode mRefMode;
		fbxsdk::FbxLayerElementTemplate<layerDataType>* mLayerMapRefData;
		LVector<layerDataType> mLayerData;
	};

	struct FbxFaceData
	{
		LVector<int32_t> mIndex;
		LVector<int32_t> mTextureUvIndex;
		int32_t mMaterialIndex;
	};

	class LFbxDataMesh : public LFbxDataBase
	{
		fbxsdk::FbxLayerElement::EMappingMode mMaterialMode;
		int32_t mMaterialRefCount = 0;
		LVector<fbxsdk::FbxVector4> mControlPoints;
		LFbxLayerData<fbxsdk::FbxVector4> mNormalLayer;
		LFbxLayerData<fbxsdk::FbxVector4> mTangentLayer;
		LFbxLayerData<fbxsdk::FbxColor> mColorLayer;
		LVector<LFbxLayerData<fbxsdk::FbxVector2>> mUvLayer;
		LVector<FbxFaceData> mFace;
		fbxsdk::FbxAMatrix mGeometryMatrix;
	public:
		LFbxDataMesh() : LFbxDataBase(LFbxDataType::FbxMeshData) {};

		inline void SetMaterialDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode
		) 
		{
			mMaterialMode = mapMode;
		};

		inline void SetMatrixGeometry(fbxsdk::FbxAMatrix geometryMatrix)
		{
			mGeometryMatrix = geometryMatrix;
		}

		inline void SetMaterialCount(int32_t matCount)
		{
			mMaterialRefCount = matCount;
		}

		inline int32_t GetMaterialCount() const
		{
			return mMaterialRefCount;
		}

		fbxsdk::FbxLayerElement::EMappingMode GetMaterialRefType() const 
		{
			return mMaterialMode;
		}

		inline void SetMaterialRefCount(
			int32_t materialRefCount
		)
		{
			mMaterialRefCount = materialRefCount;
		};

		void AddControlPoint(const fbxsdk::FbxVector4& point);

		void SetNormalDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode,
			fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* layerData
		);

		void AddNormal(const fbxsdk::FbxVector4& normal);

		void SetTangentDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode,
			fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* layerData
		);

		void AddTangent(const fbxsdk::FbxVector4& tangent);

		void SetColorDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode,
			fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxColor>* layerData
		);

		void AddColor(const fbxsdk::FbxColor& color);

		void SetUvDataType(
			int32_t channel,
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode,
			fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector2>* layerData
		);

		void AddUv(const fbxsdk::FbxVector2& uv,int32_t channel);

		void AddFace(const FbxFaceData& face_data);

		inline const size_t GetFaceSize()const
		{
			return mFace.size();
		}

		inline const FbxFaceData& GetFaceDataByIndex(const size_t index)const
		{
			return mFace[index];
		}

		inline const fbxsdk::FbxVector4 GetControlPointByIndex(const size_t index) const
		{
			return mControlPoints[index];
		}

		const fbxsdk::FbxColor GetColorByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const fbxsdk::FbxVector4 GetNormalByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const fbxsdk::FbxVector4 GetTangentByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const fbxsdk::FbxVector2 GetUvByIndex(const size_t vertIndex, const size_t polygenTextureIndex,const size_t uv_channel) const;

		inline const size_t GetUvChannelSize() const
		{
			return mUvLayer.size();
		};
	};

	class LFbxLoaderMesh : public LFbxLoaderBase
	{
	public:
		LFbxLoaderMesh(){};
	private:
		std::shared_ptr<LFbxDataBase> ParsingDataImpl(const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager) override;
	};
}
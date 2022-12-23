#pragma once
#include"fbx_loader_common.h"
namespace luna::lfbx
{
	template<typename layerDataType>
	struct LFbxLayerData
	{
		fbxsdk::FbxLayerElement::EMappingMode mMapMode;
		fbxsdk::FbxLayerElement::EReferenceMode mRefMode;
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
	public:
		LFbxDataMesh() : LFbxDataBase(LFbxDataType::FbxMeshData) {};

		inline void SetMaterialDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode
		) 
		{
			mMaterialMode = mapMode;
		};

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
			fbxsdk::FbxLayerElement::EReferenceMode refMode
		);

		void AddNormal(const fbxsdk::FbxVector4& normal);

		void SetTangentDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode
		);

		void AddTangent(const fbxsdk::FbxVector4& tangent);

		void SetColorDataType(
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode
		);

		void AddColor(const fbxsdk::FbxColor& color);

		void SetUvDataType(
			int32_t channel,
			fbxsdk::FbxLayerElement::EMappingMode mapMode,
			fbxsdk::FbxLayerElement::EReferenceMode refMode
		);

		void AddUv(const fbxsdk::FbxVector2& uv,int32_t channel);

		void AddFace(const FbxFaceData& face_data);
	};

	class LFbxLoaderMesh : public LFbxLoaderBase
	{
	public:
		LFbxLoaderMesh(){};
	private:
		std::shared_ptr<LFbxDataBase> ParsingDataImpl(const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager) override;
	};
}
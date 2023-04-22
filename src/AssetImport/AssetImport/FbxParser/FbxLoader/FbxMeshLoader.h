#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderCommon.h"
namespace luna::lfbx
{
	template<typename layerDataType>
	struct LFbxLayerData
	{
		FbxLayerElement::EMappingMode mMapMode;

		FbxLayerElement::EReferenceMode mRefMode;

		FbxLayerElementTemplate<layerDataType>* mLayerMapRefData;

		LArray<layerDataType> mLayerData;
	};

	struct FbxFaceData
	{
		LArray<int32_t> mIndex;

		LArray<int32_t> mTextureUvIndex;

		int32_t mMaterialIndex;
	};

	struct FbxClusterControlPointData
	{
		int32_t mCtrlPointCount;

		LArray<int> mIndices;

		LArray<double> mWeights;
	};

	class LFbxDataMesh : public LFbxDataBase
	{
		bool mHasSkinMessage = false;

		FbxLayerElement::EMappingMode mMaterialMode;

		int32_t mMaterialRefCount = 0;

		LArray<FbxVector4> mControlPoints;

		LFbxLayerData<FbxVector4> mNormalLayer;

		LFbxLayerData<FbxVector4> mTangentLayer;

		LFbxLayerData<FbxColor> mColorLayer;

		LArray<LFbxLayerData<FbxVector2>> mUvLayer;

		LArray<FbxFaceData> mFace;

		FbxAMatrix mGeometryMatrix;

		LArray<LString> mBindBoneName;

		LArray<FbxAMatrix> mPoseMatrix;

		LArray<FbxClusterControlPointData> mBindBoneControlData;

		LUnorderedMap<LString,int32_t> mBindBoneNameRef;
		
	public:
		LFbxDataMesh() : LFbxDataBase(LFbxDataType::FbxMeshData) {};

		bool HasSkinInfo()const { return mHasSkinMessage; };

		int32_t TryAddBone(const LString &boneName);

		void SetBonePoseMatrix(int32_t BoneIndex,const FbxAMatrix &linkMatrix);

		void SetBoneControlData(int32_t BoneIndex, int32_t CtrlPointCount, int* pCtrlPointIndices, double* pCtrlPointWeights);

		inline int32_t GetRefBoneCount() const
		{
			return (int32_t)mBindBoneName.size();
		}

		LString GetRefBoneNameByIndex(int32_t BoneIndex) const;

		FbxAMatrix GetBonePoseByIndex(int32_t BoneIndex) const;
		
		const FbxClusterControlPointData& GetBoneControlVertexDataByIndex(int32_t BoneIndex) const;


		inline void SetMaterialDataType(
			FbxLayerElement::EMappingMode mapMode
		) 
		{
			mMaterialMode = mapMode;
		};

		inline void SetMatrixGeometry(FbxAMatrix geometryMatrix)
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

		FbxLayerElement::EMappingMode GetMaterialRefType() const 
		{
			return mMaterialMode;
		}

		inline void SetMaterialRefCount(
			int32_t materialRefCount
		)
		{
			mMaterialRefCount = materialRefCount;
		};

		void AddControlPoint(const FbxVector4& point);

		void SetNormalDataType(
			FbxLayerElement::EMappingMode mapMode,
			FbxLayerElement::EReferenceMode refMode,
			FbxLayerElementTemplate<FbxVector4>* layerData
		);

		void AddNormal(const FbxVector4& normal);

		void SetTangentDataType(
			FbxLayerElement::EMappingMode mapMode,
			FbxLayerElement::EReferenceMode refMode,
			FbxLayerElementTemplate<FbxVector4>* layerData
		);

		void AddTangent(const FbxVector4& tangent);

		void SetColorDataType(
			FbxLayerElement::EMappingMode mapMode,
			FbxLayerElement::EReferenceMode refMode,
			FbxLayerElementTemplate<FbxColor>* layerData
		);

		void AddColor(const FbxColor& color);

		void SetUvDataType(
			int32_t channel,
			FbxLayerElement::EMappingMode mapMode,
			FbxLayerElement::EReferenceMode refMode,
			FbxLayerElementTemplate<FbxVector2>* layerData
		);

		void AddUv(const FbxVector2& uv,int32_t channel);

		void AddFace(const FbxFaceData& face_data);

		inline const size_t GetFaceSize()const
		{
			return mFace.size();
		}

		inline const FbxFaceData& GetFaceDataByIndex(const size_t index)const
		{
			return mFace[index];
		}

		inline const FbxVector4 GetControlPointByIndex(const size_t index) const
		{
			return mControlPoints[index];
		}

		inline const size_t GetControlPointSize() const
		{
			return mControlPoints.size();
		}

		const FbxColor GetColorByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const FbxVector4 GetNormalByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const FbxVector4 GetTangentByIndex(const size_t vertIndex, const size_t polygenIndex) const;

		const FbxVector2 GetUvByIndex(const size_t vertIndex, const size_t polygenTextureIndex,const size_t uv_channel) const;

		inline const size_t GetUvChannelSize() const
		{
			return mUvLayer.size();
		};

		void GenerateBindPoseToMesh(const FbxSkin* pSkin);
	};

	class LFbxLoaderMesh : public LFbxLoaderBase
	{
	public:

		LFbxLoaderMesh(){};
	private:

		std::shared_ptr<LFbxDataBase> ParsingDataImpl(const LArray<LFbxNodeBase>& sceneNodes, FbxNode* pNode, LFbxLoadContext context) override;
	};
}
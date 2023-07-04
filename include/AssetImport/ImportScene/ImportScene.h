#pragma once
#include "AssetImport/ImportScene/ImportDataMesh.h"
#include "AssetImport/ImportScene/ImportDataSkeleton.h"
#include "AssetImport/ImportScene/ImportDataSkeletonAnimation.h"
#ifndef ASSET_IMPORT_API
#ifdef ASSET_IMPORT_EXPORT
#define ASSET_IMPORT_API __declspec( dllexport )
#else
#define ASSET_IMPORT_API __declspec( dllimport )
#endif
#endif

namespace luna::asset
{
	class ASSET_IMPORT_API LImportScene
	{
		LImportAxisType mAxis;

		LImportUnitType mUnit;

		LArray<LImportSceneNode> mNodes;

		LArray<std::shared_ptr<LImportNodeDataBase>> mDatas;

		LArray<std::shared_ptr<LImportNodeAnimationBase>> mAnimations;

	public:
		void SetAxisAndUnit(LImportAxisType axis, LImportUnitType unit);

		void AddNodeData(LImportSceneNode node);

		void ConvertDataAxisAndUnit(LImportAxisType axis, LImportUnitType unit);

		void PostProcessData();

		float GetUnitValue(LImportUnitType unit);

		template<typename ImportDataType>
		ImportDataType* AddNewData()
		{
			size_t newIndex = mDatas.size();
			std::shared_ptr<LImportNodeDataBase> newData = std::make_shared<ImportDataType>(newIndex);
			mDatas.push_back(newData);
			return dynamic_cast<ImportDataType*>(newData.get());
		};

		template<typename ImportAnimationType>
		ImportAnimationType* AddNewAnimation()
		{
			size_t newIndex = mAnimations.size();
			std::shared_ptr<ImportAnimationType> newData = std::make_shared<ImportAnimationType>(newIndex);
			mAnimations.push_back(newData);
			return dynamic_cast<ImportAnimationType*>(newData.get());
		};

		size_t GetNodeSize()const
		{
			return mNodes.size();
		};

		const LImportSceneNode& GetNode(size_t dataIndex) const
		{
			return mNodes[dataIndex];
		};

		void ResetNodeData(size_t nodeIndex, asset::LImportNodeDataType dataType,size_t dataIndex);

		size_t GetNodeDataSize()const
		{
			return mDatas.size();
		};

		template<typename ImportDataType>
		const ImportDataType* GetData(size_t dataIndex) const
		{
			if (dataIndex >= mDatas.size())
			{
				return nullptr;
			}
			ImportDataType* dataFind = dynamic_cast<ImportDataType*>(mDatas[dataIndex].get());
			return dataFind;
		};

		LArray<size_t> FilterDataByType(LImportNodeDataType type);

		size_t GetAnimationDataSize()const
		{
			return mAnimations.size();
		};

		LArray<size_t> FilterAnimationByType(LImportNodeAnimationType type)const;

		template<typename ImportAnimationType>
		const ImportAnimationType* GetAnimation(size_t animIndex) const
		{
			if (animIndex >= mAnimations.size())
			{
				return nullptr;
			}
			ImportAnimationType* animFind = dynamic_cast<ImportAnimationType*>(mAnimations[animIndex].get());
			return animFind;
		};

	private:
		void ComputeConvertMatrix(LImportAxisType axis, LImportUnitType unit, LMatrix4f &convertMatrix, LMatrix4f &convertInvMatrix);
	};

}


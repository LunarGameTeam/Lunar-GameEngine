#pragma once
#include"import_data_mesh.h"
namespace luna::resimport
{
	class RESOURCE_IMPORT_API LImportScene
	{
		LImportAxisType mAxis;
		LImportUnitType mUnit;
		LVector<LImportSceneNode> mNodes;
		LVector<std::shared_ptr<LImportNodeDataBase>> mDatas;
	public:
		void SetAxisAndUnit(LImportAxisType axis, LImportUnitType unit);

		void AddNodeData(LImportSceneNode node);

		void ConvertDataAxisAndUnit(LImportAxisType axis, LImportUnitType unit);

		float GetUnitValue(LImportUnitType unit);

		template<typename ImportDataType>
		size_t AddNewData()
		{
			size_t newIndex = mDatas.size();
			std::shared_ptr<LImportNodeDataBase> newData = std::make_shared<ImportDataType>(newIndex);
			mDatas.push_back(newData);
			return newIndex;
		};

		template<typename ImportDataType>
		ImportDataType* GetData(size_t dataIndex)
		{
			if (dataIndex >= mDatas.size())
			{
				return nullptr;
			}
			ImportDataType* dataFind = dynamic_cast<ImportDataType*>(mDatas[dataIndex].get());
			return dataFind;
		};

	private:
		void ComputeConvertMatrix(LImportAxisType axis, LImportUnitType unit, LMatrix4f &convertMatrix, LMatrix4f &convertInvMatrix);
	};

}


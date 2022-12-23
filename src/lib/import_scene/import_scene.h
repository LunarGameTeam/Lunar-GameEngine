#pragma once
#include "core/math/math.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"
namespace luna::ImportData
{
	enum class LImportNodeDataType : std::uint8_t
	{
		ImportDataMesh = 0,
		ImportDataCamera,
		ImportDataMaterial
	};
	enum LImportAxisType : std::uint8_t
	{
		ImportAxisZupLeftHand = 0,
		ImportAxisYupLeftHand,
		ImportAxisZupRightHand,
		ImportAxisYupRightHand
	};
	enum LImportUnitType : std::uint8_t
	{
		ImportUnitMeter = 0,
		ImportUnitDeciMeter,
		ImportUnitCenterMeter,
		ImportUnitInch
	};
	class LImportSceneNode
	{
		size_t mIndex;
		size_t mParent;
		size_t mName;
		LTransform mTransformLocal;
		std::unordered_map<LImportNodeDataType, size_t> mNodeData;
		std::vector<size_t> mChild;
	};
	class LImportNodeDataBase
	{
		LImportNodeDataType mType;
		size_t mIndex;
	public:
		LImportNodeDataBase(const LImportNodeDataType type, const size_t index) :mType(type), mIndex(index)
		{
		};

	};

	struct LImportSubmesh
	{
		size_t mIndex;
		LString mName;
		LVector<uint32_t> mIndices;
		size_t mMaterialUse;
	};
	class LImportNodeDataMesh : public LImportNodeDataBase
	{
		LVector<LVector3f> mVertexPosition;
		LVector<LVector3f> mVertexNormal;
		LVector<LVector4f> mVertexTangent;
		LVector<LVector<LVector2f>> mVertexUv;
		LVector<LVector4f> mVertexColor;
		LVector<LImportSubmesh> mSubmesh;
	public:
		LImportNodeDataMesh(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMesh, index)
		{

		};

		void AddFullVertex(const LVector3f pos, const LVector3f norm, const LVector4f tangent, const LVector<LVector2f> uv, const LVector4f color);
		
		size_t AddSubMeshMessage(const LString &name, const size_t materialUse);

		void AddFaceIndexDataToSubmesh(const size_t subMeshIndex, const uint32_t index);
	};


	struct LImportNodeDataMaterial : public LImportNodeDataBase
	{
		LUnorderedMap<LString,LString> mMaterialParam;
	public:
		LImportNodeDataMaterial(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMaterial, index) {};
	};

	class LImportScene
	{
		LImportAxisType mAxis;
		LImportAxisType mUnit;
		LVector<LImportSceneNode> mNodes;
		LVector<std::shared_ptr<LImportNodeDataBase>> mDatas;
	public:
		template<typename ImportDataType>
		size_t AddNewData()
		{
			size_t newIndex = mDatas.size();
			std::shared_ptr<LImportNodeDataBase> newData = std::make_shared<ImportDataType>(newIndex);
			mDatas.push_back(newData);
		};

		template<typename ImportDataType>
		ImportDataType* GetData(size_t dataIndex)
		{
			if (dataIndex >= mDatas.size())
			{
				return nullptr;
			}
			ImportDataType* dataFind = static_cast<ImportDataType*>(mDatas[dataIndex].get());
			return dataFind;
		};

	};

}


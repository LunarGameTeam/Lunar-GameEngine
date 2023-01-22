#pragma once
#include"ObjLoader/ObjLoaderCommon.h"
namespace luna::lobj
{
	class LObjDataMesh : public LObjDataBase
	{
		tinyobj::attrib_t mAttriData;
		std::vector<tinyobj::shape_t> mShapeData;
	public:
		LObjDataMesh(): LObjDataBase(LObjDataType::ObjMeshData) {};
		void SetData(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData)
		{
			mAttriData = attriData;
			mShapeData = shapeData;
		};
		const tinyobj::attrib_t& GetAttriData() const { return mAttriData; };
		const std::vector<tinyobj::shape_t>& GetShapesData()const { return mShapeData; };
	};

	class LObjLoaderMesh
	{
	public:
		LObjLoaderMesh(){};
		std::shared_ptr<LObjDataBase> ParsingData(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData);
	};
}
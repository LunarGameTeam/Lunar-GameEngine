#pragma once

#include "Core/CoreMin.h"
#include <tiny_obj_loader.h>

namespace luna::asset
{

enum LObjDataType
{
	ObjMeshData = 0,
	ObjMaterialData,
};

class LObjDataBase
{
	LObjDataType mType;
public:
	LObjDataBase(LObjDataType type) { mType = type; };
	LObjDataType GetType() const { return mType; };
};

class LObjDataMesh : public LObjDataBase
{
	tinyobj::attrib_t mAttriData;
	std::vector<tinyobj::shape_t> mShapeData;
public:
	LObjDataMesh() : LObjDataBase(LObjDataType::ObjMeshData) {};
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
	LObjLoaderMesh() {};
	std::shared_ptr<LObjDataBase> ParsingData(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData);
};

	struct LObjSceneData
	{
		LArray<std::shared_ptr<LObjDataBase>> mDatas;
	};

	class LObjLoaderFactory
	{
		LObjLoaderMesh mMeshLoader;
	public:
		LObjLoaderFactory();
		std::shared_ptr<LObjDataBase> LoadObjDataMesh(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData);
	};
	static std::shared_ptr<LObjLoaderFactory> singleLoaderInterface = std::make_shared<LObjLoaderFactory>();

	class LObjLoaderHelper
	{
	public:
		void LoadObjFile(const char* pFilename, LObjSceneData& scene_out);
	};
}
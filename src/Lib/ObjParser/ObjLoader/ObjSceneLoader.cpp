#pragma once
#include "ObjLoader/ObjSceneLoader.h"
namespace luna::lobj
{
	LObjLoaderFactory::LObjLoaderFactory()
	{
	}

	std::shared_ptr<LObjDataBase> LObjLoaderFactory::LoadObjDataMesh(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData)
	{
		return mMeshLoader.ParsingData(attriData,shapeData);
	}

	void LObjLoaderHelper::LoadObjFile(const char* pFilename, LObjSceneData& sceneOut)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pFilename);
		std::shared_ptr<LObjDataBase> newMeshData = singleLoaderInterface->LoadObjDataMesh(attrib, shapes);
		sceneOut.mDatas.push_back(newMeshData);
		int a = 0;
		//tinyobj::LoadObj(,);
	}
}
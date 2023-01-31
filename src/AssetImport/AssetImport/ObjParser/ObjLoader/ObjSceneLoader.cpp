#include "ObjSceneLoader.h"
#include <memory>

namespace luna::asset
{

std::shared_ptr<luna::asset::LObjDataBase> LObjLoaderMesh::ParsingData(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData)
{
	std::shared_ptr<LObjDataMesh> newData = std::make_shared<LObjDataMesh>();
	newData->SetData(attriData, shapeData);
	return newData;
}


LObjLoaderFactory::LObjLoaderFactory()
{
}

std::shared_ptr<LObjDataBase> LObjLoaderFactory::LoadObjDataMesh(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData)
{
	return mMeshLoader.ParsingData(attriData, shapeData);
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
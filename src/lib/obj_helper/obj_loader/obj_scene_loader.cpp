#pragma once
#include "obj_scene_loader.h"
namespace luna::lobj
{
	LObjLoaderFactory::LObjLoaderFactory()
	{
	}

	std::shared_ptr<LObjDataBase> LObjLoaderFactory::LoadFbxDataMesh(LObjDataType type, const tinyobj::shape_t& shapeData)
	{
		return mMeshLoader.ParsingData(shapeData);
	}

	void LObjLoaderHelper::LoadObjFile(const char* pFilename, LObjSceneData& sceneOut)
	{
		//tinyobj::LoadObj(,);
	}
}
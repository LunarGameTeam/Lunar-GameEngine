#pragma once
#include "ObjLoader/ObjLoaderCommon.h"
#include "ObjLoader/ObjMeshLoader.h"
#include "ObjLoader/ObjMaterialLoader.h"
namespace luna::lobj
{
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
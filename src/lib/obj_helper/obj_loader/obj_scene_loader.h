#pragma once
#include"obj_loader_common.h"
#include "obj_mesh_loader.h"
#include "obj_material_loader.h"
namespace luna::lobj
{
	struct LObjSceneData
	{
		LVector<std::shared_ptr<LObjDataBase>> mDatas;
	};

	class LObjLoaderFactory
	{
		LObjLoaderMesh mMeshLoader;
	public:
		LObjLoaderFactory();
		std::shared_ptr<LObjDataBase> LoadFbxDataMesh(LObjDataType type,const tinyobj::shape_t &shapeData);
	};
	static std::shared_ptr<LObjLoaderFactory> singleLoaderInterface = std::make_shared<LObjLoaderFactory>();

	class LObjLoaderHelper
	{
	public:
		void LoadObjFile(const char* pFilename, LObjSceneData& scene_out);
	};
}
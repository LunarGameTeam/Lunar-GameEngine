#pragma once
#include"obj_loader_common.h"
namespace luna::lobj
{
	class LObjDataMesh : public LObjDataBase
	{
	public:
		LObjDataMesh(): LObjDataBase(LObjDataType::ObjMeshData) {};

	};

	class LObjLoaderMesh
	{
	public:
		LObjLoaderMesh(){};
		std::shared_ptr<LObjDataBase> ParsingData(const tinyobj::shape_t &shape_in);
	};
}
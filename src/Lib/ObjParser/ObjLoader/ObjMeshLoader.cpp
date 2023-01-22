#pragma once
#include "ObjLoader/ObjMeshLoader.h"
namespace luna::lobj
{
	std::shared_ptr<LObjDataBase> LObjLoaderMesh::ParsingData(const tinyobj::attrib_t& attriData, const std::vector<tinyobj::shape_t>& shapeData)
	{
		std::shared_ptr<LObjDataMesh> newData = std::make_shared<LObjDataMesh>();
		newData->SetData(attriData, shapeData);
		return newData;
	}
}
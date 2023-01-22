#pragma once
#include "Core/CoreMin.h"
#include<tiny_obj_loader.h>
namespace luna::lobj
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

}
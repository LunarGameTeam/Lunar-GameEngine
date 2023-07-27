#pragma once
#include "Graphics/Asset/MeshAsset.h"

namespace luna::graphics
{
	void AddLineToSubMesh(const BaseVertex& v1, const BaseVertex& v2, SubMesh& submeshData);

	void AddLineToSubMesh(const LVector3f& v1, const LVector3f& v2, SubMesh& submeshData);

	void AddCubeWiredToSubMesh(SubMesh& submeshData,const LVector3f& position, const LVector3f& size = LVector3f(1, 1, 1), const LVector4f& color = LVector4f(1, 1, 1));

	void AddTriangleToSubMesh(const BaseVertex& v1, const BaseVertex& v2, const BaseVertex& v3, SubMesh& submeshData);

}
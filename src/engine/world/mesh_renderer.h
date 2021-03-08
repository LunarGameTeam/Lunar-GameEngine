#pragma once
#include "private_world.h"
#include "world/renderer.h"

namespace luna
{

class WORLD_API MeshRenderer : public RendererComponent
{

};

class WORLD_API StaticMeshRenderer : public MeshRenderer
{

};

class WORLD_API SkeletonMeshRenderer : public MeshRenderer
{

};


}
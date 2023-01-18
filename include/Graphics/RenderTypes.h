#pragma once

#include "Core/Foundation/Container.h"

namespace luna::render
{

struct PackedParams;
struct ShaderParamsBuffer;

class MaterialTemplateAsset;
class MaterialInstance;
class SubMesh;
class ShaderAsset;

class RenderScene;
class RenderView;

struct Light;
struct PointLight;
struct DirectionLight;

class FrameGraphBuilder;
class FGNode;
class FGResource;
class FGTexture;
class FGResourceView;

class RenderPass;
class Texture2D;
class TextureCube;


class RenderTarget;

struct ImguiTexture;
class RenderDevice;

struct RenderLight;
struct RenderObject;

using ROArray = LArray<RenderObject*>;
using ViewArray = LArray<RenderView*>;

}
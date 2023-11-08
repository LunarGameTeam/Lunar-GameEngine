#pragma once

#include "Core/Foundation/Container.h"

namespace luna::graphics
{

struct PackedParams;
struct ShaderCBuffer;

class MaterialTemplateAsset;
class MaterialInstance;
class SubMesh;
class ShaderAsset;

class RenderScene;
class RenderView;

struct PointBasedLight;

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
class RenderContext;

struct RenderLight;
struct RenderObject;

using ROArray = LArray<RenderObject*>;
using ViewArray = LArray<RenderView*>;

}
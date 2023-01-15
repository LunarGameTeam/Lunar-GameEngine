#pragma once

#include "core/foundation/container.h"

namespace luna::render
{

struct PackedParams;
struct PerViewBuffer;

class RenderScene;
class RenderView;
class MaterialTemplateAsset;

class FrameGraphBuilder;
class FGNode;
class FGTexture;
class RenderPass;
class Texture2D;
class TextureCube;

class ShaderAsset;

class RenderTarget;

struct ImguiTexture;

class MaterialInstance;
class SubMesh;
class RenderDevice;

struct RenderLight;
struct RenderObject;

using ROArray = LArray<RenderObject*>;
using ViewArray = LArray<RenderView*>;

}
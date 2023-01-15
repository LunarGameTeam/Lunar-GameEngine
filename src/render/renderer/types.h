#pragma once

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

class MaterialInstance;
class SubMesh;

struct RenderLight;
struct RenderObject;

using ROArray = LArray<RenderObject*>;
using ViewArray = LArray<RenderView*>;

}
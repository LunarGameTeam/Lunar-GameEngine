#pragma once

#include "render/pch.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_obj.h"

#include "core/misc/container.h"
#include "core/misc/string.h"
#include "render/frame_graph/frame_graph.h"
#include <functional>


namespace luna::render
{

struct RenderObject;
class RenderModule;
class RenderView;
class RenderScene;


class RENDER_API RenderPass
{
public:
	RenderPass()
	{
	}

	~RenderPass()
	{
	};

	RHIResourcePtr mPassBuffer;
	std::vector<byte> mPassBufferPtr;
	RHIViewPtr mPassView;

	virtual void BuildRenderPass(FrameGraphBuilder* fg, RenderView* view, RenderScene* renderScene);
public:
	void Init();

	virtual void OnRenderPassPrepare();

	bool IsInit() { return m_is_init; };
public:
	void SetUsingMaterialShader(bool val) { m_using_mat_shader = val; }
	//virtual void BuildRenderPass(LFrameGraphBuilder* builder) = 0;
protected:
	void SetupPassBuffer();
	virtual void RenderPassSetup(FrameGraphBuilder* builder, FGNode& pass_node) {};

	virtual void RenderPassExcute(FrameGraphBuilder* builder, FGNode& node, RenderDevice* device) {};

	virtual void InitImpl();

	LString m_pass_name;

	const RenderView* mView;

	const RenderScene* mRenderScene;

	bool m_using_mat_shader = false;

	bool m_is_init = false;
};
}

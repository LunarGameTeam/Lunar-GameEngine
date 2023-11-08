#pragma once
#include "Graphics/FrameGraph/FrameGraphNode.h"
namespace luna::graphics
{
	class FrameGraphPassGenerator
	{
		LString mPassName;
	public:
		FrameGraphPassGenerator(const LString passName) :
			mPassName(passName) {};
		virtual void AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)  = 0;
	};

	class FrameGraphMeshPassGenerator : public FrameGraphPassGenerator
	{
	public:
		FrameGraphMeshPassGenerator(const LString passName) :
			FrameGraphPassGenerator(passName) {};

	};
}


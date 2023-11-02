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
		void AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene);
	private:
		virtual void AddPassNodeImpl(FrameGraphBuilder* builder, FGNode& curNode,RenderView* view, RenderScene* renderScene) = 0;
	};
	void FrameGraphPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
	{
		FGNode& curNode = builder->AddPass(mPassName);
		AddPassNodeImpl(builder, curNode, view, renderScene);
	}

	class FrameGraphMeshPassGenerator : public FrameGraphPassGenerator
	{
	public:
		FrameGraphMeshPassGenerator(const LString passName) :
			FrameGraphPassGenerator(passName) {};

	};
}


#pragma once

#include "Core/Reflection/Reflection.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIResource.h"

namespace luna::graphics
{

//IMGUI
struct RENDER_API ImguiTexture
{
public:
	RHIViewPtr	mView;
	TRHIPtr<RHIObject> mDX12Pool;
	void* mImg;
};
}

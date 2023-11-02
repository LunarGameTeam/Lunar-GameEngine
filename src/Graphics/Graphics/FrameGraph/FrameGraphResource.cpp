#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/FrameGraph/FrameGraph.h"


namespace luna::graphics
{

	FGResource::~FGResource()
	{
		mBuilder->RemoveVirtualResourceId(mUniqueId);
	}

}

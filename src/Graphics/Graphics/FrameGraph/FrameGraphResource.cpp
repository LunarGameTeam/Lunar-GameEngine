#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/FrameGraph/FrameGraph.h"


namespace luna::graphics
{
	RHIResDesc GenerateTexture2DRhiDesc(
		uint32_t width,
		uint32_t height,
		RHITextureFormat format,
		RHIImageUsage usage
	)
	{
		RHIResDesc newDesc = {};
		newDesc.ResHeapType = RHIHeapType::Default;
		newDesc.mUseVma = false;
		newDesc.Dimension = RHIResDimension::Texture2D;
		newDesc.mType = ResourceType::kTexture;
		newDesc.Width = width;
		newDesc.Height = height;
		newDesc.Format = format;
		newDesc.mImageUsage = usage;
		return newDesc;
	}
	FGResource::~FGResource()
	{
		mBuilder->RemoveVirtualResourceId(mUniqueId);
	}

}

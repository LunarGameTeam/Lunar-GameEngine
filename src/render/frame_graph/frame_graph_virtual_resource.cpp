#include"frame_graph_virtual_resource.h"
#include "render/render_module.h"


namespace luna::render
{
FGVirtualRes::FGVirtualRes(const LString& name, const RHIResDesc& desc, VirtualResType type):
	mName(name),mType(type),mDesc(desc)
{

}

FGVirtualRes::~FGVirtualRes()
{
}

FGVirtualBuffer::FGVirtualBuffer(
	const LString& name,
	const RHIResDesc& resDesc
) : FGVirtualRes(name, resDesc, VirtualResType::Buffer)
{

}


FGVirtualTexture::FGVirtualTexture(
	const LString& name,
	const RHIResDesc& desc,
	const RHITextureDesc& texDesc
) : FGVirtualRes(name, desc, VirtualResType::Texture)
{
	mTextureDesc = texDesc;
}

}

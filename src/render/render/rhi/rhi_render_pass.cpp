#include "render/rhi/rhi_render_pass.h"

#include <boost/container_hash/hash.hpp>

namespace luna::render
{

size_t RenderPassDesc::Hash() const
{
	size_t result = 0;
	boost::hash_combine(result, mColors.size());
	for (auto& view : mColorView)
	{
		boost::hash_combine(result, view->mBindResource->mResDesc.Format);
	}
	boost::hash_combine(result, mDepths.size());
	if (mDepthStencilView)
		boost::hash_combine(result, mDepthStencilView->mBindResource->mResDesc.Format);
	return result;
}

}

#include "Graphics/Renderer/MaterialParam.h"
#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/RHI/RHIResource.h"
#include "Core/Memory/PtrBinding.h"
#include "Graphics/RHI/RHIPipeline.h"


namespace luna::graphics
{
//优化 增量更新
PARAM_ID(MaterialBuffer);

ShaderCBuffer::ShaderCBuffer(const RHICBufferDesc& cbDesc) :
	mVars(cbDesc.mVars)
{
	size_t curAlignedSize = SizeAligned2Pow(cbDesc.mSize, 256);
	mData.resize(curAlignedSize);
}

}
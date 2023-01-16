#pragma once

#include "Graphics/RHI/RHIPch.h"

#include <Vulkan/vulkan.hpp>

namespace luna::render
{

vk::DescriptorType Convert(RHIViewType view_type);
vk::ImageType Convert(RHIResDimension dimension);
vk::ImageLayout ConvertState(ResourceState state);
vk::Format Convert(RHITextureFormat format);
vk::CompareOp Convert(RHIComparisionFunc func);
vk::PrimitiveTopology Convert(RHIPrimitiveTopology primitive);
RHITextureFormat Convert(vk::Format format);

vk::ShaderStageFlagBits ConvertShader(RHIShaderVisibility type);

}
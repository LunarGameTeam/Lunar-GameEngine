//
// Copyright(c) 2019 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
#include "tinyddsloader.h"


#if _WIN32
#undef min
#undef max
#endif  // _Win32

#include <algorithm>
#include <fstream>

namespace tinyddsloader {

const char DDSFile::Magic[4] = {'D', 'D', 'S', ' '};

bool DDSFile::IsCompressed(DXGIFormat fmt) {
    switch (fmt) {
        case DXGIFormat::BC1_Typeless:
        case DXGIFormat::BC1_UNorm:
        case DXGIFormat::BC1_UNorm_SRGB:
        case DXGIFormat::BC2_Typeless:
        case DXGIFormat::BC2_UNorm:
        case DXGIFormat::BC2_UNorm_SRGB:
        case DXGIFormat::BC3_Typeless:
        case DXGIFormat::BC3_UNorm:
        case DXGIFormat::BC3_UNorm_SRGB:
        case DXGIFormat::BC4_Typeless:
        case DXGIFormat::BC4_UNorm:
        case DXGIFormat::BC4_SNorm:
        case DXGIFormat::BC5_Typeless:
        case DXGIFormat::BC5_UNorm:
        case DXGIFormat::BC5_SNorm:
        case DXGIFormat::BC6H_Typeless:
        case DXGIFormat::BC6H_UF16:
        case DXGIFormat::BC6H_SF16:
        case DXGIFormat::BC7_Typeless:
        case DXGIFormat::BC7_UNorm:
        case DXGIFormat::BC7_UNorm_SRGB:
            return true;
    }
    return false;
}

uint32_t DDSFile::MakeFourCC(char ch0, char ch1, char ch2, char ch3) {
    return (uint32_t(uint8_t(ch0)) | (uint32_t(uint8_t(ch1)) << 8) |
            (uint32_t(uint8_t(ch2)) << 16) | (uint32_t(uint8_t(ch3)) << 24));
}

DDSFile::DXGIFormat DDSFile::GetDXGIFormat(const PixelFormat& pf) {
    if (pf.m_flags & uint32_t(PixelFormatFlagBits::RGB)) {
        switch (pf.m_bitCount) {
            case 32:
                if (pf.m_RBitMask == 0x000000ff &&
                    pf.m_GBitMask == 0x0000ff00 &&
                    pf.m_BBitMask == 0x00ff0000 &&
                    pf.m_ABitMask == 0xff000000) {
                    return DXGIFormat::R8G8B8A8_UNorm;
                }
                if (pf.m_RBitMask == 0x00ff0000 &&
                    pf.m_GBitMask == 0x0000ff00 &&
                    pf.m_BBitMask == 0x000000ff &&
                    pf.m_ABitMask == 0xff000000) {
                    return DXGIFormat::B8G8R8A8_UNorm;
                }
                if (pf.m_RBitMask == 0x00ff0000 &&
                    pf.m_GBitMask == 0x0000ff00 &&
                    pf.m_BBitMask == 0x000000ff &&
                    pf.m_ABitMask == 0x00000000) {
                    return DXGIFormat::B8G8R8X8_UNorm;
                }

                if (pf.m_RBitMask == 0x0000ffff &&
                    pf.m_GBitMask == 0xffff0000 &&
                    pf.m_BBitMask == 0x00000000 &&
                    pf.m_ABitMask == 0x00000000) {
                    return DXGIFormat::R16G16_UNorm;
                }

                if (pf.m_RBitMask == 0xffffffff &&
                    pf.m_GBitMask == 0x00000000 &&
                    pf.m_BBitMask == 0x00000000 &&
                    pf.m_ABitMask == 0x00000000) {
                    return DXGIFormat::R32_Float;
                }
                break;
            case 24:
                break;
            case 16:
                if (pf.m_RBitMask == 0x7c00 && pf.m_GBitMask == 0x03e0 &&
                    pf.m_BBitMask == 0x001f && pf.m_ABitMask == 0x8000) {
                    return DXGIFormat::B5G5R5A1_UNorm;
                }
                if (pf.m_RBitMask == 0xf800 && pf.m_GBitMask == 0x07e0 &&
                    pf.m_BBitMask == 0x001f && pf.m_ABitMask == 0x0000) {
                    return DXGIFormat::B5G6R5_UNorm;
                }

                if (pf.m_RBitMask == 0x0f00 && pf.m_GBitMask == 0x00f0 &&
                    pf.m_BBitMask == 0x000f && pf.m_ABitMask == 0xf000) {
                    return DXGIFormat::B4G4R4A4_UNorm;
                }
                break;
            default:
                break;
        }
    } else if (pf.m_flags & uint32_t(PixelFormatFlagBits::Luminance)) {
        if (8 == pf.m_bitCount) {
            if (pf.m_RBitMask == 0x000000ff && pf.m_GBitMask == 0x00000000 &&
                pf.m_BBitMask == 0x00000000 && pf.m_ABitMask == 0x00000000) {
                return DXGIFormat::R8_UNorm;
            }
            if (pf.m_RBitMask == 0x000000ff && pf.m_GBitMask == 0x0000ff00 &&
                pf.m_BBitMask == 0x00000000 && pf.m_ABitMask == 0x00000000) {
                return DXGIFormat::R8G8_UNorm;
            }
        }
        if (16 == pf.m_bitCount) {
            if (pf.m_RBitMask == 0x0000ffff && pf.m_GBitMask == 0x00000000 &&
                pf.m_BBitMask == 0x00000000 && pf.m_ABitMask == 0x00000000) {
                return DXGIFormat::R16_UNorm;
            }
            if (pf.m_RBitMask == 0x000000ff && pf.m_GBitMask == 0x0000ff00 &&
                pf.m_BBitMask == 0x00000000 && pf.m_ABitMask == 0x00000000) {
                return DXGIFormat::R8G8_UNorm;
            }
        }
    } else if (pf.m_flags & uint32_t(PixelFormatFlagBits::Alpha)) {
        if (8 == pf.m_bitCount) {
            return DXGIFormat::A8_UNorm;
        }
    } else if (pf.m_flags & uint32_t(PixelFormatFlagBits::BumpDUDV)) {
        if (16 == pf.m_bitCount) {
            if (pf.m_RBitMask == 0x00ff && pf.m_GBitMask == 0xff00 &&
                pf.m_BBitMask == 0x0000 && pf.m_ABitMask == 0x0000) {
                return DXGIFormat::R8G8_SNorm;
            }
        }
        if (32 == pf.m_bitCount) {
            if (pf.m_RBitMask == 0x000000ff && pf.m_GBitMask == 0x0000ff00 &&
                pf.m_BBitMask == 0x00ff0000 && pf.m_ABitMask == 0xff000000) {
                return DXGIFormat::R8G8B8A8_SNorm;
            }
            if (pf.m_RBitMask == 0x0000ffff && pf.m_GBitMask == 0xffff0000 &&
                pf.m_BBitMask == 0x00000000 && pf.m_ABitMask == 0x00000000) {
                return DXGIFormat::R16G16_SNorm;
            }
        }
    } else if (pf.m_flags & uint32_t(PixelFormatFlagBits::FourCC)) {
        if (MakeFourCC('D', 'X', 'T', '1') == pf.m_fourCC) {
            return DXGIFormat::BC1_UNorm;
        }
        if (MakeFourCC('D', 'X', 'T', '3') == pf.m_fourCC) {
            return DXGIFormat::BC2_UNorm;
        }
        if (MakeFourCC('D', 'X', 'T', '5') == pf.m_fourCC) {
            return DXGIFormat::BC3_UNorm;
        }

        if (MakeFourCC('D', 'X', 'T', '4') == pf.m_fourCC) {
            return DXGIFormat::BC2_UNorm;
        }
        if (MakeFourCC('D', 'X', 'T', '5') == pf.m_fourCC) {
            return DXGIFormat::BC3_UNorm;
        }

        if (MakeFourCC('A', 'T', 'I', '1') == pf.m_fourCC) {
            return DXGIFormat::BC4_UNorm;
        }
        if (MakeFourCC('B', 'C', '4', 'U') == pf.m_fourCC) {
            return DXGIFormat::BC4_UNorm;
        }
        if (MakeFourCC('B', 'C', '4', 'S') == pf.m_fourCC) {
            return DXGIFormat::BC4_SNorm;
        }

        if (MakeFourCC('A', 'T', 'I', '2') == pf.m_fourCC) {
            return DXGIFormat::BC5_UNorm;
        }
        if (MakeFourCC('B', 'C', '5', 'U') == pf.m_fourCC) {
            return DXGIFormat::BC5_UNorm;
        }
        if (MakeFourCC('B', 'C', '5', 'S') == pf.m_fourCC) {
            return DXGIFormat::BC5_SNorm;
        }

        if (MakeFourCC('R', 'G', 'B', 'G') == pf.m_fourCC) {
            return DXGIFormat::R8G8_B8G8_UNorm;
        }
        if (MakeFourCC('G', 'R', 'G', 'B') == pf.m_fourCC) {
            return DXGIFormat::G8R8_G8B8_UNorm;
        }

        if (MakeFourCC('Y', 'U', 'Y', '2') == pf.m_fourCC) {
            return DXGIFormat::YUY2;
        }

        switch (pf.m_fourCC) {
            case 36:
                return DXGIFormat::R16G16B16A16_UNorm;
            case 110:
                return DXGIFormat::R16G16B16A16_SNorm;
            case 111:
                return DXGIFormat::R16_Float;
            case 112:
                return DXGIFormat::R16G16_Float;
            case 113:
                return DXGIFormat::R16G16B16A16_Float;
            case 114:
                return DXGIFormat::R32_Float;
            case 115:
                return DXGIFormat::R32G32_Float;
            case 116:
                return DXGIFormat::R32G32B32A32_Float;
        }
    }

    return DXGIFormat::Unknown;
}

uint32_t DDSFile::GetBitsPerPixel(DXGIFormat fmt) {
    switch (fmt) {
        case DXGIFormat::R32G32B32A32_Typeless:
        case DXGIFormat::R32G32B32A32_Float:
        case DXGIFormat::R32G32B32A32_UInt:
        case DXGIFormat::R32G32B32A32_SInt:
            return 128;

        case DXGIFormat::R32G32B32_Typeless:
        case DXGIFormat::R32G32B32_Float:
        case DXGIFormat::R32G32B32_UInt:
        case DXGIFormat::R32G32B32_SInt:
            return 96;

        case DXGIFormat::R16G16B16A16_Typeless:
        case DXGIFormat::R16G16B16A16_Float:
        case DXGIFormat::R16G16B16A16_UNorm:
        case DXGIFormat::R16G16B16A16_UInt:
        case DXGIFormat::R16G16B16A16_SNorm:
        case DXGIFormat::R16G16B16A16_SInt:
        case DXGIFormat::R32G32_Typeless:
        case DXGIFormat::R32G32_Float:
        case DXGIFormat::R32G32_UInt:
        case DXGIFormat::R32G32_SInt:
        case DXGIFormat::R32G8X24_Typeless:
        case DXGIFormat::D32_Float_S8X24_UInt:
        case DXGIFormat::R32_Float_X8X24_Typeless:
        case DXGIFormat::X32_Typeless_G8X24_UInt:
        case DXGIFormat::Y416:
        case DXGIFormat::Y210:
        case DXGIFormat::Y216:
            return 64;

        case DXGIFormat::R10G10B10A2_Typeless:
        case DXGIFormat::R10G10B10A2_UNorm:
        case DXGIFormat::R10G10B10A2_UInt:
        case DXGIFormat::R11G11B10_Float:
        case DXGIFormat::R8G8B8A8_Typeless:
        case DXGIFormat::R8G8B8A8_UNorm:
        case DXGIFormat::R8G8B8A8_UNorm_SRGB:
        case DXGIFormat::R8G8B8A8_UInt:
        case DXGIFormat::R8G8B8A8_SNorm:
        case DXGIFormat::R8G8B8A8_SInt:
        case DXGIFormat::R16G16_Typeless:
        case DXGIFormat::R16G16_Float:
        case DXGIFormat::R16G16_UNorm:
        case DXGIFormat::R16G16_UInt:
        case DXGIFormat::R16G16_SNorm:
        case DXGIFormat::R16G16_SInt:
        case DXGIFormat::R32_Typeless:
        case DXGIFormat::D32_Float:
        case DXGIFormat::R32_Float:
        case DXGIFormat::R32_UInt:
        case DXGIFormat::R32_SInt:
        case DXGIFormat::R24G8_Typeless:
        case DXGIFormat::D24_UNorm_S8_UInt:
        case DXGIFormat::R24_UNorm_X8_Typeless:
        case DXGIFormat::X24_Typeless_G8_UInt:
        case DXGIFormat::R9G9B9E5_SHAREDEXP:
        case DXGIFormat::R8G8_B8G8_UNorm:
        case DXGIFormat::G8R8_G8B8_UNorm:
        case DXGIFormat::B8G8R8A8_UNorm:
        case DXGIFormat::B8G8R8X8_UNorm:
        case DXGIFormat::R10G10B10_XR_BIAS_A2_UNorm:
        case DXGIFormat::B8G8R8A8_Typeless:
        case DXGIFormat::B8G8R8A8_UNorm_SRGB:
        case DXGIFormat::B8G8R8X8_Typeless:
        case DXGIFormat::B8G8R8X8_UNorm_SRGB:
        case DXGIFormat::AYUV:
        case DXGIFormat::Y410:
        case DXGIFormat::YUY2:
            return 32;

        case DXGIFormat::P010:
        case DXGIFormat::P016:
            return 24;

        case DXGIFormat::R8G8_Typeless:
        case DXGIFormat::R8G8_UNorm:
        case DXGIFormat::R8G8_UInt:
        case DXGIFormat::R8G8_SNorm:
        case DXGIFormat::R8G8_SInt:
        case DXGIFormat::R16_Typeless:
        case DXGIFormat::R16_Float:
        case DXGIFormat::D16_UNorm:
        case DXGIFormat::R16_UNorm:
        case DXGIFormat::R16_UInt:
        case DXGIFormat::R16_SNorm:
        case DXGIFormat::R16_SInt:
        case DXGIFormat::B5G6R5_UNorm:
        case DXGIFormat::B5G5R5A1_UNorm:
        case DXGIFormat::A8P8:
        case DXGIFormat::B4G4R4A4_UNorm:
            return 16;

        case DXGIFormat::NV12:
        case DXGIFormat::YUV420_OPAQUE:
        case DXGIFormat::NV11:
            return 12;

        case DXGIFormat::R8_Typeless:
        case DXGIFormat::R8_UNorm:
        case DXGIFormat::R8_UInt:
        case DXGIFormat::R8_SNorm:
        case DXGIFormat::R8_SInt:
        case DXGIFormat::A8_UNorm:
        case DXGIFormat::AI44:
        case DXGIFormat::IA44:
        case DXGIFormat::P8:
            return 8;

        case DXGIFormat::R1_UNorm:
            return 1;

        case DXGIFormat::BC1_Typeless:
        case DXGIFormat::BC1_UNorm:
        case DXGIFormat::BC1_UNorm_SRGB:
        case DXGIFormat::BC4_Typeless:
        case DXGIFormat::BC4_UNorm:
        case DXGIFormat::BC4_SNorm:
            return 4;

        case DXGIFormat::BC2_Typeless:
        case DXGIFormat::BC2_UNorm:
        case DXGIFormat::BC2_UNorm_SRGB:
        case DXGIFormat::BC3_Typeless:
        case DXGIFormat::BC3_UNorm:
        case DXGIFormat::BC3_UNorm_SRGB:
        case DXGIFormat::BC5_Typeless:
        case DXGIFormat::BC5_UNorm:
        case DXGIFormat::BC5_SNorm:
        case DXGIFormat::BC6H_Typeless:
        case DXGIFormat::BC6H_UF16:
        case DXGIFormat::BC6H_SF16:
        case DXGIFormat::BC7_Typeless:
        case DXGIFormat::BC7_UNorm:
        case DXGIFormat::BC7_UNorm_SRGB:
            return 8;
        default:
            return 0;
    }
}

Result DDSFile::Load(const char* filepath) {
    std::ifstream ifs(filepath, std::ios_base::binary);
    if (!ifs.is_open()) {
        return Result::ErrorFileOpen;
    }

    return Load(ifs);
}

Result DDSFile::Load(std::istream& input) {
    m_dds.clear();

    input.seekg(0, std::ios_base::beg);
    auto begPos = input.tellg();
    input.seekg(0, std::ios_base::end);
    auto endPos = input.tellg();
    input.seekg(0, std::ios_base::beg);

    auto fileSize = endPos - begPos;
    if (fileSize == 0) {
        return Result::ErrorRead;
    }
    std::vector<uint8_t> dds(fileSize);

    input.read(reinterpret_cast<char*>(dds.data()), fileSize);

    if (input.bad()) {
        return Result::ErrorRead;
    }

    return Load(std::move(dds));
}

Result DDSFile::Load(const uint8_t* data, size_t size) {
    std::vector<uint8_t> dds(data, data + size);
    return Load(std::move(dds));
}

Result DDSFile::Load(std::vector<uint8_t>&& dds) {
    m_dds.clear();

    if (dds.size() < 4) {
        return Result::ErrorSize;
    }

    for (int i = 0; i < 4; i++) {
        if (dds[i] != Magic[i]) {
            return Result::ErrorMagicWord;
        }
    }

    if ((sizeof(uint32_t) + sizeof(Header)) >= dds.size()) {
        return Result::ErrorSize;
    }
    auto header =
        reinterpret_cast<const Header*>(dds.data() + sizeof(uint32_t));

    if (header->m_size != sizeof(Header) ||
        header->m_pixelFormat.m_size != sizeof(PixelFormat)) {
        return Result::ErrorVerify;
    }
    bool dxt10Header = false;
    if ((header->m_pixelFormat.m_flags &
         uint32_t(PixelFormatFlagBits::FourCC)) &&
        (MakeFourCC('D', 'X', '1', '0') == header->m_pixelFormat.m_fourCC)) {
        if ((sizeof(uint32_t) + sizeof(Header) + sizeof(HeaderDXT10)) >=
            dds.size()) {
            return Result::ErrorSize;
        }
        dxt10Header = true;
    }
    ptrdiff_t offset = sizeof(uint32_t) + sizeof(Header) +
                       (dxt10Header ? sizeof(HeaderDXT10) : 0);

    m_height = header->m_height;
    m_width = header->m_width;
    m_texDim = TextureDimension::Unknown;
    m_arraySize = 1;
    m_format = DXGIFormat::Unknown;
    m_isCubemap = false;
    m_mipCount = header->m_mipMapCount;
    if (0 == m_mipCount) {
        m_mipCount = 1;
    }

    if (dxt10Header) {
        auto dxt10Header = reinterpret_cast<const HeaderDXT10*>(
            reinterpret_cast<const char*>(header) + sizeof(Header));

        m_arraySize = dxt10Header->m_arraySize;
        if (m_arraySize == 0) {
            return Result::ErrorInvalidData;
        }

        switch (dxt10Header->m_format) {
            case DXGIFormat::AI44:
            case DXGIFormat::IA44:
            case DXGIFormat::P8:
            case DXGIFormat::A8P8:
                return Result::ErrorNotSupported;
            default:
                if (GetBitsPerPixel(dxt10Header->m_format) == 0) {
                    return Result::ErrorNotSupported;
                }
        }

        m_format = dxt10Header->m_format;

        switch (dxt10Header->m_resourceDimension) {
            case TextureDimension::Texture1D:
                if ((header->m_flags & uint32_t(HeaderFlagBits::Height) &&
                     (m_height != 1))) {
                    return Result::ErrorInvalidData;
                }
                m_height = m_depth = 1;
                break;
            case TextureDimension::Texture2D:
                if (dxt10Header->m_miscFlag &
                    uint32_t(DXT10MiscFlagBits::TextureCube)) {
                    m_arraySize *= 6;
                    m_isCubemap = true;
                }
                m_depth = 1;
                break;
            case TextureDimension::Texture3D:
                if (!(header->m_flags & uint32_t(HeaderFlagBits::Volume))) {
                    return Result::ErrorInvalidData;
                }
                if (m_arraySize > 1) {
                    return Result::ErrorNotSupported;
                }
                break;
            default:
                return Result::ErrorNotSupported;
        }

        m_texDim = dxt10Header->m_resourceDimension;
    } else {
        m_format = GetDXGIFormat(header->m_pixelFormat);
        if (m_format == DXGIFormat::Unknown) {
            return Result::ErrorNotSupported;
        }

        if (header->m_flags & uint32_t(HeaderFlagBits::Volume)) {
            m_texDim = TextureDimension::Texture3D;
        } else {
            auto caps2 = header->m_caps2 &
                         uint32_t(HeaderCaps2FlagBits::CubemapAllFaces);
            if (caps2) {
                if (caps2 != uint32_t(HeaderCaps2FlagBits::CubemapAllFaces)) {
                    return Result::ErrorNotSupported;
                }
                m_arraySize = 6;
                m_isCubemap = true;
            }

            m_depth = 1;
            m_texDim = TextureDimension::Texture2D;
        }
    }

    std::vector<ImageData> imageDatas(m_mipCount * m_arraySize);
    uint8_t* srcBits = dds.data() + offset;
    uint8_t* endBits = dds.data() + dds.size();
    uint32_t idx = 0;
    for (uint32_t j = 0; j < m_arraySize; j++) {
        uint32_t w = m_width;
        uint32_t h = m_height;
        uint32_t d = m_depth;
        for (uint32_t i = 0; i < m_mipCount; i++) {
            uint32_t numBytes;
            uint32_t rowBytes;
            GetImageInfo(w, h, m_format, &numBytes, &rowBytes, nullptr);

            imageDatas[idx].m_width = w;
            imageDatas[idx].m_height = h;
            imageDatas[idx].m_depth = d;
            imageDatas[idx].m_mem = srcBits;
            imageDatas[idx].m_memPitch = rowBytes;
            imageDatas[idx].m_memSlicePitch = numBytes;
            idx++;

            if (srcBits + (numBytes * d) > endBits) {
                return Result::ErrorInvalidData;
            }
            srcBits += numBytes * d;
            w = std::max<uint32_t>(1, w / 2);
            h = std::max<uint32_t>(1, h / 2);
            d = std::max<uint32_t>(1, d / 2);
        }
    }

    m_dds = std::move(dds);
    m_imageDatas = std::move(imageDatas);

    return Result::Success;
}

void DDSFile::GetImageInfo(uint32_t w, uint32_t h, DXGIFormat fmt,
                           uint32_t* outNumBytes, uint32_t* outRowBytes,
                           uint32_t* outNumRows) {
    uint32_t numBytes = 0;
    uint32_t rowBytes = 0;
    uint32_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    uint32_t bpe = 0;
    switch (fmt) {
        case DXGIFormat::BC1_Typeless:
        case DXGIFormat::BC1_UNorm:
        case DXGIFormat::BC1_UNorm_SRGB:
        case DXGIFormat::BC4_Typeless:
        case DXGIFormat::BC4_UNorm:
        case DXGIFormat::BC4_SNorm:
            bc = true;
            bpe = 8;
            break;

        case DXGIFormat::BC2_Typeless:
        case DXGIFormat::BC2_UNorm:
        case DXGIFormat::BC2_UNorm_SRGB:
        case DXGIFormat::BC3_Typeless:
        case DXGIFormat::BC3_UNorm:
        case DXGIFormat::BC3_UNorm_SRGB:
        case DXGIFormat::BC5_Typeless:
        case DXGIFormat::BC5_UNorm:
        case DXGIFormat::BC5_SNorm:
        case DXGIFormat::BC6H_Typeless:
        case DXGIFormat::BC6H_UF16:
        case DXGIFormat::BC6H_SF16:
        case DXGIFormat::BC7_Typeless:
        case DXGIFormat::BC7_UNorm:
        case DXGIFormat::BC7_UNorm_SRGB:
            bc = true;
            bpe = 16;
            break;

        case DXGIFormat::R8G8_B8G8_UNorm:
        case DXGIFormat::G8R8_G8B8_UNorm:
        case DXGIFormat::YUY2:
            packed = true;
            bpe = 4;
            break;

        case DXGIFormat::Y210:
        case DXGIFormat::Y216:
            packed = true;
            bpe = 8;
            break;

        case DXGIFormat::NV12:
        case DXGIFormat::YUV420_OPAQUE:
            planar = true;
            bpe = 2;
            break;

        case DXGIFormat::P010:
        case DXGIFormat::P016:
            planar = true;
            bpe = 4;
            break;
        default:
            break;
    }

    if (bc) {
        uint32_t numBlocksWide = 0;
        if (w > 0) {
            numBlocksWide = std::max<uint32_t>(1, (w + 3) / 4);
        }
        uint32_t numBlocksHigh = 0;
        if (h > 0) {
            numBlocksHigh = std::max<uint32_t>(1, (h + 3) / 4);
        }
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    } else if (packed) {
        rowBytes = ((w + 1) >> 1) * bpe;
        numRows = h;
        numBytes = rowBytes * h;
    } else if (fmt == DXGIFormat::NV11) {
        rowBytes = ((w + 3) >> 2) * 4;
        numRows = h * 2;
        numBytes = rowBytes + numRows;
    } else if (planar) {
        rowBytes = ((w + 1) >> 1) * bpe;
        numBytes = (rowBytes * h) + ((rowBytes * h + 1) >> 1);
        numRows = h + ((h + 1) >> 1);
    } else {
        uint32_t bpp = GetBitsPerPixel(fmt);
        rowBytes = (w * bpp + 7) / 8;
        numRows = h;
        numBytes = rowBytes * h;
    }

    if (outNumBytes) {
        *outNumBytes = numBytes;
    }
    if (outRowBytes) {
        *outRowBytes = rowBytes;
    }
    if (outNumRows) {
        *outNumRows = numRows;
    }
}

bool DDSFile::Flip() {
    if (IsCompressed(m_format)) {
        for (auto& imageData : m_imageDatas) {
            if (!FlipCompressedImage(imageData)) {
                return false;
            }
        }
    } else {
        for (auto& imageData : m_imageDatas) {
            if (!FlipImage(imageData)) {
                return false;
            }
        }
    }
    return true;
}

bool DDSFile::FlipImage(ImageData& imageData) {
    for (uint32_t y = 0; y < imageData.m_height / 2; y++) {
        auto line0 = (uint8_t*)imageData.m_mem + y * imageData.m_memPitch;
        auto line1 = (uint8_t*)imageData.m_mem +
                     (imageData.m_height - y - 1) * imageData.m_memPitch;
        for (uint32_t i = 0; i < imageData.m_memPitch; i++) {
            std::swap(*line0, *line1);
            line0++;
            line1++;
        }
    }
    return true;
}

bool DDSFile::FlipCompressedImage(ImageData& imageData) {
    if (DXGIFormat::BC1_Typeless == m_format ||
        DXGIFormat::BC1_UNorm == m_format ||
        DXGIFormat::BC1_UNorm_SRGB == m_format) {
        FlipCompressedImageBC1(imageData);
        return true;
    } else if (DXGIFormat::BC2_Typeless == m_format ||
               DXGIFormat::BC2_UNorm == m_format ||
               DXGIFormat::BC2_UNorm_SRGB == m_format) {
        FlipCompressedImageBC2(imageData);
        return true;
    } else if (DXGIFormat::BC3_Typeless == m_format ||
               DXGIFormat::BC3_UNorm == m_format ||
               DXGIFormat::BC3_UNorm_SRGB == m_format) {
        FlipCompressedImageBC3(imageData);
        return true;
    } else if (DXGIFormat::BC4_Typeless == m_format ||
               DXGIFormat::BC4_UNorm == m_format ||
               DXGIFormat::BC4_SNorm == m_format) {
        FlipCompressedImageBC4(imageData);
        return true;
    } else if (DXGIFormat::BC5_Typeless == m_format ||
               DXGIFormat::BC5_UNorm == m_format ||
               DXGIFormat::BC5_SNorm == m_format) {
        FlipCompressedImageBC5(imageData);
        return true;
    }
    return false;
}

void DDSFile::FlipCompressedImageBC1(ImageData& imageData) {
    uint32_t numXBlocks = (imageData.m_width + 3) / 4;
    uint32_t numYBlocks = (imageData.m_height + 3) / 4;
    if (imageData.m_height == 1) {
    } else if (imageData.m_height == 2) {
        auto blocks = (BC1Block*)imageData.m_mem;
        for (uint32_t x = 0; x < numXBlocks; x++) {
            auto block = blocks + x;
            std::swap(block->m_row0, block->m_row1);
            std::swap(block->m_row2, block->m_row3);
        }
    } else {
        for (uint32_t y = 0; y < (numYBlocks + 1) / 2; y++) {
            auto blocks0 = (BC1Block*)((uint8_t*)imageData.m_mem +
                                       imageData.m_memPitch * y);
            auto blocks1 =
                (BC1Block*)((uint8_t*)imageData.m_mem +
                            imageData.m_memPitch * (numYBlocks - y - 1));
            for (uint32_t x = 0; x < numXBlocks; x++) {
                auto block0 = blocks0 + x;
                auto block1 = blocks1 + x;
                if (blocks0 != blocks1) {
                    std::swap(block0->m_color0, block1->m_color0);
                    std::swap(block0->m_color1, block1->m_color1);
                    std::swap(block0->m_row0, block1->m_row3);
                    std::swap(block0->m_row1, block1->m_row2);
                    std::swap(block0->m_row2, block1->m_row1);
                    std::swap(block0->m_row3, block1->m_row0);
                } else {
                    std::swap(block0->m_row0, block0->m_row3);
                    std::swap(block0->m_row1, block0->m_row2);
                }
            }
        }
    }
}

void DDSFile::FlipCompressedImageBC2(ImageData& imageData) {
    uint32_t numXBlocks = (imageData.m_width + 3) / 4;
    uint32_t numYBlocks = (imageData.m_height + 3) / 4;
    if (imageData.m_height == 1) {
    } else if (imageData.m_height == 2) {
        auto blocks = (BC2Block*)imageData.m_mem;
        for (uint32_t x = 0; x < numXBlocks; x++) {
            auto block = blocks + x;
            std::swap(block->m_alphaRow0, block->m_alphaRow1);
            std::swap(block->m_alphaRow2, block->m_alphaRow3);
            std::swap(block->m_row0, block->m_row1);
            std::swap(block->m_row2, block->m_row3);
        }
    } else {
        for (uint32_t y = 0; y < (numYBlocks + 1) / 2; y++) {
            auto blocks0 = (BC2Block*)((uint8_t*)imageData.m_mem +
                                       imageData.m_memPitch * y);
            auto blocks1 =
                (BC2Block*)((uint8_t*)imageData.m_mem +
                            imageData.m_memPitch * (numYBlocks - y - 1));
            for (uint32_t x = 0; x < numXBlocks; x++) {
                auto block0 = blocks0 + x;
                auto block1 = blocks1 + x;
                if (block0 != block1) {
                    std::swap(block0->m_alphaRow0, block1->m_alphaRow3);
                    std::swap(block0->m_alphaRow1, block1->m_alphaRow2);
                    std::swap(block0->m_alphaRow2, block1->m_alphaRow1);
                    std::swap(block0->m_alphaRow3, block1->m_alphaRow0);
                    std::swap(block0->m_color0, block1->m_color0);
                    std::swap(block0->m_color1, block1->m_color1);
                    std::swap(block0->m_row0, block1->m_row3);
                    std::swap(block0->m_row1, block1->m_row2);
                    std::swap(block0->m_row2, block1->m_row1);
                    std::swap(block0->m_row3, block1->m_row0);
                } else {
                    std::swap(block0->m_alphaRow0, block0->m_alphaRow3);
                    std::swap(block0->m_alphaRow1, block0->m_alphaRow2);
                    std::swap(block0->m_row0, block0->m_row3);
                    std::swap(block0->m_row1, block0->m_row2);
                }
            }
        }
    }
}

void DDSFile::FlipCompressedImageBC3(ImageData& imageData) {
    uint32_t numXBlocks = (imageData.m_width + 3) / 4;
    uint32_t numYBlocks = (imageData.m_height + 3) / 4;
    if (imageData.m_height == 1) {
    } else if (imageData.m_height == 2) {
        auto blocks = (BC3Block*)imageData.m_mem;
        for (uint32_t x = 0; x < numXBlocks; x++) {
            auto block = blocks + x;
            uint8_t r0 = (block->m_alphaR1 >> 4) | (block->m_alphaR2 << 4);
            uint8_t r1 = (block->m_alphaR2 >> 4) | (block->m_alphaR0 << 4);
            uint8_t r2 = (block->m_alphaR0 >> 4) | (block->m_alphaR1 << 4);
            uint8_t r3 = (block->m_alphaR4 >> 4) | (block->m_alphaR5 << 4);
            uint8_t r4 = (block->m_alphaR5 >> 4) | (block->m_alphaR3 << 4);
            uint8_t r5 = (block->m_alphaR3 >> 4) | (block->m_alphaR4 << 4);

            block->m_alphaR0 = r0;
            block->m_alphaR1 = r1;
            block->m_alphaR2 = r2;
            block->m_alphaR3 = r3;
            block->m_alphaR4 = r4;
            block->m_alphaR5 = r5;
            std::swap(block->m_row0, block->m_row1);
            std::swap(block->m_row2, block->m_row3);
        }
    } else {
        for (uint32_t y = 0; y < (numYBlocks + 1) / 2; y++) {
            auto blocks0 = (BC3Block*)((uint8_t*)imageData.m_mem +
                                       imageData.m_memPitch * y);
            auto blocks1 =
                (BC3Block*)((uint8_t*)imageData.m_mem +
                            imageData.m_memPitch * (numYBlocks - y - 1));
            for (uint32_t x = 0; x < numXBlocks; x++) {
                auto block0 = blocks0 + x;
                auto block1 = blocks1 + x;
                if (block0 != block1) {
                    std::swap(block0->m_alpha0, block1->m_alpha0);
                    std::swap(block0->m_alpha1, block1->m_alpha1);

                    uint8_t r0[6];
                    r0[0] = (block0->m_alphaR4 >> 4) | (block0->m_alphaR5 << 4);
                    r0[1] = (block0->m_alphaR5 >> 4) | (block0->m_alphaR3 << 4);
                    r0[2] = (block0->m_alphaR3 >> 4) | (block0->m_alphaR4 << 4);
                    r0[3] = (block0->m_alphaR1 >> 4) | (block0->m_alphaR2 << 4);
                    r0[4] = (block0->m_alphaR2 >> 4) | (block0->m_alphaR0 << 4);
                    r0[5] = (block0->m_alphaR0 >> 4) | (block0->m_alphaR1 << 4);
                    uint8_t r1[6];
                    r1[0] = (block1->m_alphaR4 >> 4) | (block1->m_alphaR5 << 4);
                    r1[1] = (block1->m_alphaR5 >> 4) | (block1->m_alphaR3 << 4);
                    r1[2] = (block1->m_alphaR3 >> 4) | (block1->m_alphaR4 << 4);
                    r1[3] = (block1->m_alphaR1 >> 4) | (block1->m_alphaR2 << 4);
                    r1[4] = (block1->m_alphaR2 >> 4) | (block1->m_alphaR0 << 4);
                    r1[5] = (block1->m_alphaR0 >> 4) | (block1->m_alphaR1 << 4);

                    block0->m_alphaR0 = r1[0];
                    block0->m_alphaR1 = r1[1];
                    block0->m_alphaR2 = r1[2];
                    block0->m_alphaR3 = r1[3];
                    block0->m_alphaR4 = r1[4];
                    block0->m_alphaR5 = r1[5];

                    block1->m_alphaR0 = r0[0];
                    block1->m_alphaR1 = r0[1];
                    block1->m_alphaR2 = r0[2];
                    block1->m_alphaR3 = r0[3];
                    block1->m_alphaR4 = r0[4];
                    block1->m_alphaR5 = r0[5];

                    std::swap(block0->m_color0, block1->m_color0);
                    std::swap(block0->m_color1, block1->m_color1);
                    std::swap(block0->m_row0, block1->m_row3);
                    std::swap(block0->m_row1, block1->m_row2);
                    std::swap(block0->m_row2, block1->m_row1);
                    std::swap(block0->m_row3, block1->m_row0);
                } else {
                    uint8_t r0[6];
                    r0[0] = (block0->m_alphaR4 >> 4) | (block0->m_alphaR5 << 4);
                    r0[1] = (block0->m_alphaR5 >> 4) | (block0->m_alphaR3 << 4);
                    r0[2] = (block0->m_alphaR3 >> 4) | (block0->m_alphaR4 << 4);
                    r0[3] = (block0->m_alphaR1 >> 4) | (block0->m_alphaR2 << 4);
                    r0[4] = (block0->m_alphaR2 >> 4) | (block0->m_alphaR0 << 4);
                    r0[5] = (block0->m_alphaR0 >> 4) | (block0->m_alphaR1 << 4);

                    block0->m_alphaR0 = r0[0];
                    block0->m_alphaR1 = r0[1];
                    block0->m_alphaR2 = r0[2];
                    block0->m_alphaR3 = r0[3];
                    block0->m_alphaR4 = r0[4];
                    block0->m_alphaR5 = r0[5];

                    std::swap(block0->m_row0, block0->m_row3);
                    std::swap(block0->m_row1, block0->m_row2);
                }
            }
        }
    }
}

void DDSFile::FlipCompressedImageBC4(ImageData& imageData) {
    uint32_t numXBlocks = (imageData.m_width + 3) / 4;
    uint32_t numYBlocks = (imageData.m_height + 3) / 4;
    if (imageData.m_height == 1) {
    } else if (imageData.m_height == 2) {
        auto blocks = (BC4Block*)imageData.m_mem;
        for (uint32_t x = 0; x < numXBlocks; x++) {
            auto block = blocks + x;
            uint8_t r0 = (block->m_redR1 >> 4) | (block->m_redR2 << 4);
            uint8_t r1 = (block->m_redR2 >> 4) | (block->m_redR0 << 4);
            uint8_t r2 = (block->m_redR0 >> 4) | (block->m_redR1 << 4);
            uint8_t r3 = (block->m_redR4 >> 4) | (block->m_redR5 << 4);
            uint8_t r4 = (block->m_redR5 >> 4) | (block->m_redR3 << 4);
            uint8_t r5 = (block->m_redR3 >> 4) | (block->m_redR4 << 4);

            block->m_redR0 = r0;
            block->m_redR1 = r1;
            block->m_redR2 = r2;
            block->m_redR3 = r3;
            block->m_redR4 = r4;
            block->m_redR5 = r5;
        }
    } else {
        for (uint32_t y = 0; y < (numYBlocks + 1) / 2; y++) {
            auto blocks0 = (BC4Block*)((uint8_t*)imageData.m_mem +
                                       imageData.m_memPitch * y);
            auto blocks1 =
                (BC4Block*)((uint8_t*)imageData.m_mem +
                            imageData.m_memPitch * (numYBlocks - y - 1));
            for (uint32_t x = 0; x < numXBlocks; x++) {
                auto block0 = blocks0 + x;
                auto block1 = blocks1 + x;
                if (block0 != block1) {
                    std::swap(block0->m_red0, block1->m_red0);
                    std::swap(block0->m_red1, block1->m_red1);

                    uint8_t r0[6];
                    r0[0] = (block0->m_redR4 >> 4) | (block0->m_redR5 << 4);
                    r0[1] = (block0->m_redR5 >> 4) | (block0->m_redR3 << 4);
                    r0[2] = (block0->m_redR3 >> 4) | (block0->m_redR4 << 4);
                    r0[3] = (block0->m_redR1 >> 4) | (block0->m_redR2 << 4);
                    r0[4] = (block0->m_redR2 >> 4) | (block0->m_redR0 << 4);
                    r0[5] = (block0->m_redR0 >> 4) | (block0->m_redR1 << 4);
                    uint8_t r1[6];
                    r1[0] = (block1->m_redR4 >> 4) | (block1->m_redR5 << 4);
                    r1[1] = (block1->m_redR5 >> 4) | (block1->m_redR3 << 4);
                    r1[2] = (block1->m_redR3 >> 4) | (block1->m_redR4 << 4);
                    r1[3] = (block1->m_redR1 >> 4) | (block1->m_redR2 << 4);
                    r1[4] = (block1->m_redR2 >> 4) | (block1->m_redR0 << 4);
                    r1[5] = (block1->m_redR0 >> 4) | (block1->m_redR1 << 4);

                    block0->m_redR0 = r1[0];
                    block0->m_redR1 = r1[1];
                    block0->m_redR2 = r1[2];
                    block0->m_redR3 = r1[3];
                    block0->m_redR4 = r1[4];
                    block0->m_redR5 = r1[5];

                    block1->m_redR0 = r0[0];
                    block1->m_redR1 = r0[1];
                    block1->m_redR2 = r0[2];
                    block1->m_redR3 = r0[3];
                    block1->m_redR4 = r0[4];
                    block1->m_redR5 = r0[5];

                } else {
                    uint8_t r0[6];
                    r0[0] = (block0->m_redR4 >> 4) | (block0->m_redR5 << 4);
                    r0[1] = (block0->m_redR5 >> 4) | (block0->m_redR3 << 4);
                    r0[2] = (block0->m_redR3 >> 4) | (block0->m_redR4 << 4);
                    r0[3] = (block0->m_redR1 >> 4) | (block0->m_redR2 << 4);
                    r0[4] = (block0->m_redR2 >> 4) | (block0->m_redR0 << 4);
                    r0[5] = (block0->m_redR0 >> 4) | (block0->m_redR1 << 4);

                    block0->m_redR0 = r0[0];
                    block0->m_redR1 = r0[1];
                    block0->m_redR2 = r0[2];
                    block0->m_redR3 = r0[3];
                    block0->m_redR4 = r0[4];
                    block0->m_redR5 = r0[5];
                }
            }
        }
    }
}

void DDSFile::FlipCompressedImageBC5(ImageData& imageData) {
    uint32_t numXBlocks = (imageData.m_width + 3) / 4;
    uint32_t numYBlocks = (imageData.m_height + 3) / 4;
    if (imageData.m_height == 1) {
    } else if (imageData.m_height == 2) {
        auto blocks = (BC5Block*)imageData.m_mem;
        for (uint32_t x = 0; x < numXBlocks; x++) {
            auto block = blocks + x;
            uint8_t r0 = (block->m_redR1 >> 4) | (block->m_redR2 << 4);
            uint8_t r1 = (block->m_redR2 >> 4) | (block->m_redR0 << 4);
            uint8_t r2 = (block->m_redR0 >> 4) | (block->m_redR1 << 4);
            uint8_t r3 = (block->m_redR4 >> 4) | (block->m_redR5 << 4);
            uint8_t r4 = (block->m_redR5 >> 4) | (block->m_redR3 << 4);
            uint8_t r5 = (block->m_redR3 >> 4) | (block->m_redR4 << 4);

            block->m_redR0 = r0;
            block->m_redR1 = r1;
            block->m_redR2 = r2;
            block->m_redR3 = r3;
            block->m_redR4 = r4;
            block->m_redR5 = r5;

            uint8_t g0 = (block->m_greenR1 >> 4) | (block->m_greenR2 << 4);
            uint8_t g1 = (block->m_greenR2 >> 4) | (block->m_greenR0 << 4);
            uint8_t g2 = (block->m_greenR0 >> 4) | (block->m_greenR1 << 4);
            uint8_t g3 = (block->m_greenR4 >> 4) | (block->m_greenR5 << 4);
            uint8_t g4 = (block->m_greenR5 >> 4) | (block->m_greenR3 << 4);
            uint8_t g5 = (block->m_greenR3 >> 4) | (block->m_greenR4 << 4);

            block->m_greenR0 = g0;
            block->m_greenR1 = g1;
            block->m_greenR2 = g2;
            block->m_greenR3 = g3;
            block->m_greenR4 = g4;
            block->m_greenR5 = g5;
        }
    } else {
        for (uint32_t y = 0; y < (numYBlocks + 1) / 2; y++) {
            auto blocks0 = (BC5Block*)((uint8_t*)imageData.m_mem +
                                       imageData.m_memPitch * y);
            auto blocks1 =
                (BC5Block*)((uint8_t*)imageData.m_mem +
                            imageData.m_memPitch * (numYBlocks - y - 1));
            for (uint32_t x = 0; x < numXBlocks; x++) {
                auto block0 = blocks0 + x;
                auto block1 = blocks1 + x;
                if (block0 != block1) {
                    std::swap(block0->m_red0, block1->m_red0);
                    std::swap(block0->m_red1, block1->m_red1);

                    uint8_t r0[6];
                    r0[0] = (block0->m_redR4 >> 4) | (block0->m_redR5 << 4);
                    r0[1] = (block0->m_redR5 >> 4) | (block0->m_redR3 << 4);
                    r0[2] = (block0->m_redR3 >> 4) | (block0->m_redR4 << 4);
                    r0[3] = (block0->m_redR1 >> 4) | (block0->m_redR2 << 4);
                    r0[4] = (block0->m_redR2 >> 4) | (block0->m_redR0 << 4);
                    r0[5] = (block0->m_redR0 >> 4) | (block0->m_redR1 << 4);
                    uint8_t r1[6];
                    r1[0] = (block1->m_redR4 >> 4) | (block1->m_redR5 << 4);
                    r1[1] = (block1->m_redR5 >> 4) | (block1->m_redR3 << 4);
                    r1[2] = (block1->m_redR3 >> 4) | (block1->m_redR4 << 4);
                    r1[3] = (block1->m_redR1 >> 4) | (block1->m_redR2 << 4);
                    r1[4] = (block1->m_redR2 >> 4) | (block1->m_redR0 << 4);
                    r1[5] = (block1->m_redR0 >> 4) | (block1->m_redR1 << 4);

                    block0->m_redR0 = r1[0];
                    block0->m_redR1 = r1[1];
                    block0->m_redR2 = r1[2];
                    block0->m_redR3 = r1[3];
                    block0->m_redR4 = r1[4];
                    block0->m_redR5 = r1[5];

                    block1->m_redR0 = r0[0];
                    block1->m_redR1 = r0[1];
                    block1->m_redR2 = r0[2];
                    block1->m_redR3 = r0[3];
                    block1->m_redR4 = r0[4];
                    block1->m_redR5 = r0[5];

                    std::swap(block0->m_green0, block1->m_green0);
                    std::swap(block0->m_green1, block1->m_green1);

                    uint8_t g0[6];
                    g0[0] = (block0->m_greenR4 >> 4) | (block0->m_greenR5 << 4);
                    g0[1] = (block0->m_greenR5 >> 4) | (block0->m_greenR3 << 4);
                    g0[2] = (block0->m_greenR3 >> 4) | (block0->m_greenR4 << 4);
                    g0[3] = (block0->m_greenR1 >> 4) | (block0->m_greenR2 << 4);
                    g0[4] = (block0->m_greenR2 >> 4) | (block0->m_greenR0 << 4);
                    g0[5] = (block0->m_greenR0 >> 4) | (block0->m_greenR1 << 4);
                    uint8_t g1[6];
                    g1[0] = (block1->m_greenR4 >> 4) | (block1->m_greenR5 << 4);
                    g1[1] = (block1->m_greenR5 >> 4) | (block1->m_greenR3 << 4);
                    g1[2] = (block1->m_greenR3 >> 4) | (block1->m_greenR4 << 4);
                    g1[3] = (block1->m_greenR1 >> 4) | (block1->m_greenR2 << 4);
                    g1[4] = (block1->m_greenR2 >> 4) | (block1->m_greenR0 << 4);
                    g1[5] = (block1->m_greenR0 >> 4) | (block1->m_greenR1 << 4);

                    block0->m_greenR0 = g1[0];
                    block0->m_greenR1 = g1[1];
                    block0->m_greenR2 = g1[2];
                    block0->m_greenR3 = g1[3];
                    block0->m_greenR4 = g1[4];
                    block0->m_greenR5 = g1[5];

                    block1->m_greenR0 = g0[0];
                    block1->m_greenR1 = g0[1];
                    block1->m_greenR2 = g0[2];
                    block1->m_greenR3 = g0[3];
                    block1->m_greenR4 = g0[4];
                    block1->m_greenR5 = g0[5];
                } else {
                    uint8_t r0[6];
                    r0[0] = (block0->m_redR4 >> 4) | (block0->m_redR5 << 4);
                    r0[1] = (block0->m_redR5 >> 4) | (block0->m_redR3 << 4);
                    r0[2] = (block0->m_redR3 >> 4) | (block0->m_redR4 << 4);
                    r0[3] = (block0->m_redR1 >> 4) | (block0->m_redR2 << 4);
                    r0[4] = (block0->m_redR2 >> 4) | (block0->m_redR0 << 4);
                    r0[5] = (block0->m_redR0 >> 4) | (block0->m_redR1 << 4);

                    block0->m_redR0 = r0[0];
                    block0->m_redR1 = r0[1];
                    block0->m_redR2 = r0[2];
                    block0->m_redR3 = r0[3];
                    block0->m_redR4 = r0[4];
                    block0->m_redR5 = r0[5];

                    uint8_t g0[6];
                    g0[0] = (block0->m_greenR4 >> 4) | (block0->m_greenR5 << 4);
                    g0[1] = (block0->m_greenR5 >> 4) | (block0->m_greenR3 << 4);
                    g0[2] = (block0->m_greenR3 >> 4) | (block0->m_greenR4 << 4);
                    g0[3] = (block0->m_greenR1 >> 4) | (block0->m_greenR2 << 4);
                    g0[4] = (block0->m_greenR2 >> 4) | (block0->m_greenR0 << 4);
                    g0[5] = (block0->m_greenR0 >> 4) | (block0->m_greenR1 << 4);

                    block0->m_greenR0 = g0[0];
                    block0->m_greenR1 = g0[1];
                    block0->m_greenR2 = g0[2];
                    block0->m_greenR3 = g0[3];
                    block0->m_greenR4 = g0[4];
                    block0->m_greenR5 = g0[5];
                }
            }
        }
    }
}

}  // namespace tinyddsloader



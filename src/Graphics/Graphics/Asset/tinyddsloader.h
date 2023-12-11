//
// Copyright(c) 2019 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
// https://github.com/benikabocha/tinyddsloader
//

#ifndef TINYDDSLOADER_H_
#define TINYDDSLOADER_H_

#include <iostream>
#include <vector>

namespace tinyddsloader {

enum Result {
    Success,
    ErrorFileOpen,
    ErrorRead,
    ErrorMagicWord,
    ErrorSize,
    ErrorVerify,
    ErrorNotSupported,
    ErrorInvalidData,
};

class DDSFile {
public:
    static const char Magic[4];

    enum class PixelFormatFlagBits : uint32_t {
        FourCC = 0x00000004,
        RGB = 0x00000040,
        RGBA = 0x00000041,
        Luminance = 0x00020000,
        LuminanceA = 0x00020001,
        AlphaPixels = 0x00000001,
        Alpha = 0x00000002,
        Palette8 = 0x00000020,
        Palette8A = 0x00000021,
        BumpDUDV = 0x00080000
    };

    enum class DXGIFormat : uint32_t {
        Unknown = 0,
        R32G32B32A32_Typeless = 1,
        R32G32B32A32_Float = 2,
        R32G32B32A32_UInt = 3,
        R32G32B32A32_SInt = 4,
        R32G32B32_Typeless = 5,
        R32G32B32_Float = 6,
        R32G32B32_UInt = 7,
        R32G32B32_SInt = 8,
        R16G16B16A16_Typeless = 9,
        R16G16B16A16_Float = 10,
        R16G16B16A16_UNorm = 11,
        R16G16B16A16_UInt = 12,
        R16G16B16A16_SNorm = 13,
        R16G16B16A16_SInt = 14,
        R32G32_Typeless = 15,
        R32G32_Float = 16,
        R32G32_UInt = 17,
        R32G32_SInt = 18,
        R32G8X24_Typeless = 19,
        D32_Float_S8X24_UInt = 20,
        R32_Float_X8X24_Typeless = 21,
        X32_Typeless_G8X24_UInt = 22,
        R10G10B10A2_Typeless = 23,
        R10G10B10A2_UNorm = 24,
        R10G10B10A2_UInt = 25,
        R11G11B10_Float = 26,
        R8G8B8A8_Typeless = 27,
        R8G8B8A8_UNorm = 28,
        R8G8B8A8_UNorm_SRGB = 29,
        R8G8B8A8_UInt = 30,
        R8G8B8A8_SNorm = 31,
        R8G8B8A8_SInt = 32,
        R16G16_Typeless = 33,
        R16G16_Float = 34,
        R16G16_UNorm = 35,
        R16G16_UInt = 36,
        R16G16_SNorm = 37,
        R16G16_SInt = 38,
        R32_Typeless = 39,
        D32_Float = 40,
        R32_Float = 41,
        R32_UInt = 42,
        R32_SInt = 43,
        R24G8_Typeless = 44,
        D24_UNorm_S8_UInt = 45,
        R24_UNorm_X8_Typeless = 46,
        X24_Typeless_G8_UInt = 47,
        R8G8_Typeless = 48,
        R8G8_UNorm = 49,
        R8G8_UInt = 50,
        R8G8_SNorm = 51,
        R8G8_SInt = 52,
        R16_Typeless = 53,
        R16_Float = 54,
        D16_UNorm = 55,
        R16_UNorm = 56,
        R16_UInt = 57,
        R16_SNorm = 58,
        R16_SInt = 59,
        R8_Typeless = 60,
        R8_UNorm = 61,
        R8_UInt = 62,
        R8_SNorm = 63,
        R8_SInt = 64,
        A8_UNorm = 65,
        R1_UNorm = 66,
        R9G9B9E5_SHAREDEXP = 67,
        R8G8_B8G8_UNorm = 68,
        G8R8_G8B8_UNorm = 69,
        BC1_Typeless = 70,
        BC1_UNorm = 71,
        BC1_UNorm_SRGB = 72,
        BC2_Typeless = 73,
        BC2_UNorm = 74,
        BC2_UNorm_SRGB = 75,
        BC3_Typeless = 76,
        BC3_UNorm = 77,
        BC3_UNorm_SRGB = 78,
        BC4_Typeless = 79,
        BC4_UNorm = 80,
        BC4_SNorm = 81,
        BC5_Typeless = 82,
        BC5_UNorm = 83,
        BC5_SNorm = 84,
        B5G6R5_UNorm = 85,
        B5G5R5A1_UNorm = 86,
        B8G8R8A8_UNorm = 87,
        B8G8R8X8_UNorm = 88,
        R10G10B10_XR_BIAS_A2_UNorm = 89,
        B8G8R8A8_Typeless = 90,
        B8G8R8A8_UNorm_SRGB = 91,
        B8G8R8X8_Typeless = 92,
        B8G8R8X8_UNorm_SRGB = 93,
        BC6H_Typeless = 94,
        BC6H_UF16 = 95,
        BC6H_SF16 = 96,
        BC7_Typeless = 97,
        BC7_UNorm = 98,
        BC7_UNorm_SRGB = 99,
        AYUV = 100,
        Y410 = 101,
        Y416 = 102,
        NV12 = 103,
        P010 = 104,
        P016 = 105,
        YUV420_OPAQUE = 106,
        YUY2 = 107,
        Y210 = 108,
        Y216 = 109,
        NV11 = 110,
        AI44 = 111,
        IA44 = 112,
        P8 = 113,
        A8P8 = 114,
        B4G4R4A4_UNorm = 115,
        P208 = 130,
        V208 = 131,
        V408 = 132,
    };

    enum class HeaderFlagBits : uint32_t {
        Height = 0x00000002,
        Width = 0x00000004,
        Texture = 0x00001007,
        Mipmap = 0x00020000,
        Volume = 0x00800000,
        Pitch = 0x00000008,
        LinearSize = 0x00080000,
    };

    enum class HeaderCaps2FlagBits : uint32_t {
        CubemapPositiveX = 0x00000600,
        CubemapNegativeX = 0x00000a00,
        CubemapPositiveY = 0x00001200,
        CubemapNegativeY = 0x00002200,
        CubemapPositiveZ = 0x00004200,
        CubemapNegativeZ = 0x00008200,
        CubemapAllFaces = CubemapPositiveX | CubemapNegativeX |
                          CubemapPositiveY | CubemapNegativeY |
                          CubemapPositiveZ | CubemapNegativeZ,
        Volume = 0x00200000,
    };

    struct PixelFormat {
        uint32_t m_size;
        uint32_t m_flags;
        uint32_t m_fourCC;
        uint32_t m_bitCount;
        uint32_t m_RBitMask;
        uint32_t m_GBitMask;
        uint32_t m_BBitMask;
        uint32_t m_ABitMask;
    };

    struct Header {
        uint32_t m_size;
        uint32_t m_flags;
        uint32_t m_height;
        uint32_t m_width;
        uint32_t m_pitchOrLinerSize;
        uint32_t m_depth;
        uint32_t m_mipMapCount;
        uint32_t m_reserved1[11];
        PixelFormat m_pixelFormat;
        uint32_t m_caps;
        uint32_t m_caps2;
        uint32_t m_caps3;
        uint32_t m_caps4;
        uint32_t m_reserved2;
    };

    enum class TextureDimension : uint32_t {
        Unknown = 0,
        Texture1D = 2,
        Texture2D = 3,
        Texture3D = 4
    };

    enum class DXT10MiscFlagBits : uint32_t { TextureCube = 0x4 };

    struct HeaderDXT10 {
        DXGIFormat m_format;
        TextureDimension m_resourceDimension;
        uint32_t m_miscFlag;
        uint32_t m_arraySize;
        uint32_t m_miscFlag2;
    };

    struct ImageData {
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_depth;
        void* m_mem;
        uint32_t m_memPitch;
        uint32_t m_memSlicePitch;
    };

    struct BC1Block {
        uint16_t m_color0;
        uint16_t m_color1;
        uint8_t m_row0;
        uint8_t m_row1;
        uint8_t m_row2;
        uint8_t m_row3;
    };

    struct BC2Block {
        uint16_t m_alphaRow0;
        uint16_t m_alphaRow1;
        uint16_t m_alphaRow2;
        uint16_t m_alphaRow3;
        uint16_t m_color0;
        uint16_t m_color1;
        uint8_t m_row0;
        uint8_t m_row1;
        uint8_t m_row2;
        uint8_t m_row3;
    };

    struct BC3Block {
        uint8_t m_alpha0;
        uint8_t m_alpha1;
        uint8_t m_alphaR0;
        uint8_t m_alphaR1;
        uint8_t m_alphaR2;
        uint8_t m_alphaR3;
        uint8_t m_alphaR4;
        uint8_t m_alphaR5;
        uint16_t m_color0;
        uint16_t m_color1;
        uint8_t m_row0;
        uint8_t m_row1;
        uint8_t m_row2;
        uint8_t m_row3;
    };

    struct BC4Block {
        uint8_t m_red0;
        uint8_t m_red1;
        uint8_t m_redR0;
        uint8_t m_redR1;
        uint8_t m_redR2;
        uint8_t m_redR3;
        uint8_t m_redR4;
        uint8_t m_redR5;
    };

    struct BC5Block {
        uint8_t m_red0;
        uint8_t m_red1;
        uint8_t m_redR0;
        uint8_t m_redR1;
        uint8_t m_redR2;
        uint8_t m_redR3;
        uint8_t m_redR4;
        uint8_t m_redR5;
        uint8_t m_green0;
        uint8_t m_green1;
        uint8_t m_greenR0;
        uint8_t m_greenR1;
        uint8_t m_greenR2;
        uint8_t m_greenR3;
        uint8_t m_greenR4;
        uint8_t m_greenR5;
    };

public:
    static bool IsCompressed(DXGIFormat fmt);
    static uint32_t MakeFourCC(char ch0, char ch1, char ch2, char ch3);
    static DXGIFormat GetDXGIFormat(const PixelFormat& pf);
    static uint32_t GetBitsPerPixel(DXGIFormat fmt);

    Result Load(const char* filepath);
    Result Load(std::istream& input);
    Result Load(const uint8_t* data, size_t size);
    Result Load(std::vector<uint8_t>&& dds);

    const ImageData* GetImageData(uint32_t mipIdx = 0,
                                  uint32_t arrayIdx = 0) const {
        if (mipIdx < m_mipCount && arrayIdx < m_arraySize) {
            return &m_imageDatas[m_mipCount * arrayIdx + mipIdx];
        }
        return nullptr;
    }

    bool Flip();

    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    uint32_t GetDepth() const { return m_depth; }
    uint32_t GetMipCount() const { return m_mipCount; }
    uint32_t GetArraySize() const { return m_arraySize; }
    DXGIFormat GetFormat() const { return m_format; }
    bool IsCubemap() const { return m_isCubemap; }
    TextureDimension GetTextureDimension() const { return m_texDim; }

private:
    void GetImageInfo(uint32_t w, uint32_t h, DXGIFormat fmt,
                      uint32_t* outNumBytes, uint32_t* outRowBytes,
                      uint32_t* outNumRows);
    bool FlipImage(ImageData& imageData);
    bool FlipCompressedImage(ImageData& imageData);
    void FlipCompressedImageBC1(ImageData& imageData);
    void FlipCompressedImageBC2(ImageData& imageData);
    void FlipCompressedImageBC3(ImageData& imageData);
    void FlipCompressedImageBC4(ImageData& imageData);
    void FlipCompressedImageBC5(ImageData& imageData);

private:
    std::vector<uint8_t> m_dds;
    std::vector<ImageData> m_imageDatas;

    uint32_t m_height;
    uint32_t m_width;
    uint32_t m_depth;
    uint32_t m_mipCount;
    uint32_t m_arraySize;
    DXGIFormat m_format;
    bool m_isCubemap;
    TextureDimension m_texDim;
};

}  // namespace tinyddsloader

#endif  // !TINYDDSLOADER_H_

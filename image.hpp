#pragma once

#include <cstdint>

namespace Gdiplus
{
class Bitmap;
}

class Image
{
public:
    Image(size_t width, size_t height);
    ~Image();

    bool Load(const wchar_t* filename);
    uint16_t GetPixel(size_t x, size_t y);

private:
    size_t m_width;
    size_t m_height;
    ULONG_PTR m_gdiplusToken;
    Gdiplus::Bitmap* m_image;
};

#pragma once

#include <cstdint>

namespace Gdiplus
{
class Bitmap;
}

class Image
{
public:
    Image();
    ~Image();

    bool Load(const wchar_t* filename);
    uint16_t GetPixel(size_t x, size_t y);

private:
    ULONG_PTR m_gdiplusToken;
    Gdiplus::Bitmap* m_image;
};

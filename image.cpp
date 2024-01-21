#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include "image.hpp"
#include "colors.hpp"

Image::Image(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
}

Image::~Image()
{
    Gdiplus::GdiplusShutdown(m_gdiplusToken);    
}

bool Image::Load(const wchar_t* filename)
{
    m_image = Gdiplus::Bitmap::FromFile(filename);

    std::cout << "Reported image width=" << m_image->GetWidth() << " height=" << m_image->GetHeight() << std::endl;
    return m_image->GetWidth() == m_width && m_image->GetHeight() == m_height;
}

uint16_t Image::GetPixel(size_t x, size_t y)
{
    Gdiplus::Color color;
    if (m_image->GetPixel(x, y, &color) != Gdiplus::Ok)
    {
        return colorBlack;
    }

    return RGB565(color.GetRed(), color.GetGreen(), color.GetBlue());
}

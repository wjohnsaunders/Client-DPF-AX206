#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include "image.hpp"
#include "colors.hpp"

Image::Image()
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
    return m_image->GetWidth() == 800 && m_image->GetHeight() == 480;
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

#pragma once

#include <cstdint>

class Display
{
public:
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual bool GetDimensions(size_t& width, size_t& height) = 0;
    virtual int GetMaxBrightness() const = 0;
    virtual void SetBrightness(int level) = 0;
    virtual bool MustBlitFullScreen() const = 0;
    virtual void Blit(size_t x0, size_t x1, size_t y0, size_t y1, uint16_t* image, size_t imageLen) = 0;
};

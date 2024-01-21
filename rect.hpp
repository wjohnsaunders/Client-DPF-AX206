#pragma once

#include <cstddef>

class Rect
{
public:
    Rect(size_t _x0, size_t _x1, size_t _y0, size_t _y1);
    Rect(const Rect& other);
    const Rect& operator=(const Rect& lhs);

    bool Intersects(Rect r) const;
    void Union(Rect r);
    size_t GetNumPixels() const;

    size_t x0{ 0 };
    size_t x1{ 0 };
    size_t y0{ 0 };
    size_t y1{ 0 };
};

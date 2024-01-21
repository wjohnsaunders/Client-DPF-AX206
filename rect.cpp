#include <algorithm>
#include "rect.hpp"

Rect::Rect(size_t _x0, size_t _x1, size_t _y0, size_t _y1)
    : x0(_x0), x1(_x1), y0(_y0), y1(_y1)
{
}

Rect::Rect(const Rect& other)
{
    x0 = other.x0;
    x1 = other.x1;
    y0 = other.y0;
    y1 = other.y1;
}

const Rect& Rect::operator=(const Rect& lhs)
{
    if (this != &lhs)
    {
        x0 = lhs.x0;
        x1 = lhs.x1;
        y0 = lhs.y0;
        y1 = lhs.y1;
    }
    return *this;
}

bool Rect::Intersects(Rect r) const
{
    return !(r.x1 < x0 || r.x0 > x1 || r.y1 < y0 || r.y0 > y1);
}

void Rect::Union(Rect r)
{
    x0 = std::min(x0, r.x0);
    x1 = std::max(x1, r.x1);
    y0 = std::min(y0, r.y0);
    y1 = std::max(y1, r.y1);
}

size_t Rect::GetNumPixels() const
{
    return (x1 - x0) * (y1 - y0);
}

#include <iostream>
#include <deque>
#include "Adafruit_GFX.h"
#include "displayax206.hpp"
#include "updateoptimizer.hpp"
#include "rect.hpp"
#include "colors.hpp"

//#define ANIMATED_COLOR_LINES

#ifdef ANIMATED_COLOR_LINES
void UpdatePoint(int& point, int& acceleration, int lowLimit, int highlimit)
{
    point += acceleration;
    if (point < lowLimit || point >= highlimit)
    {
        acceleration = -acceleration;
        point += acceleration;
    }
}
#endif

int main()
{
    DisplayAx206 ax206;

    if (!ax206.Open())
    {
        std::cout << "failed to open USB" << std::endl;
    }

    size_t width = 0;
    size_t height = 0;
    if (!ax206.GetDimensions(width, height))
    {
        std::cout << "Device get dimensions failed - replug USB connection" << std::endl;
        exit(1);
    }
    std::cout << "Reported LCD width=" << width << " height=" << height << std::endl;

    GFXcanvas16 canvas(width, height);
    UpdateOptimizer updater(width, height);

    canvas.fillScreen(colorGrey10);
    updater.TransferToDisplay(canvas.getBuffer(), ax206);

    ax206.SetBrightness(ax206.GetMaxBrightness());

    canvas.drawRect(0, 0, width, height, colorWhite);
    canvas.drawRect(1, 1, width - 2, height - 2, colorWhite);
    updater.TransferToDisplay(canvas.getBuffer(), ax206);

#ifdef ANIMATED_COLOR_LINES
    std::deque<std::pair<Rect, uint16_t>> history;

    int x0 = 19, y0 = 45, x1 = 745, y1 = 411;
    int x0a = 7, y0a = 5, x1a = 7, y1a = 5;

    for (;;)
    {
        UpdatePoint(x0, x0a, 2, 798);
        UpdatePoint(y0, y0a, 2, 478);
        UpdatePoint(x1, x1a, 2, 798);
        UpdatePoint(y1, y1a, 2, 478);
        const uint16_t color = rand() % 65536;

        history.push_back(std::make_pair(Rect(x0, x1, y0, y1), color));
        if (history.size() > 80)
        {
            history.pop_front();
        }

        canvas.fillRect(2, 2, width - 4, height - 4, colorGrey10);
        for (const auto& r : history)
        {
            canvas.drawLine(r.first.x0, r.first.y0, r.first.x1, r.first.y1, r.second);
        }

        updater.TransferToDisplay(canvas.getBuffer(), ax206);
    }
#else
    for (size_t y = 0; y < 128; ++y)
    {
        for (size_t x = 0; x < 128; ++x)
        {
            canvas.drawPixel(20 + x, 75 + y, RGB565(x * 2, y * 2, 0));
            canvas.drawPixel(336 + x, 75 + y, RGB565(x * 2, 0, y * 2));
            canvas.drawPixel(652 + x, 75 + y, RGB565(0, x * 2, y * 2));
        }

        canvas.drawFastHLine(140, 278 + y, 40, RGB565(y * 2, y * 2, y * 2));
        canvas.drawFastHLine(220, 278 + y, 40, RGB565(y * 2, 0, 0));
        canvas.drawFastHLine(300, 278 + y, 40, RGB565(0, y * 2, 0));
        canvas.drawFastHLine(380, 278 + y, 40, RGB565(0, 0, y * 2));
        canvas.drawFastHLine(460, 278 + y, 40, RGB565(y * 2, y * 2, 0));
        canvas.drawFastHLine(540, 278 + y, 40, RGB565(y * 2, 0, y * 2));
        canvas.drawFastHLine(620, 278 + y, 40, RGB565(0, y * 2, y * 2));
    }

    updater.TransferToDisplay(canvas.getBuffer(), ax206);
#endif

    ax206.Close();

    return 0;
}

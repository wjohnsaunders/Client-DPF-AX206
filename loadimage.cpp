#include <iostream>
#include <cstdlib>
#include "Adafruit_GFX.h"
#include "displayax206.hpp"
#include "updateoptimizer.hpp"
#include "image.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage:: loadimage <imagefilename>" << std::endl;
        exit(1);
    }

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

    const size_t filenameLen = strlen(argv[1]) + 1;
    wchar_t* filename = new wchar_t[filenameLen];
    mbstowcs(filename, argv[1], filenameLen);

    Image image;
    image.Load(filename);
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            canvas.drawPixel(x, y, image.GetPixel(x, y));
        }
    }

    updater.TransferToDisplay(canvas.getBuffer(), ax206);

    ax206.Close();

    return 0;
}

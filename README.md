# Sample Windows program for the AX206 based Digital Picture Frame

## Background

On eBay you can find a 5" 800x480 LCD with a USB connection that claims to be supported by
AIDA64 and lcd4linux. They are based on the AX206 SOC with some custom firmware that allows
the LCD to be accessed via USB commands. You can also find them in 3.5" and possibly other
sizes, with various resolutions.

## This Software

This software is a starting project, or a proof of concept if you will, that puts in place the
basics of a framebuffer, using Adafruit_GFX, and writing to the USB port, using libusb. The
program draws some color gradients, or an animation, and the payout is designedfor an 800x480 LCD.

In order to develop this software I used the following resources to understand the protocol
and to get it working.

- https://github.com/dreamlayers/dpf-ax the firmware in my device doesn't act exactly as you
  would expect from reading the code, perhaps the device is shipped with a much older version.

- https://github.com/plumbum/go2dpf my original code was a conversion of Go to C++, however
  I have done much refactoring since then.

- AIDA64 https://www.aida64.com/ when I couldn't understand why it wasn't working, I watched
  the USB transfers from AIDA64 to see what I was doing wrong.

- Wireshark https://www.wireshark.org/ for monitoring USB transfers from AIDA64. Don't bother
  trying to use any Microsofts tools for USB tracing, Wireshark is so simple to use and
  just works.

## What is missing

- The USB handling will probably crash and burn if you unplug the LCD while the program is
  running. I noticed that AIDA64 nicely handles this, and when you plug the device back in
  it will be detected and the screen will start showing. So I suspect this should be doable
  to add at some stage.

- Using CRTL-C to stop the program prevents in running again. Re-plug the USB cable to fix.
- Adafruit_GFX seems fairly basic. While functions exist for drawing bitmaps and text, there
  doesn't appear to be functions for loading the bitmaps and fonts from a file. I presume
  being targeted to an embedded system, there are probably tools to generate .c files from
  bitmaps and fonts.

- Running on linux has not been done, although it is probably not that big of a step.

## Building

I used w64devkit based on the MinGW-64 GCC compiler (https://github.com/skeeto/w64devkit),
CMake (https://cmake.org/) and Ninja (https://ninja-build.org/).

Unzip w64devkit to some
location and add the bin directory to your PATH. Install the Windows version of CMake.
Download the ninja.exe and copy to the bin directory of w64devkit.

Create a "build" directory inside the root of the source code.

Open a command prompt in the "build" directory.

Run **cmake -G "Ninja" ..** then run **ninja** and demo.exe and loadimage.exe will be built.

Note: Make sure the MinGW-64 gcc and g++ compilers are the first in your PATH so that CMake
picks it up. I initially had trouble because CMake prefered Clang over GCC.

## Code Description

demo.cpp - Demo for drawing to the bitmap and sending to the LCD. There is the option to draw
a color gradient, or display a "stix" animation.

loadimage.cpp - Tool to load a fixed 800x480 image onto the display. Supports file formats as
supported by Gdiplus. Must be 800x480 image, it doesn't scale.

display.hpp specifies an interface that all displays must implement. Only 1 display implemented
for now,

displayax206.{cpp|hpp} contains the code for communicating with the AX206 based digital picture
frame over USB.

updatestrategy.gpp specifies an interface for handling updates from the framebuffer to the display.
Only 1 update strategy implemented for now.

updateoptimizer.{cpp|hpp} - If the display allows it, determine areas that changed and only blit those
to the display. However it turns out that my LCD crashes if I blit anything other than the entire
framebuffer, so it falls back to full framebuffer blit if the display required that. However this
functionality may be possible with better firmwware on your display, or a different display type.

rect.{cpp|hpp} Utility class for holding the coordinates defining a rectangle.

image.{cpp|hpp} Utility class to load image files via Windows Gdiplus classes. Not the most portable
way, but the quickest way on Windows.

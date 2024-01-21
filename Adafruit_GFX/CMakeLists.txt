# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

set(SOURCES "Adafruit_GFX.cpp" "glcdfont.c")
add_library(Adafruit-GFX-Library STATIC ${SOURCES})
target_include_directories(Adafruit-GFX-Library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

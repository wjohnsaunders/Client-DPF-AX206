
cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_C_COMPILER_NAMES gcc)
set(CMAKE_CXX_COMPILER_NAMES g++)

project(LcdSensor CXX)

set(COMMON_SOURCES
    "updateoptimizer.cpp" "displayax206.cpp" "rect.cpp" "image.cpp"
)

add_subdirectory(libusb)
add_subdirectory(Adafruit_GFX)

add_executable(demo "demo.cpp" ${COMMON_SOURCES})
set_target_properties(demo PROPERTIES CXX_STANDARD 17)
target_link_libraries(demo PRIVATE libusb Adafruit-GFX-Library gdiplus)

add_executable(loadimage "loadimage.cpp" ${COMMON_SOURCES})
set_target_properties(loadimage PROPERTIES CXX_STANDARD 17)
target_link_libraries(loadimage PRIVATE libusb Adafruit-GFX-Library gdiplus)

file(COPY TestImage.jpg DESTINATION ${CMAKE_BINARY_DIR})

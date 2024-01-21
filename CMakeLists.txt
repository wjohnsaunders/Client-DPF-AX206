
cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_C_COMPILER_NAMES gcc)
set(CMAKE_CXX_COMPILER_NAMES g++)

project(LcdSensor CXX)

set(SOURCES
    "main.cpp" "updateoptimizer.cpp" "displayax206.cpp" "rect.cpp"
)

add_subdirectory(libusb)
add_subdirectory(Adafruit_GFX)

add_executable(prototype ${SOURCES})
set_target_properties(prototype PROPERTIES CXX_STANDARD 17)
target_link_libraries(prototype PRIVATE libusb Adafruit-GFX-Library)

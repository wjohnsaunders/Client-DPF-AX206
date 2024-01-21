add_library(libusb STATIC IMPORTED GLOBAL)
set_target_properties(libusb PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libusb-1.0.a)
target_include_directories(libusb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.19)

project(glasswindow
    DESCRIPTION "Glass window effect plugin for Hyprland"
    VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Hyprland
pkg_check_modules(hyprland REQUIRED IMPORTED_TARGET hyprland)
pkg_check_modules(hyprlang REQUIRED IMPORTED_TARGET hyprlang)
pkg_check_modules(hyprutils REQUIRED IMPORTED_TARGET hyprutils)
pkg_check_modules(pixman REQUIRED IMPORTED_TARGET pixman-1)
pkg_check_modules(libdrm REQUIRED IMPORTED_TARGET libdrm)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Create the plugin library
add_library(glasswindow SHARED
    src/glasswindow.cpp
)

# Set up include directories
target_include_directories(glasswindow PRIVATE
    include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Hyprland/src
)

# Link libraries
target_link_libraries(glasswindow
    PkgConfig::hyprland
    PkgConfig::hyprlang
    PkgConfig::hyprutils
    PkgConfig::pixman
    PkgConfig::libdrm
    OpenGL::GL
)

# Set compiler flags
target_compile_options(glasswindow PRIVATE
    -Wall -Wextra -Wno-unused-parameter -Wno-unused-value
    -Wno-missing-field-initializers -Wno-narrowing
)

# Plugin-specific definitions
target_compile_definitions(glasswindow PRIVATE
    WLR_USE_UNSTABLE
)

# Set up plugin properties
set_target_properties(glasswindow PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# Installation (optional)
install(TARGETS glasswindow
    LIBRARY DESTINATION lib
)
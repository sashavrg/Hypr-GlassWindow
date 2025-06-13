cmake_minimum_required(VERSION 3.16)
project(Hypr-GlassWindow)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRLAND REQUIRED hyprland)
pkg_check_modules(HYPRLANG REQUIRED hyprlang)
pkg_check_modules(HYPRUTILS REQUIRED hyprutils)
pkg_check_modules(PIXMAN REQUIRED pixman-1)
pkg_check_modules(DRM REQUIRED libdrm)
find_package(OpenGL REQUIRED)

# Add source files
set(SOURCES
    src/glasswindow.cpp
)

# Create the plugin library
add_library(glasswindow SHARED ${SOURCES})

# Add include directories
target_include_directories(glasswindow PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${HYPRLAND_INCLUDE_DIRS}
    ${HYPRLANG_INCLUDE_DIRS}
    ${HYPRUTILS_INCLUDE_DIRS}
    ${PIXMAN_INCLUDE_DIRS}
    ${DRM_INCLUDE_DIRS}
)

# Link against required libraries
target_link_libraries(glasswindow
    PRIVATE
    ${HYPRLAND_LIBRARIES}
    ${HYPRLANG_LIBRARIES}
    ${HYPRUTILS_LIBRARIES}
    ${PIXMAN_LIBRARIES}
    ${DRM_LIBRARIES}
    OpenGL::GL
)

# Set output directory
set_target_properties(glasswindow PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PREFIX ""
)

# Install rules
install(TARGETS glasswindow
    LIBRARY DESTINATION lib/hyprland/plugins
) 
cmake_minimum_required(VERSION 3.8)



add_compile_definitions(IMGUI_EXPORT)

# Add ImGui source files
file(GLOB_RECURSE IMGUI_SOURCES
    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_sdl2.h
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
)

file(GLOB_RECURSE PROJECT_HEADERS
    imgui.h
    imconfig.h
)

add_library(ImGui SHARED ${IMGUI_SOURCES} ${PROJECT_HEADERS})

target_include_directories(
    ImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ImGui SDL2::SDL2)

# Create executable

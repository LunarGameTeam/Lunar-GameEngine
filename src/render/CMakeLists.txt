# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

configure_file(${SOLUTION_CMAKE_SOURCE_PATH}/src/packages.config ${CMAKE_CURRENT_BINARY_DIR}/packages.config COPYONLY)
add_library(WinPixEventRuntime INTERFACE IMPORTED GLOBAL)
set_target_properties(WinPixEventRuntime PROPERTIES INTERFACE_LINK_LIBRARIES "${CMAKE_BINARY_DIR}/packages/WinPixEventRuntime.1.0.220124001/build/WinPixEventRuntime.targets")

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)

add_compile_definitions(RENDER_DEVICE_DX11_ENABLE)
add_compile_definitions(RENDER_DEVICE_OPENGL_ENABLE)
add_compile_definitions(RENDER_DEVICE_DX12_ENABLE)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group("${ALL_FILES}" DST_FILES)

add_library(render SHARED ${DST_FILES})

add_compile_definitions(IMGUI_EXPORT)
add_compile_definitions(RENDER_EXPORT)

add_dependencies(render core window)


target_link_libraries(render debug shaderc_combinedd glslangd)
target_link_libraries(render optimized shaderc_combined glslang)

target_link_libraries(render core window ${Vulkan_LIBRARY} unofficial::spirv-reflect::spirv-reflect  d3d12 dxgi dxguid dxcompiler d3dcompiler Microsoft::DirectXTK12 Microsoft::DirectXTex WinPixEventRuntime)

add_custom_command(TARGET render POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$ENV{VULKAN_SDK}/Bin/dxcompiler.dll"
        $<TARGET_FILE_DIR:render>)

add_custom_command(TARGET render POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${SOLUTION_CMAKE_SOURCE_PATH}/third-party/dxil.dll"
        $<TARGET_FILE_DIR:render>)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
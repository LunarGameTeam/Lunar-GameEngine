# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(${SOLUTION_CMAKE_SOURCE_PATH}/src/packages.config ${CMAKE_CURRENT_BINARY_DIR}/packages.config COPYONLY)
add_library(WinPixEventRuntime INTERFACE IMPORTED GLOBAL)
set_target_properties(WinPixEventRuntime PROPERTIES INTERFACE_LINK_LIBRARIES "${CMAKE_BINARY_DIR}/packages/WinPixEventRuntime.1.0.220124001/build/WinPixEventRuntime.targets")

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Graphics/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.cpp *.cc *.c *h) 

source_group(TREE "${SOLUTION_CMAKE_SOURCE_PATH}/include" PREFIX Public FILES ${PROJECT_HEADERS} )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX Private FILES ${PROJECT_CPPS} )

add_library(Graphics SHARED ${PROJECT_HEADERS} ${PROJECT_CPPS})

add_compile_definitions(RENDER_EXPORT)

add_dependencies(Graphics Core)

target_link_libraries(Graphics debug shaderc_combinedd glslangd)
target_link_libraries(Graphics optimized shaderc_combined glslang)

target_link_libraries(Graphics Core ${Vulkan_LIBRARY} unofficial::spirv-reflect::spirv-reflect  d3d12 dxgi dxguid dxcompiler d3dcompiler Microsoft::DirectXTK12 D3D12MemoryAllocator Microsoft::DirectXTex WinPixEventRuntime)

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "$ENV{VULKAN_SDK}/Bin/dxcompiler.dll"
        $<TARGET_FILE_DIR:Graphics>)

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${SOLUTION_CMAKE_SOURCE_PATH}/third-party/dxil.dll"
        $<TARGET_FILE_DIR:Graphics>)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
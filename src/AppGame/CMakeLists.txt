# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

add_executable(AppGame GameMain.cpp)

add_dependencies(AppGame ImGuiScript Graphics Game Core) 
target_link_libraries(AppGame Core)

set_target_properties(AppGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}"
    OUTPUT_NAME "Luna Game"   
)

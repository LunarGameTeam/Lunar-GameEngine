# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

add_executable(AppEditor EditorMain.cpp)

add_dependencies(AppEditor ImGuiScript Graphics Game Core ResourceImportBinding) 
target_link_libraries(AppEditor Core)


set_target_properties(AppEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}"
    OUTPUT_NAME "Luna Editor"   
)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

add_executable(app_editor main.cpp)

add_dependencies(app_editor editor game core) 
target_link_libraries(app_editor core)


set_target_properties(app_editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}"
    OUTPUT_NAME "Luna Editor"   
)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

add_executable(editor_exe main.cpp)

add_dependencies(editor_exe editor) 
target_link_libraries(editor_exe editor)


set_target_properties(editor_exe PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}"
    OUTPUT_NAME "LunaEditor"   
)

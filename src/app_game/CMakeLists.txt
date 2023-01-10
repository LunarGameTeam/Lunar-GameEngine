# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

add_executable(app_game main.cpp)

add_dependencies(app_game core game) 
target_link_libraries(app_game core)


set_target_properties(app_game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}"
    OUTPUT_NAME "Luna Game"   
)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Core/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.cpp *.cc *.c)

auto_header_group(Core "${PROJECT_HEADERS}" DST_FILES "")
auto_source_group(Core "${PROJECT_CPPS}" DST_HEADERS "")

add_library(Core SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

# add_compile_definitions(TRACY_EXPORTS)
# add_compile_definitions(IMGUI_EXPORT)
add_compile_definitions(CORE_EXPORT)
# add_compile_definitions(FMT_EXPORT)

add_dependencies(Core imgui Eigen3::Eigen JsonCpp::JsonCpp SDL2::SDL2)

if(LUNA_USING_DEBUG_PYTHON)
    target_link_libraries(Core PUBLIC python311_d)
else()
    target_link_libraries(Core PUBLIC python311)
endif()

target_link_libraries(Core PUBLIC Boost::boost bcrypt imgui JsonCpp::JsonCpp Eigen3::Eigen Tracy::TracyClient SDL2::SDL2)

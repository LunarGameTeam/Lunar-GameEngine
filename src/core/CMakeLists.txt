# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp *.cc *.c)

set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS} ${PROJECT_IXXS})
auto_source_group("${ALL_FILES}" DST_FILES)

add_library(core SHARED ${DST_FILES} ${NATVIS_PATH})

# add_compile_definitions(TRACY_EXPORTS)
# add_compile_definitions(IMGUI_EXPORT)
add_compile_definitions(CORE_EXPORT)
# add_compile_definitions(FMT_EXPORT)

add_dependencies(core imgui Eigen3::Eigen JsonCpp::JsonCpp SDL2::SDL2)

if(LUNA_USING_DEBUG_PYTHON)
    target_link_libraries(core PUBLIC python311_d)
else()
    target_link_libraries(core PUBLIC python311)
endif()

target_link_libraries(core PUBLIC Boost::boost bcrypt imgui JsonCpp::JsonCpp Eigen3::Eigen Tracy::TracyClient SDL2::SDL2)

# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息
cmake_policy(VERSION "3.24")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sdk/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(Lunar-Engine)

math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sdk/)

set(Python3_FIND_REGISTRY, LAST)

SET(NATVIS_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/src/NatvisFile.natvis
	${CMAKE_CURRENT_SOURCE_DIR}/src/Python.natvis)

SET(SOLUTION_CMAKE_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)


option(LUNA_USING_DEBUG_PYTHON "使用Debug版本的Python" OFF)

find_package (Python3 3.11 EXACT COMPONENTS Interpreter Development)

find_package(stb REQUIRED)
find_package(Threads REQUIRED)
find_package(eigen3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(tracy REQUIRED)
find_package(directxtk12 REQUIRED)
find_package(directxtex REQUIRED)
find_package(boost COMPONENTS system REQUIRED)
find_package(unofficial-spirv-reflect REQUIRED)
find_package(GTest REQUIRED)
find_package(Vulkan REQUIRED glslang glslangValidator glslc SPIRV-Tools shaderc_combined)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build/Compile/)
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})

set(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "" FORCE)

# FBX工具加入可选项
option(LUNA_USING_FBX_TOOLS "enable fbx tools" OFF)
option(LUNA_USING_ASAN "Enable Snitize Address" OFF)

if(LUNA_USING_DEBUG_PYTHON)
    message(STATUS "Using Debug Python")
    add_compile_definitions(LUNA_USING_DEBUG_PYTHON)
else()
    message(STATUS "Using Release Python")
endif()

if(LUNA_USING_ASAN)
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

#禁用VC的MinMax
add_compile_definitions(NOMINMAX)
# add_compile_definitions(TRACY_ENABLE)

add_compile_options(-std:c++latest)
add_compile_options(/MP)
add_compile_options(/bigobj)
add_compile_options(/Zc:char8_t)
add_compile_options(/GR)

add_definitions(-DPYTHON3_PATH="${Python3_RUNTIME_LIBRARY_DIRS}")

message(STATUS "python include dir: ${Python3_INCLUDE_DIRS}")
message(STATUS "python runtime dir: ${Python3_RUNTIME_LIBRARY_DIRS}")
message(STATUS "python libs: ${Python3_LIBRARIES}")
message(STATUS "python exe: ${Python3_EXECUTABLE}")
message(STATUS "vulkan glslang: ${Vulkan_glslang_LIBRARY}")
message(STATUS "vulkan: $ENV{VULKAN_SDK}")
message(STATUS "vulkan: ${Vulkan_shaderc_combined_LIBRARY}")
message(STATUS "vulkan: ${Vulkan_SPIRV-Tools_LIBRARY}")

include_directories(
	src
	${SDK_PATH}imgui
	${SDK_PATH}vcpkg/installed/x64-windows/include
	${SDK_PATH}imgui/backends
	${Python3_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

link_directories(
	${Python3_LIBRARY_DIRS}
	$ENV{VULKAN_SDK}/Lib
)

function(auto_source_group src_files dst_files)
    foreach(_source IN ITEMS ${src_files})
	 	if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${SOLUTION_CMAKE_SOURCE_PATH}/src" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_file_name "${_source_rel}" NAME)		
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
		if ("${_file_name}" MATCHES "win32_*")
			if(WIN32)
				message("Windows Source: ${_file_name}")
				list(APPEND dst_files ${_source})
				source_group("${_source_path_msvc}" FILES "${_source}")
			endif()
		else()
			if("${_file_name}" MATCHES "linux_*")				
				if(UNIX)
					list(APPEND dst_files ${_source})
					message("Linux Source: ${_file_name}")
					source_group("${_source_path_msvc}" FILES "${_source}")
				endif()
			else()
				list(APPEND dst_files ${_source})
				source_group("${_source_path_msvc}" FILES "${_source}")
			endif()
		endif()	
    endforeach()
	set(${dst_files} PARENT_SCOPE)
endfunction(auto_source_group)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src/core)
add_subdirectory(src/render)
add_subdirectory(src/window)
add_subdirectory(sdk/imgui)
add_subdirectory(src/engine)

add_subdirectory(src/editor)
add_subdirectory(src/editor_exe)

SET_PROPERTY(TARGET core PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET render PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET window PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET imgui PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET engine PROPERTY FOLDER "Runtime")

SET_PROPERTY(TARGET editor PROPERTY FOLDER "Editor")
SET_PROPERTY(TARGET editor_exe PROPERTY FOLDER "Editor")


add_subdirectory(test/core_test)
SET_PROPERTY(TARGET core_test PROPERTY FOLDER "Test")

if(LUNA_USING_FBX_TOOLS)
	add_subdirectory(src/lib/fbx_helper)
	SET_PROPERTY(TARGET fbx_helper PROPERTY FOLDER "Lib")
	add_subdirectory(test/fbx_test)
	SET_PROPERTY(TARGET fbx_test PROPERTY FOLDER "Test")
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT editor_exe)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/release)

install(TARGETS core render window engine editor editor_exe
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin)

install(FILES file(GET_RUNTIME_DEPENDENCIES EXECUTABLES $<TARGET_FILE:editor_exe>)
	DESTINATION bin)

install(DIRECTORY assets/ DESTINATION assets)
install(FILES bin/jsoncpp.dll DESTINATION bin)
install(FILES third-party/dxil.dll DESTINATION bin)		 
install(DIRECTORY assets/ DESTINATION assets)
install(DIRECTORY editor/ DESTINATION editor)
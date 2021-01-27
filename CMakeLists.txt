# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
cmake_policy(VERSION "3.8.0")
project(Lunar-Engine)

math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

set(SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sdk/)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(SOLUTION_CMAKE_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build/Compile/)
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})

include_directories(
	src/engine
	src/engine/lua
	${SDK_PATH}glm
	${SDK_PATH}googletest/googletest/include
	${SDK_PATH}jsoncpp/include
	${SDK_PATH}boost
	${SDK_PATH}DirectXTex
	${SDK_PATH}sol2/include
	${SDK_PATH}DirectXTK12
	${SDK_PATH}DirectXTK12/Src
	${SDK_PATH}DirectXTK12/Inc
)
IF(WIN32)
	link_directories(
	${LIBRARY_OUTPUT_PATH}/${CMAKE_INTDIR}
	${SDK_PATH}freetype/win64
	${SDK_PATH}/jsoncpp/build/lib/{$CMAKE_BUILD_TYPE}
	${SDK_PATH}boost/stage/lib
	${CMAKE_SOURCE_DIR}/bin/CMake/${CMAKE_INTDIR}
	)
ELSE()
	link_directories(
	${LIBRARY_OUTPUT_PATH}/
	${SDK_PATH}freetype/win64
	${SDK_PATH}/jsoncpp/build/lib/{$CMAKE_BUILD_TYPE}
	${SDK_PATH}boost/stage/lib
	${CMAKE_SOURCE_DIR}/bin/
	)
ENDIF()

add_compile_options("/std:c++latest")

function(auto_source_group)
    foreach(_source IN ITEMS ${ARGN})

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
				source_group("${_source_path_msvc}" FILES "${_source}")
			endif()
		else()
			if("${_file_name}" MATCHES "linux_*")				
				if(UNIX)
					message("Linux Source: ${_file_name}")
					source_group("${_source_path_msvc}" FILES "${_source}")
				endif()
			else()
				source_group("${_source_path_msvc}" FILES "${_source}")
			endif()
		endif()
        
    endforeach()
endfunction(auto_source_group)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")

add_subdirectory(src/engine/core)
add_subdirectory(src/engine/render)
add_subdirectory(src/engine/world)
add_subdirectory(src/engine/lua_bind)
add_subdirectory(src/engine/lua)

add_subdirectory(src/application)
add_subdirectory(src/editor)

add_subdirectory(src/test/core_test)
add_subdirectory(src/test/lua_bind_test)
add_subdirectory(src/test/world_test)


SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET_PROPERTY(TARGET core PROPERTY FOLDER "Runtime")

SET_PROPERTY(TARGET render PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET application PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET world PROPERTY FOLDER "Runtime")
SET_PROPERTY(TARGET lua_bind PROPERTY FOLDER "Runtime")

SET_PROPERTY(TARGET editor PROPERTY FOLDER "Editor")

add_subdirectory(sdk/googletest)
add_subdirectory(sdk/DirectXTex)
add_subdirectory(sdk/DirectXTK12)
add_subdirectory(sdk/jsoncpp)
add_subdirectory(sdk/sol2)

SET_PROPERTY(TARGET lua PROPERTY FOLDER "SDK")

SET_PROPERTY(TARGET DirectXTex PROPERTY FOLDER "SDK")
SET_PROPERTY(TARGET DirectXTK12 PROPERTY FOLDER "SDK")

SET_PROPERTY(TARGET jsoncpp_lib PROPERTY FOLDER "SDK")
SET_PROPERTY(TARGET jsoncpp_object PROPERTY FOLDER "SDK")
SET_PROPERTY(TARGET jsoncpp_static PROPERTY FOLDER "SDK")

SET_PROPERTY(TARGET texassemble PROPERTY FOLDER "SDK")
SET_PROPERTY(TARGET texconv PROPERTY FOLDER "SDK")
SET_PROPERTY(TARGET texdiag PROPERTY FOLDER "SDK")


SET_PROPERTY(TARGET core_test PROPERTY FOLDER "Test")
SET_PROPERTY(TARGET gtest PROPERTY FOLDER "Test")
SET_PROPERTY(TARGET lua_bind_test PROPERTY FOLDER "Test")
SET_PROPERTY(TARGET world_test PROPERTY FOLDER "Test")


set_property(TARGET application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}")
set_property(TARGET lua_bind_test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}")
set_property(TARGET core_test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_CMAKE_SOURCE_PATH}")
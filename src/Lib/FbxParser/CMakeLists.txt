# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)

# 项目信息
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROJECT_HEADERS "${SOLUTION_CMAKE_SOURCE_PATH}/include/Lib/FbxParser/*.h")
file(GLOB_RECURSE PROJECT_CPPS *.h *.cpp *.cc *.c)

auto_header_group(FbxParser "${PROJECT_HEADERS}" DST_FILES /Lib/FbxParser)
auto_source_group(FbxParser "${PROJECT_CPPS}" DST_HEADERS Lib/)

add_library(FbxParser SHARED ${DST_FILES} ${DST_HEADERS} ${NATVIS_PATH})

add_compile_definitions(FBXSDK_SHARED)

add_compile_definitions(FBX_EXPORT)

find_path(FBX_ROOT_DIR DOC "Path to FBX SDK folder" REQUIRED)

INCLUDE_DIRECTORIES(BEFORE "${FBX_ROOT_DIR}/include")
IF(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x64/release/libfbxsdk.lib)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x86/release/libfbxsdk.lib)
	endif()
ENDIF()

add_dependencies(FbxParser ImportScene)

target_link_libraries(FbxParser ImportScene ${FBX_LIB_DIR})

add_custom_command(TARGET FbxParser
  POST_BUILD  
  COMMAND xcopy ${FBX_ROOT_DIR}/lib/vs2019/x64/release\\*.dll "\"$(OutDir)\"" /Y
)
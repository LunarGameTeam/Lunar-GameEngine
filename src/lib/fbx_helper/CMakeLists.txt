# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.24)
# 项目信息

file(GLOB_RECURSE PROJECT_HEADERS *.h)
file(GLOB_RECURSE PROJECT_CPPS *.cpp)


set(ALL_FILES ${PROJECT_HEADERS} ${PROJECT_CPPS})

auto_source_group(fbx_helper "${ALL_FILES}" DST_FILES)

add_library(fbx_helper SHARED ${DST_FILES})
add_compile_definitions(FBXSDK_SHARED)

find_path(FBX_ROOT_DIR DOC "Path to FBX SDK folder" REQUIRED)

INCLUDE_DIRECTORIES(BEFORE "${FBX_ROOT_DIR}/include")
INCLUDE_DIRECTORIES(BEFORE "${LIBRARY_OUTPUT_PATH}/111")
IF(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x64/release/libfbxsdk.lib)
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(FBX_LIB_DIR ${FBX_ROOT_DIR}/lib/vs2019/x86/release/libfbxsdk.lib)
	endif()
ENDIF()
add_dependencies(fbx_helper core render)

target_link_libraries(fbx_helper core render ${FBX_LIB_DIR})

add_custom_command(TARGET fbx_helper
  POST_BUILD  
  COMMAND xcopy ${FBX_ROOT_DIR}/lib/vs2019/x64/release\\*.dll "\"$(OutDir)\"" /Y
)
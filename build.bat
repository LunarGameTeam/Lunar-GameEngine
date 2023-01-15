call sdk\vcpkg\bootstrap-vcpkg.bat

.\sdk\vcpkg\vcpkg.exe install stb:x64-windows
.\sdk\vcpkg\vcpkg.exe install eigen3:x64-windows
.\sdk\vcpkg\vcpkg.exe install SDL2[core,vulkan]:x64-windows
.\sdk\vcpkg\vcpkg.exe install jsoncpp:x64-windows
.\sdk\vcpkg\vcpkg.exe install tracy:x64-windows
.\sdk\vcpkg\vcpkg.exe install spirv-reflect:x64-windows
.\sdk\vcpkg\vcpkg.exe install boost-algorithm:x64-windows
.\sdk\vcpkg\vcpkg.exe install boost-uuid:x64-windows
.\sdk\vcpkg\vcpkg.exe install boost-lexical-cast:x64-windows
.\sdk\vcpkg\vcpkg.exe install gtest:x64-windows
.\sdk\vcpkg\vcpkg.exe install directxtk12:x64-windows
.\sdk\vcpkg\vcpkg.exe install directxtex:x64-windows
pause
mkdir build
echo begin cmake
cmake -D WIN32=1 -G "Visual Studio 17" -DBUILD_EXAMPLES=OFF -DBUILD_UNIT_TESTS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DJSON_WITH_EXAMPLE=False -DBUILD_TESTING=False -DBUILD_GMOCK=False -DJSONCPP_WITH_TESTS=False -A x64  -B ./build 
pause
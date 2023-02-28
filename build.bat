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
.\sdk\vcpkg\vcpkg.exe install tinyobjloader:x64-windows
.\sdk\vcpkg\vcpkg.exe install ms-gltf:x64-windows
pause
call CMakeGenerate.bat
pause
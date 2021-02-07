mkdir build
cmake -D WIN32=1 -G "Visual Studio 16" -DJSON_WITH_EXAMPLE=False -DBUILD_TESTING=False -DBUILD_GMOCK=False -DJSONCPP_WITH_TESTS=False -A x64 --build ./ -B ./build 
pause
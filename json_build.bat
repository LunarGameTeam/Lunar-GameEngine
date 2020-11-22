cd ..\jsoncpp
mkdir build
cmake -D WIN32=1 -G "Visual Studio 16" -A x64 --build ./ -B ./build
pause
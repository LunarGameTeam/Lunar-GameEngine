mkdir build
cmake -D WIN32=1 -G "Visual Studio 15" -A x64 --build ./ -B ./build
cd .\sdk\boost
call bootstrap.bat
mkdir build
b2 --address-model=64 architecture=x86 variant=debug link=static threading=multi runtime-link=shared --build-type=complete stage
cd ..\jsoncpp
mkdir build
cmake -D WIN32=1 -G "Visual Studio 15" -A x64 --build ./ -B ./build
pause
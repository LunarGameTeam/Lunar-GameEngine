cd sdk
cd boost
git submodule update --init --recursive tools/build tools/boost_install libs/config libs/headers libs/move libs/assert libs/static_assert libs/core libs/intrusive


git submodule update --init --recursive^
    libs/detail^
    libs/container^
    libs/throw_exception^
    libs/algorithm^
    libs/array^
    libs/unordered^
    libs/functional^
    libs/function^
    libs/utility^
    libs/type_traits^
    libs/bimap^
    libs/container_hash^
    libs/smart_ptr^
    libs/tuple^
    libs/predef^
    libs/preprocessor^
    libs/integer

call bootstrap.bat
mkdir build
b2 --address-model=64 architecture=x86 variant=debug link=static threading=multi runtime-link=shared --build-type=complete stage
pause
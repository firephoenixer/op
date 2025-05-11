@ECHO OFF
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
Title Debug

REM 设置BlackBone根目录
set BLACKBONE_ROOT=C:\libs\Blackbone

:BEGIN
mkdir build
cd build
mkdir msvc17
cd msvc17
set CMAKE_PREFIX_PATH=K:\Qt5\lib\cmake
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBLACKBONE_ROOT="%BLACKBONE_ROOT%" ..\..\  -G"Visual Studio 17 2022"
@echo msbuild Project.sln /p:Configuration=Debug /p:Platform=X64 /m
cd ..\..\
echo %cd%
echo ========================OK====================
pause
goto BEGIN

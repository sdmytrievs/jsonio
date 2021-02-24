@echo off
setlocal EnableDelayedExpansion 

set LOCALINSTALL=D:/usr_vs/local
set PROGFILES=%ProgramFiles%
if not "%ProgramFiles(x86)%" == "" set PROGFILES=%ProgramFiles(x86)%

REM Check if Visual Studio 2017 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2017\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2017"
        echo Using Visual Studio 2017 Community
	goto setup_env
  )
)

REM Check if Visual Studio 2019 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2019\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2019"
        echo Using Visual Studio 2019 Community
	goto setup_env
  )
)


echo No compiler : Microsoft Visual Studio 2017 or 2019 Community is not installed.
goto end

:setup_env

echo "%MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat"
call %MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat x64


mkdir tmp_velo
cd tmp_velo

echo
echo ******                		 ******
echo ****** Compiling Velocypack ******
echo ******                		 ******
echo


echo Get velocypack from git...
git clone https://github.com/arangodb/velocypack.git
cd velocypack

echo "Configuring..."
cmake -G"Visual Studio 16 2019" .. -DCMAKE_BUILD_TYPE=Release  -DBuildVelocyPackExamples=OFF -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -A x64 -S . -B build
echo "Building..."
cmake --build build  --target install

cd ..\..

REM Housekeeping
rd /s /q tmp_velo




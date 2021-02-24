
## Prepare building tools for jsonio/jsonui library on Windows10

* Make sure you have Qt and git installed. 

* Add symlink make to mingw32-make  ( do it as Admin, "Command prompt" right mouse bottom run as admin ) 

```sh

cd  C:/Qt/Tools/mingw730_64/bin
mklink make.exe mingw32-make.exe

```

### Prepare folder structure and download source code ###


* Let us call devJSONUI the directory structure where the jsonUI-test, jsonio and jsonui repositories have to be cloned from their respective git repositories:

```

~/devJSONUI
           /jsonui-test
           /jsonio
           /jsonui
           /build-jsonui-test

```


* In a "Command Prompt", run the following commands to download the jsonio, jsonui libraries:

```sh

cd devJSONUI/jsonio
git clone https://bitbucket.org/gems4/jsonio.git .
cd ../jsonui
git clone https://bitbucket.org/gems4/jsonui.git .
cd ../jsonui-test
git clone https://bitbucket.org/gems4/jsonui-test.git .
cd ..

```


### Install Dependencies ###


* Install cmake, curl, boost. In "Command prompt" execute the following"


```sh

cd jsonui-test\build-win
powershell -ExecutionPolicy ByPass ".\Install-win64-mingw.ps1 -installPrefix "C:\usr"  -mingwPath "C:\Qt\Tools\mingw730_64\bin" "

```

* Install/Update Dependencies

In order to build the jsonio&jsonui library on Windows, execute the following:

```sh

powershell -ExecutionPolicy ByPass ".\Update-win64-mingw.ps1 -installPrefix "C:\usr" -mingwPath "C:\Qt\Tools\mingw730_64\bin" "

```


> Before execute program, you need copy from  "C:\usr" files libcrypto-1_1-x64.dll, libssl-1_1-x64.dll and libcurl-x64.dll to executable ( see openssl-1.1.1d_2-win64-mingw.zip and "C:\usr\local\bin")


* Install/Update Dependencies for ThermoMatch and ThermoFunGui (under construction)

In order to build the ThermoFunGui on Windows, execute the following:

```sh

powershell -ExecutionPolicy ByPass ".\Update-win64-mingw-jsonio.ps1 -installPrefix "C:\usr" -mingwPath "C:\Qt\Tools\mingw730_64\bin" -QtPath "C:\Qt\5.13.0\mingw73_64" "

```

> Error ThermoHubClient 
```

Cloning into 'thermohubclient'...
remote: Counting objects: 276, done.
remote: Compressing objects: 100% (190/190), done.
remote: Total 276 (delta 80), reused 236 (delta 68)
Receiving objects: 100% (276/276), 3.92 MiB | 451.00 KiB/s, done.
Resolving deltas: 100% (80/80), done.
-- The CXX compiler identification is GNU 7.3.0
-- Check for working CXX compiler: C:/Qt5/Tools/mingw730_64/bin/g++.exe
-- Check for working CXX compiler: C:/Qt5/Tools/mingw730_64/bin/g++.exe -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Boost  found.
CMake Error at python/pyThermoHubClient/CMakeLists.txt:9 (pybind11_add_module):
  Unknown CMake command "pybind11_add_module".


-- Configuring incomplete, errors occurred!
See also "C:/Users/svd/code/thermohubclient/build/CMakeFiles/CMakeOutput.log".
```

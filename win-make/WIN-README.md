# jsonio17


## Build and Run jsonio17 on Windows 10

* Make sure you have git installed. If not, install it on Windows: https://git-scm.com/download/win.
* To download jsonio17 source code, using Windows Command Prompt go to C:/gitJsonio and execute

```sh
git clone https://svetadmitrieva@bitbucket.org/gems4/jsonio17.git
```

## Prepare building tools mingw


## Prepare building tools MSVC

* jsonio17 dependencies will be compiled using MSVC 2017 64 bit compiler. For this Visual Studio Community (2017) needs to be installed: 
https://docs.microsoft.com/en-us/visualstudio/install/install-visual-studio?view=vs-2017
At Step 4 - Select workloads, select Desktop development with C++ to be installed. On the individual components page check that also Windows 10 SDK is selected to be installed.

* In addition to MSVC 2017, Qt needs to be installed: https://www.qt.io/download in C:/Qt folder (Qt installation folder is used in further scripts, please use C:/Qt)!
Select with Qt 5.12.0 MSVC 2017 64-bit with Qt Charts, and Qt WebEngine.

### Install Dependencies

* For compiling the libraries that jsonio17 is dependent on, .bat script can be found in /win-make. The process will several minutes. In a windows Command Prompt terminal go to ~jsonarango/win-make and run:

```
jsonio17/win-make>install-dependencies-Win-msvc.bat
```

This script builds curl, velocypack and jsonarango libraries, copies then in the C:\usr\local folder.

### Compiling and the jsonio17 test in Qt Creator

* In Qt Creator open ~\jsonio17\jsonio17_test.pro


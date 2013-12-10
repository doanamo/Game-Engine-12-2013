SET CWD=%CD%

IF EXIST build_gcc_release goto BUILD_GCC_REL
mkdir build_gcc_release
:BUILD_GCC_REL
cd build_gcc_release
call ../configs/configure-gcc-release.bat
mingw32-make package > build_log.txt 2>&1
cd %CWD%

IF EXIST build_gcc_debug goto BUILD_GCC_DBG
mkdir build_gcc_debug
:BUILD_GCC_DBG
cd build_gcc_debug
call ../configs/configure-gcc-debug.bat
mingw32-make package > build_log.txt 2>&1
cd %CWD%

IF EXIST build_vc_release goto BUILD_VC_REL
mkdir build_vc_release
:BUILD_VC_REL
cd build_vc_release
call ../configs/configure-vc-release.bat
nmake package > build_log.txt 2>&1
cd %CWD%

IF EXIST build_vc_debug goto BUILD_VC_DBG
mkdir build_vc_debug
:BUILD_VC_DBG
cd build_vc_debug
call ../configs/configure-vc-debug.bat
nmake package > build_log.txt 2>&1
cd %CWD%

erase glew-1.10.0-src.zip
bzr export glew-1.10.0-src.zip
erase glew-1.10.0-src.tar.bz2
bzr export glew-1.10.0-src.tar.bz2

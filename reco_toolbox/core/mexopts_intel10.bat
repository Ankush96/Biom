@echo off
rem INTEL10OPTS.BAT
rem
rem    Compile and link options used for building MEX-files
rem    using the Intel C++ compiler version 10 
rem
rem    $Revision: 1.11 $  $Date: 2001/11/06 16:53:46 $
rem
rem ********************************************************************
rem General parameters
rem ********************************************************************

rem set MSVCDir="C:\Program Files\Microsoft Visual Studio 8\VC"
rem set INTELDir="C:\Program Files\Intel\Compiler\C++\10.1.013\IA32"
rem set MATLAB=%MATLAB%
rem set PATH=%INTELDir%\BIN;%PATH%
rem set INCLUDE=%INTELDir%\INCLUDE;%MSVCDir%\INCLUDE;%INCLUDE%
rem set LIB=%INTELDir%\LIB;%MSVCDir%\LIB;%MSVCDir%\LIB;%LIB%


set MATLAB=%MATLAB%
set VS80COMNTOOLS=%VS80COMNTOOLS%
set VSINSTALLDIR=C:\Program Files\Microsoft Visual Studio 8
set VCINSTALLDIR=%VSINSTALLDIR%\VC
set PATH=%INTELDir;%VCINSTALLDIR%\BIN\;%VCINSTALLDIR%\PlatformSDK\bin;%VSINSTALLDIR%\Common7\IDE;%VSINSTALLDIR%\SDK\v2.0\bin;%VSINSTALLDIR%\Common7\Tools;%VSINSTALLDIR%\Common7\Tools\bin;%VCINSTALLDIR%\VCPackages;%MATLAB_BIN%;%PATH%
set INCLUDE=%INTELDir%\INCLUDE;%VCINSTALLDIR%\ATLMFC\INCLUDE;%VCINSTALLDIR%\INCLUDE;%VCINSTALLDIR%\PlatformSDK\INCLUDE;%VSINSTALLDIR%\SDK\v2.0\include;%INCLUDE%
set LIB=%INTELDir%\LIB;%VCINSTALLDIR%\ATLMFC\LIB;%VCINSTALLDIR%\LIB;%VCINSTALLDIR%\PlatformSDK\lib;%VSINSTALLDIR%\SDK\v2.0\lib;%MATLAB%\extern\lib\win32;%LIB%
set MW_TARGET_ARCH=win32


rem ********************************************************************
rem Compiler parameters
rem ********************************************************************
set COMPILER=icl
set COMPFLAGS= -c /Qopenmp -W3  -DMATLAB_MEX_FILE -nologo
set OPTIMFLAGS=/LD /fast -DNDEBUG

rem OPTIMFLAGS=/MD /QxT /O3 /Qipo /Qprec-div- /fp:fast -DNDEBUG

rem -G6 -Qip -QxK -O3
set DEBUGFLAGS=/LDd -Zi -Fd"%OUTDIR%%MEX_NAME%.pdb"
set NAME_OBJECT=/Fo

rem ********************************************************************
rem Linker parameters
rem ********************************************************************
rem set LIBLOC=%MATLAB%\extern\lib\win32\microsoft
set LIBLOC=C:\Program Files\Intel\Compiler\C++\10.1.013\IA32\lib
set LINKER=xilink
set LINKFLAGS=/dll /export:%ENTRYPOINT% /MAP /LIBPATH:"%LIBLOC%" libmx.lib libmex.lib libmat.lib /implib:%LIB_NAME%.x  /NOLOGO
set LINKOPTIMFLAGS= 
set LINKDEBUGFLAGS=/debug
set LINK_FILE=
set LINK_LIB=
set NAME_OUTPUT=/out:"%OUTDIR%%MEX_NAME%.mexw32"
set RSP_FILE_INDICATOR=@

rem ********************************************************************
rem Resource compiler parameters
rem ********************************************************************
set RC_COMPILER=rc /fo "%OUTDIR%mexversion.res"
set RC_LINKER=

set POSTLINK_CMDS=del "%OUTDIR%%MEX_NAME%.map"
set POSTLINK_CMDS1=del %LIB_NAME%.x

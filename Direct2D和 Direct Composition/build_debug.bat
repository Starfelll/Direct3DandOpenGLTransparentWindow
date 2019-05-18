
@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

set compilerflags=/Od /Zi /EHsc

set linkerflags=/OUT:main.exe
set libs=/DYNAMICBASE "d2d1.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"

cl.exe main.cpp GameTimer.cpp /link /OUT:"Debug\Direct2D_Test.exe" /MANIFEST /NXCOMPAT /PDB:"Debug\Direct2D_Test.pdb" %libs% /DEBUG /MACHINE:X86 /INCREMENTAL /SUBSYSTEM:WINDOWS /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:"Debug\Direct2D_Test.exe.intermediate.manifest" /ERRORREPORT:PROMPT /NOLOGO /TLBID:1 

::call .\Debug\Direct2D_Test.exe
echo ---------------build-end-----------------------------------------------

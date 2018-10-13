@REM Requires the DirectX SDK bin directory to be in your path!

del test_fxc.txt
del test_fxdis.txt
del test.bin
fxc /Od /T ps_4_0 test.hlsl /Fo test.bin /Fc test_fxc.txt 
@if ERRORLEVEL 1 goto errorexit
type test.txt
debug\fxdis.exe test.bin > test_fxdis.txt
goto done
:errorexit
@ECHO Compile error!
:done

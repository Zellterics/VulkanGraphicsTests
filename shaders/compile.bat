@echo off
setlocal

set GLSLC=glslc
set SHADERS_DIR=.
set VERT=%SHADERS_DIR%\shader.vert
set FRAG=%SHADERS_DIR%\shader.frag
set VERT_OUT=%SHADERS_DIR%\vert.spv
set FRAG_OUT=%SHADERS_DIR%\frag.spv

echo Compilando vertex shader...
%GLSLC% "%VERT%" -o "%VERT_OUT%"
if errorlevel 1 goto :error

echo Compilando fragment shader...
%GLSLC% "%FRAG%" -o "%FRAG_OUT%"
if errorlevel 1 goto :error

echo.
echo ✅ Compilación exitosa.
goto :end

:error
echo.
echo ❌ Error durante la compilación.

:end
pause
endlocal

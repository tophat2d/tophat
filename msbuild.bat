@echo off
rem Detect if VC devenv is activated by checking if cl is available
where cl
if %ERRORLEVEL% NEQ 0 (
    echo Error: You need to enable VC developer console. Press the windows key and type 'Developer Command Prompt for VS'
    echo        and run the command again.
    exit /b 1
)

where cmake
if %ERRORLEVEL% NEQ 0 (
    echo Error: You need cmake.    
    exit /b
)

md bin
cd bin
cmake -DCMAKE_BUILD_TYPE=Release .. && msbuild tophat.sln /property:Configuration=Release
cd ..
copy bin\Release\tophat.exe tophat.exe
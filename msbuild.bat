@echo off
rem Detect if VC devenv is activated by checking if cl is available
where cl
if %ERRORLEVEL% NEQ 0 (
  echo ERROR: You need to enable VC developer console. Press the windows key and type 'x64 Native Tools Command Prompt for VS'
  echo        and run the command again.
  exit /b 1
)

where cmake
if %ERRORLEVEL% NEQ 0 (
  echo ERROR: You need cmake.    
  exit /b
)

if not exist lib\umka\umka_windows_msvc\umka.exe (
  echo BUILD: Umka
  cd lib\umka\
  call build_windows_msvc
  cd ..\..\
)

echo BUILD: Tophat
set target=Release

md bin
cd bin

cmake -DCMAKE_BUILD_TYPE=%target% .. && msbuild tophat.sln /property:Configuration=%target%
cd ..
copy bin\%target%\tophat.exe tophat.exe
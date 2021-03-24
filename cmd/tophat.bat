@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

set Path=%appdata%\tophat

goto :start
:get
echo downloading

C:\WINDOWS\System32\curl.exe "https://github.com/marekmaskarinec/tophat/raw/main/bin/tophat.zip" -o tophat.zip
if exist tophat.zip (
  echo download completed
) else (
  echo download failed
  goto :EOF
)

goto :EOF
:clean
if exist !Path! (
  echo removing tophat
)

del %Path%
if exist tophat.zip (
  del tophat.zip
)

goto :EOF
:setup
del %Path%
mkdir %Path%


echo extracting
C:\WINDOWS\System32\WindowsPowerShell\v1.0\powershell.exe -NoP -NonI -Command "Expand-Archive" 'tophat.zip' '%Path%'

echo copying
MOVE %Path%/tophat-release/* %Path%
del %Path%/tophat-release


echo done

goto :EOF
:update
echo updating not yet available on windows
EXIT /B 1

set /a _1_%~2=^(1 + %~2^)
call :get _2_%~2 !_1_%~2!
echo | set /p ^=!_2_%~2!
set _0_%~2=tophat.zip
set /a _3_%~2=^(1 + %~2^)
call :setup _4_%~2 !_3_%~2! _0_%~2
echo | set /p ^=!_4_%~2!

goto :EOF
:package
set os_%~2=!%~3!
set name_%~2=!%~4!
if !os_%~2! EQU  (
  echo specify os
  goto :EOF
)
if !name_%~2! EQU  (
  set Name=game
) else (
  set Name=!name_%~2!
)
if !os_%~2! EQU linux (
  set Bin=tophat-linux
) else (
  if !os_%~2! EQU windows (
    set Bin=tophat-win
  ) else (
    if !os_%~2! EQU all (
      echo packaging for linux
      set _1_%~2=linux
      set _0_%~2=!Name!
      set /a _4_%~2=^(1 + %~2^)
      call :package _5_%~2 !_4_%~2! _0_%~2 _1_%~2
      echo | set /p ^=!_5_%~2!
      echo packaging for windows
      set _3_%~2=windows
      set _2_%~2=!Name!
      set /a _6_%~2=^(1 + %~2^)
      call :package _7_%~2 !_6_%~2! _2_%~2 _3_%~2
      echo | set /p ^=!_7_%~2!
      goto :EOF
    )
  )
)
if !Bin! EQU  (
  echo invalid os specified. only windows and linux currently supported
  goto :EOF
)
mkdir ..\%Name%-%1
mkdir ..\%Name%-%1\%Name%.dat
COPY .\* ..\%Name%-%1\%Name%.dat
COPY %Path%\bin\%Bin% ..\%Name%-%1\%Name%




if !os_%~2! EQU windows (
  
)




echo done
goto :EOF

:start

if "%1" == "get" (
	goto :get
)

if "%1" == "install" (
	goto :setup
)

if "%1" == "update" (
	goto :update
	goto :EOF
)

if "%1" == "package" (
	set !%~3! = %2
	set !%~4! = %3
	goto :package
)

echo invalid usage
echo options:
echo get: downloads archive with tophat
echo install: installs tophat
echo update: updates tophat
echo package [ platform windows/linux/all ] [ game name ]: packages game
echo more info on github.com/marekmaskarinec/tophat

echo test

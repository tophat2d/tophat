set /p Name=Target: 

mkdir %Name%-windows
mkdir %Name%-windows\tophat.dat
copy /Y %Name%\*.* %Name%-windows\tophat.dat
copy /Y bin\tophat-win.exe %Name%-windows
rename %Name%-windows\tophat-win.exe %Name%.exe

mkdir %Name%-linux
mkdir %Name%-linux\tophat.dat
copy /Y %Name%\*.* %Name%-linux\tophat.dat
copy /Y bin\tophat-linux %Name%-linux
rename %Name%-linux\tophat-linux %Name%


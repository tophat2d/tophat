set /p Name=Target: 
mkdir %Target%-windows
COPY %Target% %Target%-windows\tophat.dat
COPY bin\tophat-win.exe %Target%-windows
mkdir %Target%-linux
COPY %Target% %Target%-linux\tophat.dat
COPY bin\tophat-linux %Target%-linux

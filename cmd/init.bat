set /p Name=Enter name of the game: 
mkdir %Name%
copy /Y preset\*.* %Name%

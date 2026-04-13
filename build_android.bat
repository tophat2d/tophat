set ASSETS=android\app\src\main\assets\tophat
mkdir %ASSETS%
copy /y demos\flappy\*.um %ASSETS%
copy /y demos\flappy\*.ttf %ASSETS%
copy /y demos\flappy\gfx\*.* %ASSETS%
copy /y demos\flappy\sfx\*.* %ASSETS%

set ANDROID_HOME=%LOCALAPPDATA%\Android\Sdk
cd android
call gradlew assembleDebug
cd ..

copy /y android\app\build\outputs\apk\debug\app-debug.apk tophat-debug.apk

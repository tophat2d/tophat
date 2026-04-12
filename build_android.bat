copy demos\flappy\*.ttf android\app\src\main\assets\tophat
copy demos\flappy\gfx\*.* android\app\src\main\assets\tophat
copy demos\flappy\sfx\*.* android\app\src\main\assets\tophat

set ANDROID_HOME=%LOCALAPPDATA%\Android\Sdk
cd android
call gradlew assembleDebug
cd ..

copy android\app\build\outputs\apk\debug\app-debug.apk tophat-debug.apk

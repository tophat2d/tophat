copy demos\flappy\*.ttf gradle\app\src\main\assets\tophat
copy demos\flappy\gfx\*.* gradle\app\src\main\assets\tophat
copy demos\flappy\sfx\*.* gradle\app\src\main\assets\tophat

set ANDROID_HOME=%LOCALAPPDATA%\Android\Sdk
cd gradle
gradlew assembleDebug

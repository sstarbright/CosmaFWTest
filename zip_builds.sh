cd build/linux
zip terracon_linux.zip terracon
zip -r terracon_linux.zip  assets
cd ../windows
zip terracon_windows.zip terracon.exe SDL2*.dll
zip -r terracon_windows.zip  assets

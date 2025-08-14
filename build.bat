@echo off
cls
echo Compiling SDL project...

g++ main.cpp editor_core.cpp -o main.exe -mconsole -IC:\msys64\mingw64\include\SDL2 -LC:\msys64\mingw64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_TTF -lcomdlg32
main.exe
pause


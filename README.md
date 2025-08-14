# textEditor

A lightweight text editor written in C++ using SDL2 and SDL_ttf. Features include:

 - Line numbers with a flexible gutter

 - Scrollable text and per-line textures for fast rendering

 - Customizable fonts and colors and fontsize


# Build & Run

Make sure SDL2 and SDL_ttf are installed. Then compile with your C++ compiler:

*On first launch, the text and line numbers may not display correctly. Move the font size slider once to refresh all textures.*
```
g++ main.cpp editor_core.cpp -o editor.exe -mconsole \
-IC:\msys64\mingw64\include\SDL2 \
-LC:\msys64\mingw64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_TTF -lcomdlg32
./editor
```
# Usage

 - Edit text directly in the window

 - Line numbers automatically update with scrolling

 - Change fonts and colors via the configuration
 -  Scroll & change font-size using scrollbars
    

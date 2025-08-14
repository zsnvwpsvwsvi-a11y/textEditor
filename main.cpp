#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include "editor_core.hpp"
#include <utility> 
#include <algorithm>
#include <cmath>

SDL_Color getColor(const std::string &line);

int main(int argc, char** argv) {
    // Initialize SDL and TTF
    
    lineObj emptyline;
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    // Initialize window and renderer
    SDL_Window* win = SDL_CreateWindow("Notepad", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 1000, 600,SDL_WINDOW_RESIZABLE);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int winH, winW;
    SDL_GetWindowSize(win, &winW, &winH);
    std::pair<int, int> StartSelection,EndSelection;


    std::ifstream inputFile("colors.txt"); // Assuming "example.txt" exists in the same directory
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return 1;
    }
    int lineCount = 0;
    std::vector<std::string> fontsNcolors;
    std::string firstLine;
    while (lineCount< 6 && std::getline(inputFile, firstLine)) {
        std::cout << "First line: " << firstLine << std::endl;
        lineCount++;
         fontsNcolors.push_back(firstLine);
    }
    inputFile.close();

    SDL_Color background = getColor( fontsNcolors[1]);
    SDL_Color foreground = getColor( fontsNcolors[2]);
    SDL_Color textColor = getColor( fontsNcolors[3]);
    SDL_Color lineColor = getColor( fontsNcolors[4]);
    SDL_Color lineNumColor = getColor( fontsNcolors[5]);

    // Initialize sliders
    HorizontalSlider marginX(0, 0, 150, 40, 0, 200, 40);
    HorizontalSlider marginY(200, 0, 150, 40, 0, 200, 70);
    HorizontalSlider fontsize(300, 0, 150, 40, 14, 72, 18);
    HorizontalSlider linespacing(600, 0, 150, 40, 100, 200, 120);
    VerticalSlider scroll(winW - 40, 0, 40, winH, 0, 10000, 0);
    SDL_Rect marginbox;


    // This is were almost all the "global" variables are 
    TextLayoutContext tx;
    //Set up a tx.font cache to lower
    std::string fontpath = fontsNcolors[0]; 
    for (size_t i = fontsize.minValue; i < fontsize.maxValue+1; i++) {
        TTF_Font* font = TTF_OpenFont(fontpath.c_str(), i);
        if (!font) {
            std::cout << "Failed to open font: " << TTF_GetError() << "\n";
            return 1;
        }
        tx.fontCache.push_back(font);
    }

    tx.font = tx.fontCache[14];

    TTF_Font* settingsfont = TTF_OpenFont("c:/windows/fonts/arial.TTf", 12);
    if (!settingsfont) {
        std::cout << "Failed to open settingsfont: " << TTF_GetError() << "\n";
        return 1;
    }


    // Initialize buttons
    Button newbutton(ren, settingsfont, {0,0,60,20}, "New");
    Button openbutton(ren, settingsfont, {65,0,60,20}, "Open");
    Button savebutton(ren, settingsfont, {130,0,60,20}, "Save");
    Button saveasbutton(ren, settingsfont, {195,0,60,20}, "Save as");


    // Load a file --------------------------------------------
    std::wstring filePath = L"";
    scroll.Value = scroll.minValue;
    tx.lines = {emptyline};
    //tx.lines.push_back(emptyline);
    //filePath = OpenFile(tx.lines,tx.font,ren);
    // Initialize cursor logic
    Uint32 lastToggle = SDL_GetTicks();
    const Uint32 blinkInterval = 500;  // milliseconds
    std::string currenttext;
    SDL_StartTextInput();
    fillNumbersTexture(ren,tx,lineNumColor);
    SDL_Rect typingArea = {marginX.Value, marginY.Value, winW, winH};
    // Main loop
    tx.font = tx.fontCache[fontsize.Value - fontsize.minValue];
    ClearLines(tx.lines);
    bool typing = true;
    bool quit = false;
    SDL_Event e;
    while (!quit) {
       marginbox = {0,0,winW,marginY.Value};
        Uint32 frameStart = SDL_GetTicks();     
        SDL_GetWindowSize(win, &winW, &winH);
        Uint32 mouseState =  SDL_GetMouseState(&tx.mouseX, &tx.mouseY);
        tx.Mouse = {tx.mouseX, tx.mouseY};
        scroll.x = winW - 40; // scroll slider x position
        scroll.h = winH;
        typingArea = {marginX.Value, marginY.Value, winW-40-marginX.Value, winH};
        while (SDL_PollEvent(&e)) {
 
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    ArrowKeysEvent(e, tx, marginX.Value, marginY.Value);
                    BackSpaceKeyEvent(e, tx, marginX.Value, marginY.Value);
                    EnterKeyEvent(e, tx, marginX.Value, marginY.Value);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (SDL_PointInRect(&tx.Mouse, &typingArea)) {
                        tx.getcurrenttext = true;
                        
                        tx.selection = false;
                        StartSelection = getLineAndColumn(tx,marginY.Value,marginX.Value);
                        if (StartSelection.first > tx.lines.size()-1) StartSelection.first = tx.lines.size();
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (SDL_PointInRect(&tx.Mouse, &typingArea)) {
                        tx.getcurrenttext = true;
                        tx.selection = false;
                        //StartSelection = NULL;

                    }
                    break;
            }
            TextInputEvent(e, tx, typing, marginX.Value, marginY.Value);
            newbutton.click_event(e);
            openbutton.click_event(e);
            savebutton.click_event(e);
            saveasbutton.click_event(e);
            //marginX.handleEvent(e);
            //marginY.handleEvent(e);
            fontsize.handleEvent(e);
            scroll.handleEvent(e);
            //linespacing.handleEvent(e);
        }


        if (mouseState & SDL_BUTTON_LMASK) {

            if (SDL_PointInRect(&tx.Mouse, &typingArea) && !scroll.dragging) {
                tx.getcurrenttext = true;
                typing = true;
                tx.selection = true;
                EndSelection = getLineAndColumn(tx,marginY.Value,marginX.Value) ;
                if (EndSelection.first > tx.lines.size()-1) EndSelection.first = tx.lines.size();
                
            } else {
                typing = false;
            }
        }
            

        // Update slider values based on dragging
        //marginX.update();
        //marginY.update();
        fontsize.update();
        scroll.update();
        //linespacing.update();

        if (openbutton.isClicked) {

            scroll.Value = scroll.minValue;
            openbutton.draw();
            filePath = OpenFile(tx.lines,tx.font,ren,textColor);
            openbutton.isClicked = false;
            if (filePath == L"") {

                newbutton.isClicked = true;
            }
        }


        if (savebutton.isClicked) { 

            if (filePath == L"") {

                SaveAsFile();
            } else {
            
                std::filesystem::path path(filePath);
                std::ofstream out(path);     
                if (!out.is_open()) {
                    std::wcout << L"Failed to open file: " << filePath << std::endl;
                    return 1;
                }
                for (const auto& ch : tx.lines) {
                    out << ch.text << "\n"; 
                }

                out.close();

                savebutton.isClicked = false;
            }
        }


        if (saveasbutton.isClicked) {  

            SaveAsFile();
            saveasbutton.isClicked = false;
        }


        if (newbutton.isClicked) { 

            scroll.Value = scroll.minValue;
            filePath = L"";
            newbutton.isClicked = false;
            for (size_t i = 0; i < tx.lines.size(); i++) {
                SDL_DestroyTexture(tx.lines[i].texture);
            }
            tx.lines.clear();
            tx.lines = {emptyline};
        }








      // RENDERING PART ***************************************************************************************************************************







        SDL_SetRenderDrawColor(ren, background.r, background.g, background.b, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, foreground.r, foreground.g, foreground.b, 255);
        SDL_RenderFillRect(ren, &typingArea);

        if (typing)
        {
            SDL_SetRenderDrawColor(ren, lineColor.r, lineColor.g, lineColor.b, 255);
            tx.linedst.y = tx.cursordst.y;
            tx.linedst.h = tx.cursordst.h;
            tx.linedst.x = marginX.Value;
            tx.linedst.w = winW ;

            SDL_RenderFillRect(ren, &tx.linedst);
        }
        

        tx.scrollY = (scroll.Value/10000.0)*(tx.lineHeight*tx.lines.size()+marginY.Value-winH);
        if (tx.scrollY <0) tx.scrollY = 0;
        //loop over all tx.lines and render them i represent's the current line in the loop
        for (size_t i = 0; i < tx.lines.size(); i++) {

            int line_y = (i + 1) * tx.lineHeight + marginY.Value - tx.scrollY;


            

            if (marginY.Value-40 <= line_y - tx.lines[i].height && line_y <= winH) {

                if (tx.lines[i].needsUpdate)
                {
                    SetLineTexture(tx.font, ren, tx.lines, i,textColor);
                    tx.lines[i].needsUpdate = false;
                }

                SDL_Rect dstLine = {marginX.Value, line_y - tx.lines[i].height, tx.lines[i].width, tx.lines[i].height};
                SDL_RenderCopy(ren, tx.lines[i].texture, NULL, &dstLine);


                std::string numStr = std::to_string(i + 1);
                int numX = marginX.Value - 5;

                for (int d = (int)numStr.size() - 1; d >= 0; --d)
                {
                    int digit = numStr[d] - '0';
                    int w = tx.numbersW[digit];
                    int h = tx.numbersH[digit];
                    numX -= w;
                    
                    SDL_Rect numDst = { numX, line_y - h, w, h };
                    SDL_RenderCopy(ren, tx.numbers[digit], NULL, &numDst);
                }


            /*if (tx.selection) {
                    int startLine, endLine, startCol, endCol;
                    if (StartSelection.first < EndSelection.first ) {
                        startLine = StartSelection.first;
                        startCol = StartSelection.second;
                        endLine = EndSelection.first;
                        endCol = EndSelection.second;

                    } else if (StartSelection.first > EndSelection.first ) {
                        startLine = EndSelection.first;
                        startCol = EndSelection.second;
                        endLine = StartSelection.first;
                        endCol = StartSelection.second;

                    } else if (StartSelection.first == EndSelection.first ) {
                    if (StartSelection.second<   EndSelection.second){
                        startCol = StartSelection.second; endCol = EndSelection.second;
                    }else {endCol = StartSelection.second; startCol = EndSelection.second;}
                    }
                    bool shouldDraw = false;
                    SDL_Rect highlightBox;
                    highlightBox.y = line_y - tx.lineHeight;
                    highlightBox.h = tx.lineHeight;


                    if (i == StartSelection.first && StartSelection.first == EndSelection.first) {
                        TTF_SizeUTF8(tx.font,tx.lines[i].text.substr(0,startCol).c_str(),&highlightBox.x,NULL);
                        TTF_SizeUTF8(tx.font,tx.lines[i].text.substr(0,endCol).c_str(),&highlightBox.w,NULL);
                        highlightBox.w -= highlightBox.x;
                        highlightBox.x += marginX.Value;
                            shouldDraw = true;
                    } else if (i == startLine) {
                        TTF_SizeUTF8(tx.font,tx.lines[i].text.substr(0,startCol).c_str(),&highlightBox.x,NULL);
                        highlightBox.w = tx.lines[i].width - highlightBox.x;
                        highlightBox.x += marginX.Value;
                            shouldDraw = true;
                    } else if (i > startLine && i < endLine) {
                        highlightBox.x = marginX.Value;
                        highlightBox.w = tx.lines[i].width;
                            shouldDraw = true;
                    } else if (i == endLine) {
                        highlightBox.x = marginX.Value;
                        TTF_SizeUTF8(tx.font,tx.lines[i].text.substr(0,endCol).c_str(),&highlightBox.w,NULL);
                            shouldDraw = true;
                    }

                    if ( shouldDraw) {
                    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(ren, 100, 100, 255, 100); // RGBA
                    SDL_RenderFillRect(ren, &highlightBox);
                    }  
                }
*/
            }
            if (line_y > winH) break;
            

        }

        if(tx.getcurrenttext) {

            std::pair<int, int> cursorPos = getLineAndColumn(tx,marginY.Value,marginX.Value);
            int cursorX;
            TTF_SizeUTF8(tx.font,tx.lines[cursorPos.first].text.substr(0,cursorPos.second).c_str(), &cursorX,NULL);
            cursorX+= marginX.Value;
            int CursorY = (cursorPos.first+1) * tx.lineHeight + marginY.Value - tx.scrollY;
            int CursorH = fontsize.Value * 12 / 10;
            tx.cursordst = {cursorX, CursorY - CursorH, 1, CursorH};
            tx.getcurrenttext = false;

        }


        SDL_SetRenderDrawColor(ren, background.r, background.g, background.b, 255);
        SDL_RenderFillRect(ren,&marginbox);

        // render the sliders SLIDER PART***************************************************************************
        //marginX.draw(ren, settingsfont);
        //marginY.draw(ren, settingsfont);
        fontsize.draw(ren, settingsfont);
        scroll.draw(ren, settingsfont);
        //linespacing.draw(ren, settingsfont);
        newbutton.draw();
        openbutton.draw();
        savebutton.draw();
        saveasbutton.draw();

        // check if the font size changed
        if (fontsize.dragging) {
            tx.font = tx.fontCache[fontsize.Value - fontsize.minValue];
            fillNumbersTexture(ren,tx,lineNumColor);
            ClearLines(tx.lines);
        }

        if (linespacing.dragging || fontsize.dragging) { tx.lineHeight  = fontsize.Value * linespacing.Value / 100.0; } // check if the tx.linespacing changed
        if (scroll.dragging) tx.showCursor = false;  // check if the SCROLLING changed


        // calculate the time of execution of each frame
        Uint32 now = SDL_GetTicks();
        if (now - lastToggle >= blinkInterval) {
            tx.showCursor = !tx.showCursor;
            lastToggle = now;
        }

        // Render cursor flicker
        if (tx.showCursor && typing) {
            //SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderFillRect(ren, &tx.cursordst);
        }

        SDL_RenderPresent(ren);   
    }
    


    // Clean everything
    SDL_StopTextInput();


    for (size_t i = 0; i < tx.fontCache.size(); i++) {
        TTF_CloseFont(tx.fontCache[i]);
    }
    tx.fontCache.clear();
    // save up closing
    std::filesystem::path path(filePath);
    std::ofstream out(path);


    if (!out.is_open()) {
        std::wcout << L"Failed to open file: " << filePath << std::endl;
        return 1;
    }


    for (const auto& ch : tx.lines) {
        out << ch.text << "\n";
        SDL_DestroyTexture(ch.texture);
    }


    tx.lines.clear();
    out.close();
    TTF_CloseFont(settingsfont);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}


SDL_Color getColor(const std::string &line) {
    SDL_Color background;
    const std::string nums = "0123456789";
    std::string res;
    int color;
    for(char ch : line) {
        if ( nums.find(ch) != std::string::npos) {
            res += ch;
        }
    }
    std::cout <<res;
    color =  std::stoi(res);
    background.r = color / 1000000;
    background.g = color/1000 - background.r*1000;
    background.b = color % 1000;
    background.a = 255;

    return background;
}

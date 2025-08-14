#include <windows.h>
#include <commdlg.h>
#include <string>
#include <iostream>

struct FileDialogResult {
    bool success;
    std::wstring filePath;
};

FileDialogResult OpenFileDialog(HWND owner = nullptr) {
    wchar_t fileName[MAX_PATH] = L"";

    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    bool result = GetOpenFileNameW(&ofn) == TRUE;//GetSaveFileNameW
    return { result, result ? std::wstring(fileName) : L"nopath" };
}


FileDialogResult SaveASFileDialog(HWND owner = nullptr) {
    wchar_t fileName[MAX_PATH] = L"";

    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    bool result = GetSaveFileNameW(&ofn) == TRUE;//GetSaveFileNameW
    return { result, result ? std::wstring(fileName) : L"nopath" };
}
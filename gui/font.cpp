#include "share.h"

BOOL CALLBACK setFontCallback(HWND hwndChild, LPARAM lParam)
{
    SendMessageW(hwndChild, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
    return TRUE;
}

void setGlobalFont() {
    const int dpi = static_cast<int>(GetDpiForWindow(mainHwnd));
    constexpr int fontSize = 12;
    const int fontHeight = -MulDiv(fontSize, dpi, 72);

    HFONT hFont = CreateFontW(
        fontHeight, 0,
        0,0,  FW_NORMAL,
        FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Microsoft YaHei"
    );

    SendMessageW(mainHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    EnumChildWindows(mainHwnd, setFontCallback, reinterpret_cast<LPARAM>(hFont));
}
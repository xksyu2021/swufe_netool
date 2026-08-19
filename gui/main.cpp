#include "share.h"

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    constexpr auto CLASS_NAME = L"netool_gui";

    WNDCLASSEXW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.cbSize = sizeof(wc);

    RegisterClassExW(&wc);

    mainHwnd = CreateWindowExW(
        0,
        CLASS_NAME, TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        nullptr, nullptr, hInstance, nullptr
    );

    MainLayout::init();
    setGlobalFont();

    ShowWindow(mainHwnd, nCmdShow);
    UpdateWindow(mainHwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_COMMAND: {
        const WORD wmId = LOWORD(wParam);
        MainLayout::callback(wmId);
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    default:
        break;

    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
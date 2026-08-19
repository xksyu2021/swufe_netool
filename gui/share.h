#pragma once
#include <string>
#include <windows.h>

// simplize string types.
// C: no edit    W: wide    P: pointer for Win32
typedef std::wstring         WSTR;
typedef std::wstring_view   CWSTR;
typedef LPCWSTR             CWSTRP;
typedef LPWSTR               WSTRP;
typedef std::string          STR;
typedef std::string_view    CSTR;

// shared vars
inline HWND mainHwnd = nullptr;
inline auto TITLE = L"Swufe Netool";

// component creater with grid layout
class Creater {
    int col, row, gap, cellW = 0, cellH = 0;
    HWND &head;

public:
    Creater(HWND &head, const int row, const int col, const int gap)
        :head(head),row(row),col(col),gap(gap) {}

    void reflesh() {
        // compute cell size
        RECT rc;
        GetClientRect(head, &rc);
        const int clientW = rc.right - rc.left;
        const int clientH = rc.bottom - rc.top;
        cellW = (clientW - gap * (col - 1)) / col;
        cellH = (clientH - gap * (row - 1)) / row;
    }

    // spwan window
    void spwan(
        LPCWSTR lpClassName,
        CWSTRP lpWindowName,
        int id,
        DWORD dwStyle,
        int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
        int margin
    ) {
        // compute position
        int x = (cpt_col-1) * cellW + cpt_col * gap + margin;
        int y = (cpt_row-1) * cellH + cpt_row * gap + margin;
        int w = cpt_col_ride * cellW + (cpt_col_ride - 1) * gap - margin * 2;
        int h = cpt_row_ride * cellH + (cpt_row_ride - 1) * gap - margin * 2;

        // create window with position, cpt, and args
        CreateWindowW(
            lpClassName,
            lpWindowName,
            dwStyle,
            x, y, w, h,
            head,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            reinterpret_cast<HINSTANCE>(GetWindowLongPtr(head, GWLP_HINSTANCE))
            ,nullptr
        );
    }

    // tools
    HWND getHwnd(const int id) {
        return GetDlgItem(head, id);
    }

    void setPos(
        int id,
        int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
        int margin
    ) {
        int x = (cpt_col-1) * cellW + cpt_col * gap + margin;
        int y = (cpt_row-1) * cellH + cpt_row * gap + margin;
        int w = cpt_col_ride * cellW + (cpt_col_ride - 1) * gap - margin * 2;
        int h = cpt_row_ride * cellH + (cpt_row_ride - 1) * gap - margin * 2;

        SetWindowPos(
            getHwnd(id),
            nullptr,
            x, y,w,h,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }

    void setVisible(int id, bool bl) {
        HWND hwnd = getHwnd(id);
        if (bl) {
            ShowWindow(hwnd, SW_SHOW);
        } else {
            ShowWindow(hwnd, SW_HIDE);
        }
    }

    bool isChecked(int id) {
        return SendMessage(
            getHwnd(id),BM_GETCHECK, 0, 0
            ) == BST_CHECKED;
    }
};

// font
void setGlobalFont();

// main layout
namespace MainLayout {
    void init();
    void callback(WORD wmId);
    void del();
};
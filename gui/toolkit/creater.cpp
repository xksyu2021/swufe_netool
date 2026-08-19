#include "../share.h"

void Creator::reflesh() {
    // compute cell size
    RECT rc;
    GetClientRect(head, &rc);
    const int clientW = rc.right - rc.left;
    const int clientH = rc.bottom - rc.top;
    cellW = (clientW - gap * (col - 1)) / col;
    cellH = (clientH - gap * (row - 1)) / row;
}

void Creator::spwan(
    LPCWSTR lpClassName,
    CWSTRP lpWindowName,
    int id,
    DWORD dwStyle,
    int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
    int margin
) const {
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

//tools
HWND Creator::getHwnd(const int id) const {
    return GetDlgItem(head, id);
}

void Creator::setPos(
    int id,
    int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
    int margin
) const {
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

void Creator::setVisible(int id, bool bl) const {
    HWND hwnd = getHwnd(id);
    if (bl) {
        ShowWindow(hwnd, SW_SHOW);
    } else {
        ShowWindow(hwnd, SW_HIDE);
    }
}

bool Creator::isChecked(int id) const {
    return SendMessage(
        getHwnd(id),BM_GETCHECK, 0, 0
        ) == BST_CHECKED;
}
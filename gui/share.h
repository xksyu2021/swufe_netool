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
class Creator {
    HWND &head;
    int row, col, gap, cellW = 0, cellH = 0;

public:
    Creator(HWND &head, const int row, const int col, const int gap)
        :head(head),row(row),col(col),gap(gap) {}

    void reflesh();
    void spwan(
        LPCWSTR lpClassName,
        CWSTRP lpWindowName,
        int id,
        DWORD dwStyle,
        int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
        int margin
    );
    // tools
    [[nodiscard]] HWND getHwnd(int id) const;
    void setPos(
        int id,
        int cpt_row, int cpt_col, int cpt_row_ride, int cpt_col_ride,
        int margin
    ) const;
    void setVisible(int id, bool bl) const;
    [[nodiscard]] bool isChecked(int id) const;
};

// font
void setGlobalFont();

// main layout
namespace MainLayout {
    void init();
    void callback(WORD wmId);
    void del();
};
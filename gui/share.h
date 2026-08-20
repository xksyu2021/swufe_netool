#pragma once
#include <string>
#include <windows.h>
#include <mutex>

// simplize string types.
// C: no edit    W: wide    P: pointer for Win32
typedef std::wstring         WSTR;
typedef std::wstring_view   CWSTR;
typedef LPCWSTR             CWSTRP;
typedef LPWSTR               WSTRP;
typedef std::string           STR;
typedef std::string_view     CSTR;

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
    ) const;

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
    void callback(WORD);
    void del();
};

// ipc toolkit
class Ipc {
    inline static HANDLE job = nullptr;

    HANDLE stdin_read = nullptr;
    HANDLE stdout_write = nullptr;
    HANDLE stderr_write = nullptr;
    HANDLE stdin_write = nullptr;
    HANDLE stdout_read = nullptr;
    HANDLE stderr_read = nullptr;
    HANDLE process = nullptr;

    static void createJob();

    void close();

public:
    Ipc(CWSTRP);
    ~Ipc();
    void send(CWSTRP) const;
    void sendLn() const;
    void readOut(WSTR&) const;
    void readErr(WSTR&) const;
    [[nodiscard]] bool active() const;
};

// info
inline struct Info {
    WSTR isp, acc, pwd;
} info;

// op function
void submit();
void analyze();
inline bool submit_lock = false;
inline bool analyze_lock = false;
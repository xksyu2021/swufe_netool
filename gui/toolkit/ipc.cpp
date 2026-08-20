#include "../share.h"

static void checkAndClose(HANDLE &hd) {
    if (hd) {
        CloseHandle(hd);
        hd = nullptr;
    }
}

void Ipc::createJob() {
    job = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
    info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(
        job,
        JobObjectExtendedLimitInformation,
        &info,
        sizeof(info)
    );
}

void Ipc::close() {
    checkAndClose(stdin_read);
    checkAndClose(stdin_write);
    checkAndClose(stdout_read);
    checkAndClose(stdout_write);
    checkAndClose(stderr_read);
    checkAndClose(stderr_write);
    checkAndClose(process);
}

Ipc::Ipc(const CWSTRP program){
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    CreatePipe(&stdin_read,  &stdin_write,  &sa, 0);
    CreatePipe(&stdout_read, &stdout_write, &sa, 0);
    CreatePipe(&stderr_read, &stderr_write, &sa, 0);

    SetHandleInformation(stdin_write,  HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stdout_read,  HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_read,  HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput  = stdin_read;
    si.hStdOutput = stdout_write;
    si.hStdError  = stderr_write;
    PROCESS_INFORMATION pi{};

    WSTR cmd = program;
    CreateProcessW(
        nullptr,
        cmd.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );
    process = pi.hProcess;

    checkAndClose(stdin_read);
    checkAndClose(stdout_write);
    checkAndClose(stderr_write);

    if (!job) {
        createJob();
    }
    AssignProcessToJobObject(job, pi.hProcess);
}

Ipc::~Ipc() {
    close();
}

void Ipc::send(const CWSTRP in) const {
    const int wide_len = static_cast<int>(wcslen(in));
    const int byte_len = WideCharToMultiByte(
        CP_UTF8, 0, in, wide_len,
        nullptr, 0, nullptr, nullptr
    );
    if (byte_len <= 0) return;
    STR bytes(byte_len, '\0');
    WideCharToMultiByte(
        CP_UTF8, 0, in, wide_len,
        bytes.data(), byte_len, nullptr, nullptr
    );
    WriteFile(stdin_write, bytes.data(), byte_len, nullptr, nullptr);
}

void Ipc::sendLn() const {
    send(L"\n");
}

void Ipc::readPipe(HANDLE pipe, WSTR& out) {
    DWORD available = 0;
    PeekNamedPipe(pipe, nullptr, 0, nullptr, &available, nullptr);
    if (available == 0) {
        return;
    }
    char buffer[4096];
    DWORD read = 0;
    if (!ReadFile(pipe, buffer, sizeof(buffer), &read, nullptr) || read == 0) {
        return;
    }
    const int wide_len = MultiByteToWideChar(CP_UTF8, 0, buffer, read, nullptr, 0);
    if (wide_len <= 0) return;
    WSTR wide(wide_len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, buffer, read, wide.data(), wide_len);
    out += wide;
}

void Ipc::readOut(WSTR& out) const {
    readPipe(stdout_read, out);
}
void Ipc::readErr(WSTR& out) const {
    readPipe(stderr_read, out);
}

bool Ipc::active() const {
    DWORD exitCode;
    GetExitCodeProcess(process, &exitCode);
    return exitCode == STILL_ACTIVE;
}

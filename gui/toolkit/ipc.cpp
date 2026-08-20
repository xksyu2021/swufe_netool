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

Ipc::Ipc(const WSTRP program){
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
    pi.hProcess = process;

    CreateProcessW(
        nullptr,
        program,
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

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

void Ipc::send(const WSTRP in) const {
    WriteFile(
        stdin_write, in,
        wcslen(in) * sizeof(wchar_t),
        nullptr, nullptr
    );
}

void Ipc::readOut(WSTRP buffer, DWORD size) const {
    ReadFile(
        stdout_read, buffer,
        size,
        nullptr, nullptr
    );
}

void Ipc::readErr(WSTRP buffer, DWORD size) const {
    ReadFile(
        stderr_read, buffer,
        size,
        nullptr, nullptr
    );
}

bool Ipc::active() const {
    DWORD exitCode;
    GetExitCodeProcess(process, &exitCode);
    return exitCode == STILL_ACTIVE;
}

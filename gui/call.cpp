#include <thread>
#include "share.h"

static void analyze(const WSTR&);

namespace {
    Ipc *kernel = nullptr, *analyzer = nullptr;

    void knl_loop() {
        WSTR out, err;
        while (kernel && kernel->active()) {
            kernel->readOut(out);
            kernel->readErr(err);

            size_t isp_target = out.find(L"*isp#");
            size_t acc_target = out.find(L"*acc#");
            size_t pwd_target = out.find(L"*pwd#");
            size_t suc_target = out.find(L"*suc#");
            if (isp_target != WSTR::npos) {
                kernel->send(info.isp.c_str());
                kernel->sendLn();
                out.clear();
            }
            else if (acc_target != WSTR::npos) {
                kernel->send(info.acc.c_str());
                kernel->sendLn();
                out.clear();
            }
            else if (pwd_target != WSTR::npos) {
                kernel->send(info.pwd.c_str());
                kernel->sendLn();
                out.clear();
            }
            else if (suc_target != WSTR::npos) {
                kernel->sendLn();
                MessageBoxW(nullptr,
                            L"连接成功",
                            TITLE,
                            MB_OK
                );
                submit_lock = false;
                return;
            }

            if (err.find(L'#') != WSTR::npos) {
                kernel->sendLn();
                if (MessageBoxW(nullptr,
                                L"连接失败。\n是否启动分析器自动排查故障？",
                                TITLE,
                                MB_ICONERROR | MB_OKCANCEL
                ) == IDOK) {
                    analyze(err);
                }
                submit_lock = false;
                return;
            }
        }

        MessageBoxW(nullptr,
                    L"内核无法正常运行。\n请重新安装此软件，请勿改动软件内部文件。\n如无法解决，请联系开发者。",
                    TITLE,
                    MB_ICONERROR | MB_OK
        );
        submit_lock = false;
    }

    void alz_loop() {
        WSTR out;
        while (analyzer && analyzer->active()) {
            analyzer->readOut(out);

            if (out.find(L'#') != WSTR::npos) {
                analyzer->sendLn();
                // TODO: show the result.
                analyze_lock = false;
                return;
            }
        }

        MessageBoxW(mainHwnd,
                    L"分析器无法正常运行。\n请重新安装此软件，请勿改动软件内部文件。\n如无法解决，请联系开发者。",
                    TITLE,
                    MB_ICONERROR | MB_OK
        );
        analyze_lock = false;
    }
}

void submit() {
    kernel = new Ipc(L"kernel.exe");
    submit_lock = true;
    std::thread kernel_thread(knl_loop);
    kernel_thread.detach();
}

void analyze() {
    analyzer = new Ipc(L"analyzer.exe --fix");
    analyze_lock = true;
    std::thread analyzer_thread(alz_loop);
    analyzer_thread.detach();
}

static void analyze(const WSTR& message) {
    analyzer = new Ipc((L"analyzer.exe --code \"" + message + L"\"").data());
    analyze_lock = true;
    std::thread analyzer_thread(alz_loop);
    analyzer_thread.detach();
}
#include "share.h"

namespace MainLayout {
    auto ctr = Creator(mainHwnd, 16, 26, 0);

    enum id {
        LABLE_POS = 1000, LABLE_ISP,
        LABLE_ACC, LABLE_DOMAIN, LABLE_PWD,
        BUTTON_DORM, BUTTON_TEACH,
        BUTTON_CMCC, BUTTON_CUCC, BUTTON_CNET, BUTTON_ISWUFE, BUTTON_EDU,
        BUTTON_SUBMIT, BUTTON_ANALYZE, BUTTON_MANUAL,
        INPUT_ACC, INPUT_PWD
    };

    void setIsp() {
        if (ctr.isChecked(BUTTON_DORM) && ctr.isChecked(BUTTON_CMCC)) {
            info.isp = L"cmcc";
        } else if (ctr.isChecked(BUTTON_DORM) && ctr.isChecked(BUTTON_CUCC)) {
            info.isp = L"cucc";
        } else if (ctr.isChecked(BUTTON_DORM) && ctr.isChecked(BUTTON_CNET)) {
            info.isp = L"cnet";
        } else if (ctr.isChecked(BUTTON_TEACH) && ctr.isChecked(BUTTON_ISWUFE)) {
            info.isp = L"iswufe";
        } else if (ctr.isChecked(BUTTON_TEACH) && ctr.isChecked(BUTTON_EDU)) {
            info.isp = L"eduroam";
        }
    }

    void eduMode(bool bl) {
        if (bl) {
            ctr.setPos(
                INPUT_ACC,
                8,7,2,11,
                0
            );
        } else {
            ctr.setPos(
                INPUT_ACC,
                8,7,2,19,
                0
            );
        }
        ctr.setVisible(LABLE_DOMAIN, bl);
    }
    void teachMode(bool bl) {
        ctr.setVisible(BUTTON_CMCC, !bl);
        ctr.setVisible(BUTTON_CUCC, !bl);
        ctr.setVisible(BUTTON_CNET, !bl);
        ctr.setVisible(BUTTON_ISWUFE, bl);
        ctr.setVisible(BUTTON_EDU, bl);
    }

    void init() {
        ctr.reflesh();

        // static
        ctr.spwan(
            L"STATIC", L"位置", LABLE_POS,
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            2,2,2,4,0
        );
        ctr.spwan(
            L"STATIC", L"运营商", LABLE_ISP,
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            5,2,2,4,0
        );
        ctr.spwan(
            L"STATIC", L"学号", LABLE_ACC,
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            8,2,2,4,0
        );
        ctr.spwan(
            L"STATIC", L"@swufe.edu.cn", LABLE_DOMAIN,
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            8,19,2,7,0
        );
        ctr.spwan(
            L"STATIC", L"密码", LABLE_PWD,
            WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
            11,2,2,4,0
        );

        // pos options
        ctr.spwan(
            L"BUTTON", L"宿舍", BUTTON_DORM,
            WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
            2,7,2,5,0
        );
        ctr.spwan(
            L"BUTTON", L"教学区", BUTTON_TEACH,
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            2,13,2,5,0
        );

        // isp options
        ctr.spwan(
            L"BUTTON", L"移动", BUTTON_CMCC,
            WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
            5,7,2,5,0
        );
        ctr.spwan(
            L"BUTTON", L"联通", BUTTON_CUCC,
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            5,13,2,5,0
        );
        ctr.spwan(
            L"BUTTON", L"电信", BUTTON_CNET,
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            5,19,2,5,0
        );
        ctr.spwan(
            L"BUTTON", L"iSwufe", BUTTON_ISWUFE,
            WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
            5,7,2,5,0
        );
        ctr.spwan(
            L"BUTTON", L"eduroam", BUTTON_EDU,
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            5,13,2,5,0
        );

        // input
        ctr.spwan(
            L"EDIT", nullptr, INPUT_ACC,
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            8,7,2,19,0
        );
        ctr.spwan(
            L"EDIT", nullptr, INPUT_PWD,
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            11,7,2,19,0
        );

        // op button
        ctr.spwan(
            L"BUTTON", L"故障排查", BUTTON_ANALYZE,
            WS_VISIBLE | WS_CHILD,
            14,2,2,4,0
        );
        ctr.spwan(
            L"BUTTON", L"连接", BUTTON_SUBMIT,
            WS_VISIBLE | WS_CHILD,
            14,22,2,4,0
        );

        // set defaults
        eduMode(false);
        teachMode(false);
        SendMessage(
            ctr.getHwnd(BUTTON_DORM),
            BM_SETCHECK,
            BST_CHECKED,
            0
        );
        SendMessage(
            ctr.getHwnd(BUTTON_CMCC),
            BM_SETCHECK,
            BST_CHECKED,
            0
        );
        SendMessage(
            ctr.getHwnd(BUTTON_ISWUFE),
            BM_SETCHECK,
            BST_CHECKED,
            0
        );
    }

    void callback(WORD wmId) {
        if (ctr.isChecked(BUTTON_DORM)) {
            teachMode(false);
            eduMode(false);
        } else if (ctr.isChecked(BUTTON_EDU)) {
            teachMode(true);
            eduMode(true);
        } else {
            teachMode(true);
            eduMode(false);
        }

        switch (wmId) {
        case BUTTON_SUBMIT: {
            #define SIZE 1024
            setIsp();

            wchar_t acc_buffer[SIZE]{};
            GetWindowTextW(
                ctr.getHwnd(INPUT_ACC)
                , acc_buffer, _countof(acc_buffer)
            );
            info.acc = acc_buffer;

            wchar_t pwd_buffer[SIZE]{};
            GetWindowTextW(
                ctr.getHwnd(INPUT_PWD)
                , pwd_buffer, _countof(pwd_buffer)
            );
            info.pwd = pwd_buffer;

            if (!submit_lock) {
                submit();
            }
            break;
            #undef SIZE
        }
        case BUTTON_ANALYZE: {
            if (!analyze_lock) {
                analyze();
            }
            break;
        }
        case BUTTON_MANUAL: {
            // TODO: switch to manual toolkits menu
        }

        default: break;
        }
    }

    void del() {
        // TODO: switch to manual toolkits menu
    }
}
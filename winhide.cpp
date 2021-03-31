#include <iostream>
#include <list>
#include <windows.h>
#include "winhide.h"

using namespace std;
list<HWND> windows;

void SingleInstance() {
    HANDLE current_mutex = CreateMutexA(NULL, true, "Window Hider");
    if (current_mutex != 0 && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(current_mutex);
        exit(EXIT_FAILURE);
    }
}

void WindowState(HWND handle, int state, string action_str) {
    ShowWindow(handle, state);
    if (GetWindowTextLengthA(handle) == 0)
        cout << action_str << ": " << handle << endl;
    else {
        char title[BUFSIZ];
        GetWindowTextA(handle, title, GetWindowTextLengthA(handle) + 1);
        cout << action_str << ": \"" << title << "\"" << endl;
    }
}

BOOL WINAPI ExitHandler(DWORD type) {
    if (type == CTRL_C_EVENT || type == CTRL_CLOSE_EVENT) {
        for (auto window : windows){
            WindowState(window, SW_SHOW, "Window shown");
            windows.pop_front();
        }
        exit(EXIT_SUCCESS);
    }
}

void main() {
    SetConsoleTitleA("Window Hider");
    cout << "Window Hider v" << VERSION << endl;
    SingleInstance();
    SetConsoleCtrlHandler(ExitHandler, TRUE);
    MSG msg = { 0 };
    list<string> exclusions = { CLASS_DESKTOP, CLASS_DESKTOP_LAYER, CLASS_TASKBAR, CLASS_START_MENU, CLASS_NOTIFY_PANEL };
    int start_hidden = false;
    char hide_key = KEY_B;
    char cwd[BUFSIZ];
    GetCurrentDirectoryA(BUFSIZ, cwd);
    string ini = string(cwd) + "\\winhidecfg.ini";
    int start_hidden_ini = GetPrivateProfileIntA("Settings", "StartHidden", 0, ini.c_str());
    if (GetLastError() != 0)
        cerr << "Unable to read configuration file winhidecfg.ini, using defaults" << endl;
    else
        start_hidden = start_hidden_ini;

    LPSTR hide_key_ini = new CHAR[BUFSIZ];
    char key_code_ini = GetPrivateProfileStringA("Settings", "HideKey", LPCSTR(KEY_B), hide_key_ini, 2, ini.c_str());

    if (GetLastError() != 0)
        cerr << "Unable to read configuration file winhidecfg.ini, using defaults" << endl;
    else
        hide_key = hide_key_ini[0];

    if (start_hidden != false) {
        HWND console = GetConsoleWindow();
        WindowState(console, SW_HIDE, "Window hidden");
        windows.push_front(console);
    }

    if (RegisterHotKey(NULL, HOTKEY_HIDE, MOD_ALT | MOD_NOREPEAT, VkKeyScanExA(hide_key, GetKeyboardLayout(0))) &&
        RegisterHotKey(NULL, HOTKEY_SHOW, MOD_ALT | MOD_NOREPEAT, KEY_C)) {
        cout << "Alt+B (hide window) and Alt-C (show window) hotkeys registered" << endl;
    }
    else {
        cerr << "Failed to register hotkeys" << endl;
        exit(EXIT_FAILURE);
    }

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_HIDE) {
            HWND handle = GetForegroundWindow();
            char classname[BUFSIZ];
            GetClassNameA(handle, classname, BUFSIZ);
            if ((!windows.empty() && windows.front() == handle) || // prevent hiding the same window
                find(exclusions.begin(), exclusions.end(), classname) != exclusions.end()) // prevent hiding OS elements
                continue;
            WindowState(handle, SW_HIDE, "Window hidden");
            windows.push_front(handle);
        }
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_SHOW && !windows.empty()) {
            WindowState(windows.front(), SW_SHOW, "Window shown");
            windows.pop_front();
        }
    }
}
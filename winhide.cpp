#include <iostream>
#include <list>
#include <windows.h>

#define HOTKEY_HIDE 1
#define HOTKEY_SHOW 2
#define KEY_B 0x42
#define KEY_C 0x43
#define CLASS_DESKTOP "Progman"
#define CLASS_DESKTOP_LAYER "WorkerW"
#define CLASS_TASKBAR "Shell_TrayWnd"
#define CLASS_START_MENU "Windows.UI.Core.CoreWindow"
#define CLASS_NOTIFY_PANEL "NotifyIconOverflowWindow"

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
    SingleInstance();
    SetConsoleCtrlHandler(ExitHandler, TRUE);
    MSG msg = { 0 };
    SetConsoleTitleA("Window Hider");
    cout << "Window Hider v1.0" << endl;
    if (RegisterHotKey(NULL, HOTKEY_HIDE, MOD_ALT | MOD_NOREPEAT, KEY_B) &&
        RegisterHotKey(NULL, HOTKEY_SHOW, MOD_ALT | MOD_NOREPEAT, KEY_C)) {
        cout << "Alt+B (hide window) and Alt-C (show window) hotkeys registered" << endl;
    }
    else {
        cout << "Failed to register hotkeys" << endl;
    }

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_HIDE) {
            HWND handle = GetForegroundWindow();
            char classname[BUFSIZ];
            GetClassNameA(handle, classname, BUFSIZ);
            if ((!windows.empty() && windows.front() == handle) || // prevent rehiding the same window
                strcmp(classname, CLASS_DESKTOP) == 0 || // prevent hiding OS elements
                strcmp(classname, CLASS_DESKTOP_LAYER) == 0 ||
                strcmp(classname, CLASS_TASKBAR) == 0 ||
                strcmp(classname, CLASS_START_MENU) == 0 ||
                strcmp(classname, CLASS_NOTIFY_PANEL) == 0)
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
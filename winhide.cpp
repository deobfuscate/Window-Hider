#include <iostream>
#include <list>
#include <windows.h>

#define HOTKEY_HIDE 1
#define HOTKEY_SHOW 2
#define KEY_B 0x42
#define KEY_C 0x43

using namespace std;

void main() {
    MSG msg = { 0 };
    list<HWND> windows;
    if (RegisterHotKey(NULL, HOTKEY_HIDE, MOD_ALT | MOD_NOREPEAT, KEY_B) &&
        RegisterHotKey(NULL, HOTKEY_SHOW, MOD_ALT | MOD_NOREPEAT, KEY_C)) {
        cout << "Alt+B (hide window) and Alt-C (show window) hotkeys registered." << endl;
    }
    else {
        cout << "Failed to register hotkeys." << endl;
    }

    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_HIDE) {
            HWND handle = GetForegroundWindow();
            if (!windows.empty() && windows.front() == handle)
                continue; // prevent rehiding the same window
            char title[BUFSIZ];
            ShowWindow(handle, SW_HIDE);
            if (GetWindowTextLength(handle) == 0)
                cout << "The window: " << handle << " was hidden" << endl;
            else {
                GetWindowTextA(handle, title, GetWindowTextLength(handle) + 1);
                cout << "The window: \"" << title << "\" was hidden" << endl;
            }
            windows.push_front(handle);
        }
        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_SHOW && !windows.empty()) {
            ShowWindow(windows.front(), SW_SHOW);
            windows.pop_front();
        }
    }
}
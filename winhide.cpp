#include <iostream>
#include <list>
#include <windows.h>
using namespace std;

void main() {
    MSG msg = { 0 };
    list<HWND> windows;
    if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x42))  //0x42 = 'b'
    {
        printf("Hotkey ALT+b with MOD_NOREPEAT flag\n");
    }
    else {
        printf("Failed to register hotkey\n");
    }
    if (RegisterHotKey(NULL, 2, MOD_ALT | MOD_NOREPEAT, 0x43))  //0x42 = 'b'
    {
        printf("Hotkey ALT+c with MOD_NOREPEAT flag\n");
    }
    else {
        printf("Failed to register hotkey\n");
    }

    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY && msg.wParam == 1)
        {
            HWND hw = GetForegroundWindow();
            int bufsize = GetWindowTextLength(hw) + 1;
            char title[BUFSIZ];
            GetWindowTextA(hw, title, bufsize);
            cout << "The window: \"" << title << "\" was hidden " << hw << endl;
            windows.push_front(hw);
        }
        if (msg.message == WM_HOTKEY && msg.wParam == 2)
        {
            if (!windows.empty()) {
                windows.pop_front();
            }
        }
    }
}
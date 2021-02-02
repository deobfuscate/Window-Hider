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

    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            HWND hw = GetForegroundWindow();
            int bufsize = GetWindowTextLength(hw) + 1;
            char title[BUFSIZ];
            GetWindowTextA(hw, title, bufsize);
            cout << title << endl;
        }
    }
}
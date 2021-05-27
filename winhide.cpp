#include <iostream>
#include <list>
#include <windows.h>
#include "winhide.h"

using namespace std;
list<HWND> windows;

char ReadIniInt(LPCSTR category, LPCSTR key, int default_value, const char* ini_path) {
    int value = -1;
    int ini_value = GetPrivateProfileIntA(category, key, default_value, ini_path);
    auto last_error = GetLastError();
    if (last_error != false && last_error != ERROR_MORE_DATA)
        cerr << "Unable to find key, using default" << endl;
    else
        value = ini_value;
    return value;
}

char ReadIniString(LPCSTR category, LPCSTR key, int default_value, const char* ini_path) {
    char value = '\0';
    LPSTR ini_value = new CHAR[BUFSIZ];
    GetPrivateProfileStringA(category, key, LPCSTR(default_value), ini_value, 2, ini_path);
    auto last_error = GetLastError();
    if (last_error != FALSE && last_error != ERROR_MORE_DATA)
        cerr << "Unable to find key, using default" << endl;
    else
        value = VkKeyScanExA(ini_value[0], GetKeyboardLayout(0));
    return value;
}

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
    return true;
}

BOOL FileExists(string path) {
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

int main() {
    SetConsoleTitleA("Window Hider");
    cout << "Window Hider v" << VERSION << endl;
    SingleInstance();
    SetConsoleCtrlHandler(ExitHandler, TRUE);
    MSG msg = { FALSE };
    list<string> exclusions = { CLASS_DESKTOP, CLASS_DESKTOP_LAYER, CLASS_TASKBAR, CLASS_START_MENU, CLASS_NOTIFY_PANEL };
    int start_hidden = 0,
        mod_ctrl = 0,
        mod_alt = 0,
        mod_shift = 0,
        show_modifiers = 0,
        hide_modifiers = 0;
    char hide_key = KEY_B,
        show_key = KEY_C,
        cwd[BUFSIZ];
    GetCurrentDirectoryA(BUFSIZ, cwd);
    string ini_path = string(cwd) + "\\winhidecfg.ini";

    if (FileExists(ini_path)) {
        start_hidden = ReadIniInt("Settings", "StartHidden", 0, ini_path.c_str());
        if (ReadIniInt("HideHotKey", "Alt", 0, ini_path.c_str()) == 1)
            hide_modifiers |= MOD_ALT;
        if (ReadIniInt("HideHotKey", "Ctrl", 0, ini_path.c_str()) == 1)
            hide_modifiers |= MOD_CONTROL;
        if (ReadIniInt("HideHotKey", "Shift", 0, ini_path.c_str()) == 1)
            hide_modifiers |= MOD_SHIFT;
        hide_key = ReadIniString("HideHotKey", "Key", KEY_B, ini_path.c_str());
        if (hide_key == -1 || hide_modifiers == 0) {
            hide_modifiers = MOD_ALT;
            hide_key = KEY_B; 
            cerr << "Invalid hide key or no modifiers specified, using defaults" << endl;
        }

        if (ReadIniInt("ShowHotKey", "Alt", 0, ini_path.c_str()) == 1)
            show_modifiers |= MOD_ALT;
        if (ReadIniInt("ShowHotKey", "Ctrl", 0, ini_path.c_str()) == 1)
            show_modifiers |= MOD_CONTROL;
        if (ReadIniInt("ShowHotKey", "Shift", 0, ini_path.c_str()) == 1)
            show_modifiers |= MOD_SHIFT;
        show_key = ReadIniString("ShowHotKey", "Key", KEY_C, ini_path.c_str());
        if (show_key == -1 || show_modifiers == 0) {
            show_modifiers = MOD_ALT;
            show_key = KEY_C; 
            cerr << "Invalid show key or no modifiers specified, using defaults" << endl;
        }
    }
    else {
        cout << "Could not find configuration file winhidecfg.ini, using default settings" << endl;
        show_modifiers = MOD_ALT;
        hide_modifiers = MOD_ALT;
    }
    
    if (start_hidden != FALSE) {
        HWND console = GetConsoleWindow();
        WindowState(console, SW_HIDE, "Window hidden");
        windows.push_front(console);
    }

    if (RegisterHotKey(NULL, HOTKEY_HIDE, hide_modifiers | MOD_NOREPEAT, hide_key) &&
        RegisterHotKey(NULL, HOTKEY_SHOW, show_modifiers | MOD_NOREPEAT, show_key)) {
        cout << "Hide Window and Show Window hotkeys registered" << endl;
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
    return true;
}
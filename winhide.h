#define HOTKEY_HIDE 1
#define HOTKEY_SHOW 2

#define KEY_B 0x42
#define KEY_C 0x43

#define CLASS_DESKTOP "Progman"
#define CLASS_DESKTOP_LAYER "WorkerW"
#define CLASS_TASKBAR "Shell_TrayWnd"
#define CLASS_START_MENU "Windows.UI.Core.CoreWindow"
#define CLASS_NOTIFY_PANEL "NotifyIconOverflowWindow"

// esures only a single instance is run
void SingleInstance();

// modifies the window state of the given handle
void WindowState(HWND handle, int state, std::string action_str);

// handles the windows program exiting event to unhide all windows
BOOL WINAPI ExitHandler(DWORD type);
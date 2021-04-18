#define VERSION "1.1"

#define HOTKEY_HIDE 1
#define HOTKEY_SHOW 2

#define KEY_B 0x42
#define KEY_C 0x43

// OS elements
#define CLASS_DESKTOP "Progman" // desktop
#define CLASS_DESKTOP_LAYER "WorkerW"  // desktop
#define CLASS_TASKBAR "Shell_TrayWnd" // taskbar
#define CLASS_START_MENU "Windows.UI.Core.CoreWindow" // start menu / notification center
#define CLASS_NOTIFY_PANEL "NotifyIconOverflowWindow" // notification area popup

// reads the specified value from an ini file and returns it
char ReadIniString(LPCSTR category, LPCSTR key, int default_value, const char* ini_path);

// esures only a single instance is run
void SingleInstance();

// modifies the window state of the given handle
void WindowState(HWND handle, int state, std::string action_str);

// handles the windows program exiting event to unhide all windows
BOOL WINAPI ExitHandler(DWORD type);
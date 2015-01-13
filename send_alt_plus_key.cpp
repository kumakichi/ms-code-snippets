#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>

#define KEYEVENTF_UNICODE 0x0004

int main(void)
{
	HWND windowHandle = FindWindow("notepad", NULL);
	if (windowHandle == NULL) {
		printf("Main window not found, terminating\n");
		return -1;
	}

	HWND childWindow = GetWindow(windowHandle, GW_CHILD);
	if (childWindow == NULL) {
		printf("Child window not found\n");
		return -1;
	}
	SetForegroundWindow(childWindow);
	PostMessage(childWindow, WM_SYSKEYDOWN, 'O', 1 << 29);

	return 0;
}

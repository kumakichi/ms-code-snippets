// Because the SendInput function is only supported in
// Windows 2000 and later, WINVER needs to be set as
// follows so that SendInput gets defined when windows.h
// is included below.
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>

void PrivateSendKey(WORD usrKey);
void SendKeyStroke(LPCTSTR szClassName, WPARAM key);

int main()
{
	HWND hWnd =::FindWindowW(L"notepad", NULL);	//search Notepad handle
	HWND hWndc =::GetWindow(hWnd, GW_CHILD);
	if (hWndc) {

		//Do not use SendMessage
		PostMessage(hWndc, WM_CONTEXTMENU, (WPARAM) 0, (LPARAM) 0);	//context menu
		PostMessage(hWndc, WM_CHAR, 'p', NULL);

		//PrivateSendKey('p');
	}

	return 0;
}

void PrivateSendKey(WORD usrKey)
{
	INPUT ip;

	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;	// hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press the usrKey
	ip.ki.wVk = usrKey;	// virtual-key code for the "a" key
	ip.ki.dwFlags = 0;	// 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release the usrKey
	ip.ki.dwFlags = KEYEVENTF_KEYUP;	// KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));
}

void SendKeyStroke(LPCTSTR szClassName, WPARAM key)
{
	PostMessage(FindWindow(szClassName, NULL), WM_KEYDOWN, key, 0);
	PostMessage(FindWindow(szClassName, NULL), WM_KEYUP, key, 0);
}

// HookTool.cpp : Defines the entry point for the application.
//

#include "HookTool.h"

/*
#include <Windows.h>
#include <CommCtrl.h>

//
//Dummy WndProc, for registering a main window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int test()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	//
	//Registering and creating a main window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, WndProc, 0, 0, hInstance, LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 2),
		NULL, "SomeWindowClass", LoadIcon(NULL, IDI_APPLICATION) };
	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "SomeWindowClass", "The title of my window",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 240, 200,
		NULL, NULL, hInstance, NULL);

	//
	//Creating an -h1- edit control and an -updown_hwnd- up down control
	HWND h1 = CreateWindowEx(0, "EDIT", "edit",
		WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_RIGHT, 0, 0, 0, 0, hwnd, 0, NULL, NULL);

	HWND updown_hwnd = CreateWindowEx(0, UPDOWN_CLASS, "",
		UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT |
		WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0, hwnd, NULL, NULL, NULL);
	//
	//attempting to send UDM_SETBUDDY message, checking errors before and after
	int error_num = (int)GetLastError();
	CHAR buffer[200];
	wsprintf(buffer, "error number is %d", error_num);
	//MessageBox(NULL, buffer, "", MB_OK);

	MoveWindow(h1, 0, 0, 60, 28, TRUE);
	SendMessage(updown_hwnd, UDM_SETBUDDY, (WPARAM)h1, 0);
	std::thread([&]() {
		Sleep(2000);
		MoveWindow(h1, 10, 10, 60, 28, TRUE);
		SendMessage(updown_hwnd, UDM_SETBUDDY, (WPARAM)h1, 0);
		Sleep(2000);
		MoveWindow(h1, 100, 100, 60, 28, TRUE);
		SendMessage(updown_hwnd, UDM_SETBUDDY, (WPARAM)h1, 0);
		Sleep(2000);
		MoveWindow(h1, 200, 200, 60, 28, TRUE);
		SendMessage(updown_hwnd, UDM_SETBUDDY, (WPARAM)h1, 0);
		}).detach();
	error_num = (int)GetLastError();
	wsprintf(buffer, "error number is %d", error_num);
	//MessageBox(NULL, buffer, "", MB_OK);
	ShowWindow(hwnd, SW_NORMAL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//将虚拟键转换成字符消息
		DispatchMessage(&msg);//分发消息给系统，系统找到过程函数指针，通过指针调用过程函数，过程函数得到消息
	}
	return 0;
}
*/

START_ENTRY_BINARY_NOONLY()
{
    std::cout << "Hello CMake." << std::endl;
    CHAR szAppName[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, szAppName, sizeof(szAppName) / sizeof(*szAppName));
    if (argc == 1)
    {
        //RunCmdAdmin(szAppName, "AutoBoot", 1);
        //return 0;
    }
    else if (argc == 2)
    {
        //SetAutoBootWithRegistry(("SST"), ("Start Route"));
    }
    else
    {
        ;//
    }
	//test();

    //HMODULE hModule = LoadLibrary(TEXT("PpsSpy.dll"));
    //PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS | SND_SYNC);
    //Sleep(50);
    //TCHAR tWavName[MAX_PATH] = { 0 };
    //wsprintf(tWavName, TEXT("notify.wav"));
    //PlaySound(tWavName, NULL, SND_FILENAME | SND_SYNC);
    //Sleep(50);
    ThreadStartRoutine(NULL);
    //FreeLibrary(hModule);
    return 0;
}
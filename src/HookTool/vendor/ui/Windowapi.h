// Operator.h : Include file for standard system include files,
// or project specific include files.

#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#include <windows.h>
__inline static
void AppendEditText(HWND hWndEdit, LPCSTR pszFormat, ...)
{
	CHAR szText[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, pszFormat);
	_vsnprintf(szText, sizeof(szText) - 1, pszFormat, vlArgs);
	va_end(vlArgs);

	lstrcatA(szText, "\r\n");

	//以下两条语句为在edit中追加字符串
	SendMessageA(hWndEdit, EM_SETSEL, -2, -1);
	SendMessageA(hWndEdit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szText);

	//设置滚轮到末尾，这样就可以看到最新信息
	SendMessageA(hWndEdit, WM_VSCROLL, SB_BOTTOM, 0);
}
__inline static
void AppendEditText(HWND hWndEdit, LPCWSTR pszFormat, ...)
{
	WCHAR szText[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, pszFormat);
	_vsnwprintf(szText, sizeof(szText) - 1, pszFormat, vlArgs);
	va_end(vlArgs);

	lstrcatW(szText, L"\r\n");

	//以下两条语句为在edit中追加字符串
	SendMessageW(hWndEdit, EM_SETSEL, -2, -1);
	SendMessageW(hWndEdit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)szText);

	//设置滚轮到末尾，这样就可以看到最新信息
	SendMessageW(hWndEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

typedef struct RectArea {
public:
	RectArea(RectArea* pRectArea)
	{
		this->left = pRectArea->left;
		this->top = pRectArea->top;
		this->right = pRectArea->right;
		this->bottom = pRectArea->bottom;
	}
	RectArea(const RectArea& rectArea)
	{
		this->left = rectArea.left;
		this->top = rectArea.top;
		this->right = rectArea.right;
		this->bottom = rectArea.bottom;
	}
	RectArea(LONG left, LONG top, LONG right, LONG bottom)
	{
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
	}
	RectArea(const RECT& rect)
	{
		this->left = rect.left;
		this->top = rect.top;
		this->right = rect.right;
		this->bottom = rect.bottom;
	}
	RectArea(LPRECT lpRect)
	{
		this->left = lpRect->left;
		this->top = lpRect->top;
		this->right = lpRect->right;
		this->bottom = lpRect->bottom;
	}
	RectArea(LPPOINT lpPoint, LPSIZE lpSize)
	{
		this->left = lpPoint->x;
		this->top = lpPoint->y;
		this->right = lpSize->cx - this->left;
		this->bottom = lpSize->cy - this->top;
	}
	RectArea(const POINT& point, const SIZE& size)
	{
		this->left = point.x;
		this->top = point.y;
		this->right = size.cx - this->left;
		this->bottom = size.cy - this->top;
	}
public:
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
public:
	LONG X() {
		return left;
	}
	LONG Y() {
		return top;
	}
	LONG Width() {
		return (right - left);
	}
	LONG Height() {
		return (bottom - top);
	}
	void X(LONG X) {
		right = X + Width();
		left = X;
	}
	void Y(LONG Y) {
		bottom = Y + Height();
		top = Y;
	}
	void Width(LONG width) {
		right = left + width;
	}
	void Height(LONG height) {
		bottom = top + height;
	}
};
__inline static
LPRECT ScreenToDialog(HWND hWnd, LPRECT lpRect, INT nRectNum = 1)
{
	for (SIZE_T i = 0; i < nRectNum * sizeof(RECT) / sizeof(POINT); i++)
	{
		::ScreenToClient(hWnd, &((LPPOINT)lpRect)[i]);
	}
	return lpRect;
}

__inline static
LPPOINT ScreenToClient(HWND hWnd, LPPOINT lpPoint, INT nPointNum)
{
	for (SIZE_T i = 0; i < nPointNum; i++)
	{
		::ScreenToClient(hWnd, &((LPPOINT)lpPoint)[i]);
	}
	return lpPoint;
}

__inline static
LPRECT GetDlgItemRect(RECT& rcRect, HWND hWnd, INT nIDWndItem)
{
	::GetWindowRect(::GetDlgItem(hWnd, nIDWndItem), &rcRect);
	return ScreenToDialog(hWnd, &rcRect, sizeof(rcRect) / sizeof(RECT));
}

__inline static
void CalculateWindowSize(RECT& rect, INT nSpaceX, INT nSpaceY)
{
	rect.right -= nSpaceX;
	rect.bottom -= nSpaceY;
}
__inline static
void CalculateWindowSize(RECT& rect, INT nSpaceL, INT nSpaceT, INT nSpaceR, INT nSpaceB)
{
	rect.left += nSpaceL;
	rect.top += nSpaceT;
	rect.right -= nSpaceR;
	rect.bottom -= nSpaceB;
}
__inline static
void CalculateWindowSize(RECT& rect, const RECT& newRect, INT nSpaceX, INT nSpaceY)
{
	rect.right = newRect.right - nSpaceX;
	rect.bottom = newRect.bottom - nSpaceY;
}
__inline static
void CalculateWindowSize(RECT& rect, const RECT& newRect, INT nSpaceL, INT nSpaceT, INT nSpaceR, INT nSpaceB)
{
	rect.left = newRect.left + nSpaceL;
	rect.top = newRect.top + nSpaceT;
	rect.right = newRect.right - nSpaceR;
	rect.bottom = newRect.bottom - nSpaceB;
}
__inline static
void ResizeWindow(HWND hWnd, INT nIDWndItem, LPCRECT lpRect, BOOL bRepaint = TRUE)
{
	MoveWindow(GetDlgItem(hWnd, nIDWndItem), lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}
__inline static
void NotifyUpdateWindowControl(HWND hWnd, INT nIDWndItem, INT nSpaceX, INT nSpaceY)
{
	RECT rc = { 0 }; GetDlgItemRect(rc, hWnd, nIDWndItem);
	rc.right += nSpaceX; rc.bottom += nSpaceY; ::InvalidateRect(hWnd, &rc, TRUE);
}
__inline static
void NotifyUpdateWindowControl(HWND hWnd, INT nIDWndItem, INT nSpaceL, INT nSpaceT, INT nSpaceR, INT nSpaceB)
{
	RECT rc = { 0 }; GetDlgItemRect(rc, hWnd, nIDWndItem);
	rc.left -= nSpaceL;	rc.top -= nSpaceT;	rc.right += nSpaceR; rc.bottom += nSpaceB; ::InvalidateRect(hWnd, &rc, TRUE);
}

__inline static
void DisplayRequestRepaint(HWND hWnd, LONG width, LONG height, BOOL bRepaint)
{
	RECT rect = { 0 }; GetWindowRect(hWnd, &rect);
	rect.left -= (width - (rect.right - rect.left)) / 2;
	rect.top -= (height - (rect.bottom - rect.top)) / 2;
	rect.right = rect.left + width; rect.bottom = rect.top + height;
	MoveWindow(hWnd, rect.left, rect.top, width, height, bRepaint);
}

__inline static
void FillSolidRect(HDC hdc, LONG x, LONG y, LONG cx, LONG cy, COLORREF clr)
{
	RECT rect = { x,y,x + cx,y + cy };
	HBRUSH hBrush = CreateSolidBrush(clr);
	FillRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}
__inline static
void FillSolidRect(HDC hdc, LPRECT lpRect, COLORREF clr)
{
	FillSolidRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clr);
}
__inline static
void Draw3dRect(HDC hdc, LONG x, LONG y, LONG cx, LONG cy, COLORREF clrLeftTop, COLORREF clrRightBottom, LONG border = 1)
{
	FillSolidRect(hdc, x, y, border, cy - border, clrLeftTop); // left
	FillSolidRect(hdc, x, y, cx - border, border, clrLeftTop); // top
	FillSolidRect(hdc, x + cx, y, -border, cy, clrRightBottom); // right
	FillSolidRect(hdc, x, y + cy, cx, -border, clrRightBottom); // bottom
}
__inline static
void Draw3dRect(HDC hdc, LPRECT lpRect, COLORREF clrLeftTop, COLORREF clrRightBottom, LONG border = 1)
{
	Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrLeftTop, clrRightBottom, border);
}

#include <shellapi.h>
#include <string>
#include <unordered_map>
#if !defined(_UNICODE) && !defined(UNICODE)
#define TSTRING std::string
#else
#define TSTRING std::wstring
#endif
__inline static
void RegisterDropFilesEvent(HWND hWnd)
{
#ifndef WM_COPYGLOBALDATA
#define WM_COPYGLOBALDAYA	0x0049
#endif

#ifndef MSGFLT_ADD
#define MSGFLT_ADD 1
#endif

#ifndef MSGFLT_REMOVE
#define MSGFLT_REMOVE 2
#endif
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_ACCEPTFILES);
	typedef BOOL(WINAPI* LPFN_ChangeWindowMessageFilter)(__in UINT message, __in DWORD dwFlag);
	LPFN_ChangeWindowMessageFilter pfnChangeWindowMessageFilter = (LPFN_ChangeWindowMessageFilter)GetProcAddress(GetModuleHandle(TEXT("USER32.DLL")), ("ChangeWindowMessageFilter"));
	if (pfnChangeWindowMessageFilter)
	{
		pfnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(WM_COPYGLOBALDAYA, MSGFLT_ADD);// 0x0049 == WM_COPYGLOBALDATA
	}
}
__inline static
size_t GetDropFiles(std::unordered_map<TSTRING, TSTRING>* pttmap, HDROP hDropInfo)
{
	UINT nIndex = 0;
	UINT nNumOfFiles = 0;
	TCHAR tszFilePathName[MAX_PATH + 1] = { 0 };

	//得到文件个数
	nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for (nIndex = 0; nIndex < nNumOfFiles; nIndex++)
	{
		//得到文件名
		DragQueryFile(hDropInfo, nIndex, (LPTSTR)tszFilePathName, _MAX_PATH);
		pttmap->emplace(tszFilePathName, tszFilePathName);
	}

	DragFinish(hDropInfo);

	return nNumOfFiles;
}
__inline static
size_t GetDropFiles(std::vector<TSTRING>* ptv, HDROP hDropInfo)
{
	UINT nIndex = 0;
	UINT nNumOfFiles = 0;
	TCHAR tszFilePathName[MAX_PATH + 1] = { 0 };

	//得到文件个数
	nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for (nIndex = 0; nIndex < nNumOfFiles; nIndex++)
	{
		//得到文件名
		DragQueryFile(hDropInfo, nIndex, (LPTSTR)tszFilePathName, _MAX_PATH);
		ptv->emplace_back(tszFilePathName);
	}

	DragFinish(hDropInfo);

	return nNumOfFiles;
}
//显示在屏幕中央
__inline static
void CenterWindowInScreen(HWND hWnd)
{
	RECT rcWindow = { 0 };
	RECT rcScreen = { 0 };
	SIZE szAppWnd = { 0, 0 };
	POINT ptAppWnd = { 0, 0 };

	// Get workarea rect.
	BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA,   // Get workarea information
		0,              // Not used
		&rcScreen,    // Screen rect information
		0);             // Not used

	GetWindowRect(hWnd, &rcWindow);
	szAppWnd.cx = rcWindow.right - rcWindow.left;
	szAppWnd.cy = rcWindow.bottom - rcWindow.top;

	//居中显示
	ptAppWnd.x = (rcScreen.right - rcScreen.left - szAppWnd.cx) / 2;
	ptAppWnd.y = (rcScreen.bottom - rcScreen.top - szAppWnd.cy) / 2;
	MoveWindow(hWnd, ptAppWnd.x, ptAppWnd.y, szAppWnd.cx, szAppWnd.cy, TRUE);
}

//显示在父窗口中央
__inline static
void CenterWindowInParent(HWND hWnd, HWND hParentWnd)
{
	RECT rcWindow = { 0 };
	RECT rcScreen = { 0 };
	RECT rcParent = { 0 };
	SIZE szAppWnd = { 0, 0 };
	POINT ptAppWnd = { 0, 0 };
	// Get workarea rect.
	BOOL fResult = SystemParametersInfo(SPI_GETWORKAREA,   // Get workarea information
		0,              // Not used
		&rcScreen,    // Screen rect information
		0);             // Not used

	GetWindowRect(hParentWnd, &rcParent);
	GetWindowRect(hWnd, &rcWindow);
	szAppWnd.cx = rcWindow.right - rcWindow.left;
	szAppWnd.cy = rcWindow.bottom - rcWindow.top;

	//居中显示
	if ((rcWindow.right - rcWindow.left) > (rcParent.right - rcParent.left))
	{
		ptAppWnd.x = (rcScreen.right - rcScreen.left - szAppWnd.cx) / 2;
	}
	else
	{
		ptAppWnd.x = (rcParent.right + rcParent.left - szAppWnd.cx) / 2;
	}
	if ((rcWindow.bottom - rcWindow.top) > (rcParent.bottom - rcParent.top))
	{
		ptAppWnd.y = (rcScreen.bottom - rcScreen.top - szAppWnd.cy) / 2;
	}
	else
	{
		ptAppWnd.y = (rcParent.bottom + rcParent.top - szAppWnd.cy) / 2;
	}
	MoveWindow(hWnd, ptAppWnd.x, ptAppWnd.y, szAppWnd.cx, szAppWnd.cy, TRUE);
}
enum Encode { ANSI = 1, UTF16_LE, UTF16_BE, UTF8_BOM, UTF8 };

__inline static
Encode IsUtf8Data(const uint8_t* data, size_t size)
{
	bool bAnsi= true;
	uint8_t ch = 0x00;
	int32_t nBytes = 0;

	for (auto i = 0; i < size; i++)
	{
		ch = *(data + i);
		if ((ch & 0x80) != 0x00)
		{
			bAnsi = false;
		}
		if (nBytes == 0)
		{
			if (ch >= 0x80)
			{
				if (ch >= 0xFC && ch <= 0xFD)
				{
					nBytes = 6;
				}
				else if (ch >= 0xF8)
				{
					nBytes = 5;
				}
				else if (ch >= 0xF0)
				{
					nBytes = 4;
				}
				else if (ch >= 0xE0)
				{
					nBytes = 3;
				}
				else if (ch >= 0xC0)
				{
					nBytes = 2;
				}
				else
				{
					return Encode::ANSI;
				}
				nBytes--;
			}
		}
		else
		{
			if ((ch & 0xC0) != 0x80)
			{
				return Encode::ANSI;
			}
			nBytes--;
		}
	}
	if (nBytes > 0 || bAnsi)
	{
		return Encode::ANSI;
	}
	return Encode::UTF8;
}
__inline static
Encode DetectEncode(const uint8_t* data, size_t size)
{
	if (size > 2 && data[0] == 0xFF && data[1] == 0xFE)
	{
		return Encode::UTF16_LE;
	}
	else if (size > 2 && data[0] == 0xFE && data[1] == 0xFF)
	{
		return Encode::UTF16_BE;
	}
	else if (size > 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
	{
		return Encode::UTF8_BOM;
	}
	else
	{
		return IsUtf8Data(data, size);
	}
}

__inline static
LPBYTE TakeResource(LPBYTE* lpData, DWORD& dwSize, DWORD dwResID, LPCTSTR lpResType, HMODULE hModule = GetModuleHandle(NULL))
{
	if (lpData == NULL)
	{
		return NULL;
	}
	HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(dwResID), lpResType);
	if (hRsrc)
	{
		HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
		if (hGlobal)
		{
			LPVOID pData = LockResource(hGlobal);
			if (pData)
			{
				if ((dwSize = SizeofResource(hModule, hRsrc)) > 0)
				{
					*lpData = (LPBYTE)malloc(dwSize * sizeof(BYTE));
					CopyMemory(*lpData, pData, dwSize);
				}
				UnlockResource(hGlobal);
			}
			FreeResource(hRsrc);
		}
	}
	return (*lpData);
}

__inline static
VOID FreeResource(LPBYTE* lpData)
{
	if (lpData != NULL && (*lpData) != NULL)
	{
		free(*lpData);
	}
}

__inline static 
std::string TakeResourceData(DWORD dwResID, LPCTSTR lpResType, HMODULE hModule = GetModuleHandle(NULL))
{
	std::string result = ("");
	LPBYTE lpData = NULL;
	DWORD dwSize = 0L;
	TakeResource(&lpData, dwSize, dwResID, lpResType, hModule);
	if (lpData != NULL)
	{
		result.assign((const char *)lpData, dwSize);
		FreeResource(lpData);
	}
	return result;
}

//判断目录是否存在，若不存在则创建
__inline static 
BOOL CreateCascadeDirectory(LPCSTR lpPathName, //Directory name
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL  // Security attribute
)
{
	CHAR* pToken = NULL;
	CHAR czPathTemp[MAX_PATH] = { 0 };
	CHAR czPathName[MAX_PATH] = { 0 };

	strcpy(czPathName, lpPathName);
	pToken = strtok(czPathName, ("\\"));
	while (pToken)
	{
		sprintf(czPathTemp, ("%s%s\\"), czPathTemp, pToken);
		//创建失败时还应删除已创建的上层目录，此次略
		if (!CreateDirectoryA(czPathTemp, lpSecurityAttributes))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_ALREADY_EXISTS && dwError != ERROR_ACCESS_DENIED)
			{
				return FALSE;
			}
		}
		pToken = strtok(NULL, ("\\"));
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 函数说明：遍历目录获取指定文件列表
// 参    数：输出的文件行内容数据、过滤后缀名、过滤的前缀字符
// 返 回 值：bool返回类型，成功返回true；失败返回false
// 编 写 者: ppshuai 20141112
//////////////////////////////////////////////////////////////////////////
__inline static 
BOOL DirectoryTraversalFile(std::vector<std::string>* pTV, LPCSTR lpRootPath/* = TEXT(".")*/, LPCSTR lpExtension/* = ("*.*")*/)
{
	BOOL bResult = FALSE;
	HANDLE hFindFile = NULL;
	WIN32_FIND_DATAA wfd = { 0 };
	CHAR czFindPath[MAX_PATH + 1] = { 0 };

	//构建遍历根目录
	sprintf(czFindPath, ("%s%s"), lpRootPath, lpExtension);

	hFindFile = FindFirstFileExA(czFindPath, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	//hFindFile = FindFirstFile(tRootPath, &wfd);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
			{
				pTV->push_back(std::string(std::string(lpRootPath) + wfd.cFileName));
			}
			else
			{
				if (strcmp(wfd.cFileName, (".")) && strcmp(wfd.cFileName, ("..")))
				{
					bResult = DirectoryTraversalFile(pTV, std::string(std::string(lpRootPath) + wfd.cFileName + ("\\")).c_str(), lpExtension);
				}
			}
		} while (FindNextFile(hFindFile, &wfd));

		FindClose(hFindFile);
		hFindFile = NULL;
		bResult = TRUE;
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////
// 函数说明：遍历目录获取指定目录列表
// 参    数：输出的文件行内容数据
// 返 回 值：bool返回类型，成功返回true；失败返回false
// 编 写 者: ppshuai 20141112
//////////////////////////////////////////////////////////////////////////
__inline static
BOOL DirectoryTraversalPath(std::vector<std::string>* pTV, LPCSTR lpRootPath/* = TEXT(".")*/, LPCSTR lpExtension/* = ("*.*")*/)
{
	BOOL bResult = FALSE;
	HANDLE hFindFile = NULL;
	WIN32_FIND_DATAA wfd = { 0 };
	CHAR czFindPath[MAX_PATH + 1] = { 0 };

	//构建遍历根目录
	sprintf(czFindPath, ("%s%s"), lpRootPath, lpExtension);

	hFindFile = FindFirstFileExA(czFindPath, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	//hFindFile = FindFirstFile(tRootPath, &wfd);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				pTV->push_back(std::string(std::string(lpRootPath) + wfd.cFileName));
			}
			else
			{
				if (strcmp(wfd.cFileName, (".")) && strcmp(wfd.cFileName, ("..")))
				{
					bResult = DirectoryTraversalPath(pTV, std::string(std::string(lpRootPath) + wfd.cFileName + ("\\")).c_str(), lpExtension);
				}
			}
		} while (FindNextFile(hFindFile, &wfd));

		FindClose(hFindFile);
		hFindFile = NULL;
		bResult = TRUE;
	}

	return bResult;
}

__inline static
BOOL ExecuteCommand(LPCSTR lpCmdLine, BOOL & bStatus, DWORD dwMilliseconds = INFINITE)
{
	BOOL bRet = FALSE;
	DWORD dwExitCode = 0;
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfoA(&si);
	si.hStdError = NULL;
	si.hStdOutput = NULL;
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcessA(NULL,   // No module name (use command line)
		(LPSTR)lpCmdLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,           // Pointer to STARTUPINFO structure
		&pi            // Pointer to PROCESS_INFORMATION structure
	))
	{
		bStatus = TRUE;
		WaitForSingleObject(pi.hProcess, dwMilliseconds);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		bRet = TRUE;
	}

	return bRet;
}
__inline static
BOOL ExecuteCommand(LPCSTR lpCmdLine, DWORD dwMilliseconds = INFINITE)
{
	BOOL bStatus = FALSE;
	return ExecuteCommand(lpCmdLine, bStatus, dwMilliseconds);
}
__inline static
BOOL ExecuteCommand(LPCWSTR lpCmdLine, BOOL& bStatus, DWORD dwMilliseconds = INFINITE)
{
	BOOL bRet = FALSE;
	DWORD dwExitCode = 0;
	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	GetStartupInfoW(&si);
	si.hStdError = NULL;
	si.hStdOutput = NULL;
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcessW(NULL,   // No module name (use command line)
		(LPWSTR)lpCmdLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,           // Pointer to STARTUPINFO structure
		&pi            // Pointer to PROCESS_INFORMATION structure
	))
	{
		bStatus = TRUE;
		WaitForSingleObject(pi.hProcess, dwMilliseconds);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		bRet = TRUE;
	}

	return bRet;
}
__inline static
BOOL ExecuteCommand(LPCWSTR lpCmdLine, DWORD dwMilliseconds = INFINITE)
{
	BOOL bStatus = FALSE;
	return ExecuteCommand(lpCmdLine, bStatus, dwMilliseconds);
}

#include <shellapi.h>
__inline static
BOOL RunCmdAdmin(LPCSTR lpFile, LPCSTR lpParameters, DWORD dwMilliseconds = INFINITE)
{
	BOOL bRet = FALSE;
	SHELLEXECUTEINFOA execinfo = { 0 };
	memset(&execinfo, 0, sizeof(execinfo));
	execinfo.lpFile = lpFile;
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = ("runas");
	execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_SHOWDEFAULT;
	execinfo.lpParameters = lpParameters;

	bRet=ShellExecuteExA(&execinfo);
	if (bRet)
	{
		DWORD dwStatus = GetLastError();
		if (dwStatus == ERROR_CANCELLED)
		{
			//MessageBox(NULL, "提升权限出错", "提示", MB_OK);
		}
		WaitForSingleObject(execinfo.hProcess, dwMilliseconds);
	}
	return bRet;
}
__inline static
BOOL RunCmdAdmin(LPCWSTR lpFile, LPCWSTR lpParameters, DWORD dwMilliseconds = INFINITE)
{
	BOOL bRet = FALSE;
	SHELLEXECUTEINFOW execinfo = { 0 };
	memset(&execinfo, 0, sizeof(execinfo));
	execinfo.lpFile = lpFile;
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = (L"runas");
	execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_SHOWDEFAULT;
	execinfo.lpParameters = lpParameters;

	bRet=ShellExecuteExW(&execinfo);
	if (bRet)
	{
		DWORD dwStatus = GetLastError();
		if (dwStatus == ERROR_CANCELLED)
		{
			//MessageBox(NULL, "提升权限出错", "提示", MB_OK);
		}
		WaitForSingleObject(execinfo.hProcess, dwMilliseconds);
	}
	return bRet;
}
// 程序开机自动启动
__inline static
BOOL SetAutoBootWithRegistry(LPCSTR lpValueName, LPCSTR lpParamLine = NULL)
{
	BOOL bRet = FALSE;
	HKEY hOpenKey = NULL;
	CHAR szModuleFileName[MAX_PATH + MAX_PATH] = { 0 };
	LPCSTR szAutoBootPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	if (RegOpenKeyA(HKEY_LOCAL_MACHINE, szAutoBootPath, &hOpenKey) == ERROR_SUCCESS)
	{
		GetModuleFileNameA(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(*szModuleFileName));
		if (lpParamLine != NULL)
		{
			wsprintfA(szModuleFileName, ("%s %s"), szModuleFileName, lpParamLine);
		}
		RegSetValueExA(hOpenKey, lpValueName, 0, REG_SZ, (LPBYTE)szModuleFileName, (lstrlenA(szModuleFileName) + 1) * sizeof(CHAR));
		RegCloseKey(hOpenKey);
	}
	return bRet;
}
__inline static
BOOL SetAutoBootWithRegistry(LPCWSTR lpName, LPCWSTR lpParamLine = NULL)
{
	BOOL bRet = FALSE;
	HKEY hOpenKey = NULL;
	WCHAR szModuleFileName[MAX_PATH] = { 0 };
	LPCWSTR szAutoBootPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	if (RegOpenKeyW(HKEY_LOCAL_MACHINE, szAutoBootPath, &hOpenKey) == ERROR_SUCCESS)
	{
		GetModuleFileNameW(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(*szModuleFileName));
		if (lpParamLine != NULL)
		{
			wsprintfW(szModuleFileName, (L"%s %s"), szModuleFileName, lpParamLine);
		}
		RegSetValueExW(hOpenKey, lpName, 0, REG_SZ, (LPBYTE)szModuleFileName, (lstrlenW(szModuleFileName) + 1) * sizeof(CHAR));
		RegCloseKey(hOpenKey);
	}
	return bRet;
}
__inline static
BOOL SetAutoBootWithTask(LPCSTR lpTaskName, LPCSTR lpParamLine = NULL)
{
	CHAR szModuleFileName[MAX_PATH] = { 0 };
	CHAR szAutoRunCmd[MAX_PATH + MAX_PATH + MAX_PATH + MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(*szModuleFileName));
	if (lpParamLine != NULL)
	{
		wsprintfA(szModuleFileName, ("%s %s"), szModuleFileName, lpParamLine);
	}
	wsprintfA(szAutoRunCmd, ("schtasks /create /tn \"%s\" /tr %s /sc onstart /rl highest /f"), lpTaskName, szModuleFileName);
	return ExecuteCommand(szAutoRunCmd);
}
__inline static
BOOL SetAutoBootWithTask(LPCWSTR lpTaskName, LPCWSTR lpParamLine = NULL)
{
	WCHAR szModuleFileName[MAX_PATH] = { 0 };
	WCHAR szAutoRunCmd[MAX_PATH + MAX_PATH + MAX_PATH + MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szModuleFileName, sizeof(szModuleFileName) / sizeof(*szModuleFileName));
	if (lpParamLine != NULL)
	{
		wsprintfW(szModuleFileName, (L"%s %s"), szModuleFileName, lpParamLine);
	}
	wsprintfW(szAutoRunCmd, (L"schtasks /create /tn \"%s\" /tr %s /sc onstart /rl highest /f"), lpTaskName, szModuleFileName);
	return ExecuteCommand(szAutoRunCmd);
}
#include <tlhelp32.h>
__inline static
BOOL FindProcessName(LPCWSTR lpName)
{
	BOOL bFound = FALSE;
	BOOL bResult = FALSE;
	PROCESSENTRY32W processEntry32 = { 0 };
	processEntry32.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		bResult = Process32FirstW(hSnapshot, &processEntry32);
		while (bResult)
		{
			if (lstrcmpiW(processEntry32.szExeFile, lpName) == 0)
			{
				bFound = TRUE;
				break;
			}
			bResult = Process32NextW(hSnapshot, &processEntry32);
		}
		CloseHandle(hSnapshot);
	}
	return bFound;
}

__inline static
BOOL FindProcessName(LPCSTR lpName)
{
	BOOL bFound = FALSE;
	BOOL bResult = FALSE;
	PROCESSENTRY32 processEntry32 = { 0 };
	processEntry32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		bResult = Process32First(hSnapshot, &processEntry32);
		while (bResult)
		{
			if (lstrcmpiA(processEntry32.szExeFile, lpName) == 0)
			{
				bFound = TRUE;
				break;
			}
			bResult = Process32Next(hSnapshot, &processEntry32);
		}
		CloseHandle(hSnapshot);
	}
	return bFound;
}
#include <commdlg.h>
__inline static
BOOL SelectSaveFile(CHAR(&tFileName)[MAX_PATH])
{
	BOOL bResult = FALSE;
	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.lpstrFilter = ("Execute Files (*.EXE)\0*.EXE\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = tFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	bResult = GetSaveFileNameA(&ofn);
	if (bResult == FALSE)
	{
		//dwError = CommDlgExtendedError();
		//return bResult;
	}
	return bResult;
}
__inline static
BOOL SelectSaveFile(WCHAR(&tFileName)[MAX_PATH])
{
	BOOL bResult = FALSE;
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.lpstrFilter = (L"Execute Files (*.EXE)\0*.EXE\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = tFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	bResult = GetSaveFileNameW(&ofn);
	if (bResult == FALSE)
	{
		//dwError = CommDlgExtendedError();
		//return bResult;
	}
	return bResult;
}
__inline static
BOOL SelectOpenFile(CHAR(&tFileName)[MAX_PATH], LPCSTR lpFilter= ("Execute Files (*.EXE)\0*.EXE\0All Files (*.*)\0*.*\0\0"), LPCSTR lpstrTitle = NULL)
{
	BOOL bResult = FALSE;
	OPENFILENAMEA ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.lpstrFilter = lpFilter;
	ofn.lpstrFile = tFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = lpstrTitle;
	ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	bResult = GetOpenFileNameA(&ofn);
	if (bResult == FALSE)
	{
		//dwError = CommDlgExtendedError();
		//return bResult;
	}
	return bResult;
}
__inline static
BOOL SelectOpenFile(WCHAR(&tFileName)[MAX_PATH], LPCWSTR lpFilter = (L"Execute Files (*.EXE)\0*.EXE\0All Files (*.*)\0*.*\0\0"), LPCWSTR lpstrTitle=NULL)
{
	BOOL bResult = FALSE;
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = lpFilter;
	ofn.lpstrFile = tFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = lpstrTitle;
	ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
	bResult = GetOpenFileNameW(&ofn);
	if (bResult == FALSE)
	{
		//dwError = CommDlgExtendedError();
		//return bResult;
	}
	return bResult;
}
#include <shlobj.h>

__inline static
BOOL SelectOpenPath(CHAR(&tPathName)[MAX_PATH])
{
	BOOL bResult = FALSE;
	LPITEMIDLIST lpItemIDList = NULL;
	BROWSEINFOA bi = { 0 };
	bi.hwndOwner = NULL;
	bi.iImage = 0;
	bi.lParam = NULL;
	bi.lpfn = NULL;
	bi.lpszTitle = ("Please select path");
	bi.pszDisplayName = tPathName;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX;

	lpItemIDList = SHBrowseForFolderA(&bi);
	if (NULL != lpItemIDList)
	{
		bResult = SHGetPathFromIDListA(lpItemIDList, tPathName);
		if (bResult == FALSE)
		{
			//dwError = CommDlgExtendedError();
			//return bResult;
		}
		CoTaskMemFree(lpItemIDList);
	}
	return bResult;
}
__inline static
BOOL SelectOpenPath(WCHAR(&tPathName)[MAX_PATH])
{
	BOOL bResult = FALSE;
	LPITEMIDLIST lpItemIDList = NULL;
	BROWSEINFOW bi = { 0 };
	bi.hwndOwner = NULL;
	bi.iImage = 0;
	bi.lParam = NULL;
	bi.lpfn = NULL;
	bi.lpszTitle = (L"Please select path");
	bi.pszDisplayName = tPathName;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX;

	lpItemIDList = SHBrowseForFolderW(&bi);
	if (NULL != lpItemIDList)
	{
		bResult = SHGetPathFromIDListW(lpItemIDList, tPathName);

		if (bResult == FALSE)
		{
			//dwError = CommDlgExtendedError();
			//return bResult;
		}
		CoTaskMemFree(lpItemIDList);
	}
	return bResult;
}

#include <winver.h>
#pragma comment(lib, "version")
__inline static
int GetFileCurrentVersion(LPTSTR lpVersion, DWORD dwVersionNum, LPCTSTR lpFileName) {
    TCHAR tMessage[MAX_PATH] = { 0 };
    DWORD fileVersionSize = GetFileVersionInfoSize(lpFileName, 0);
    TCHAR* lpData = new TCHAR[fileVersionSize];
    if (!GetFileVersionInfo(lpFileName, NULL, fileVersionSize, lpData)) {
        wsprintf(tMessage, TEXT("错误1：%d"), GetLastError());
        MessageBox(NULL, tMessage, tMessage, MB_OK);
        delete lpData;
        return (-1);
    }
    VS_FIXEDFILEINFO* versionInfo;
    UINT uLen = 0;
    if (!VerQueryValue(lpData, TEXT("\\"), (LPVOID*)&versionInfo, &uLen)) {
        wsprintf(tMessage, TEXT("错误2：%d"), GetLastError());
        MessageBox(NULL, tMessage, tMessage, MB_OK);
        delete lpData;
        return (-1);
    }
    //产品版本
    wsprintf(lpVersion, TEXT("%d.%d.%d.%d"),
        HIWORD(versionInfo->dwProductVersionMS), LOWORD(versionInfo->dwProductVersionMS),
        HIWORD(versionInfo->dwProductVersionLS), LOWORD(versionInfo->dwProductVersionLS)
    );
    //文件版本
    wsprintf(lpVersion, TEXT("%d.%d.%d.%d"),
        HIWORD(versionInfo->dwFileVersionMS), LOWORD(versionInfo->dwFileVersionMS),
        HIWORD(versionInfo->dwFileVersionLS), LOWORD(versionInfo->dwFileVersionLS)
    );
    delete lpData;
    return 0;
}

//  Forward declarations:
__inline static
std::string PrintError(DWORD dwError, const std::string & strMsg)
{
	std::string msg(strMsg.data(), strMsg.size());
	LPVOID lpMsg = NULL;
	DWORD dwLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPSTR)&lpMsg, 0, NULL);
	if (lpMsg != NULL)
	{
		if (dwLen > 0)
		{
			msg.append((const CHAR*)lpMsg, dwLen);
		}
		LocalFree(lpMsg);
	}
	return msg;
}
__inline static
std::wstring PrintError(DWORD dwError, const std::wstring& strMsg)
{
	std::wstring msg(strMsg.data(), strMsg.size());
	LPVOID lpMsg = NULL;
	DWORD dwLen = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPWSTR)&lpMsg, 0, NULL);
	if (lpMsg != NULL)
	{
		if (dwLen > 0)
		{
			msg.append((const WCHAR*)lpMsg, dwLen);
		}
		LocalFree(lpMsg);
	}
	return msg;
}
__inline static
LPBYTE GetProcessModuleHandleByPid(DWORD dwPID, LPCSTR lpModuleName = ("xxx.dll"))
{
    LPBYTE lpBaseAddr = NULL;
    MODULEENTRY32 me32 = { 0 };
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap != INVALID_HANDLE_VALUE)
    {
        // Set the size of the structure before using it.
        me32.dwSize = sizeof(me32);
        if (Module32First(hModuleSnap, &me32) == TRUE)
        {
            do
            {
                if (lstrcmpiA(lpModuleName, me32.szExePath + lstrlenA(me32.szExePath) - lstrlenA(lpModuleName)) == 0)
                {
                    lpBaseAddr = me32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hModuleSnap, &me32) == TRUE);
        }
    }

    CloseHandle(hModuleSnap);
    return lpBaseAddr;
}
__inline static
LPBYTE GetProcessModuleHandleByPid(DWORD dwPID, LPCWSTR lpModuleName = (L"xxx.dll"))
{
	LPBYTE lpBaseAddr = NULL;
	MODULEENTRY32W me32 = { 0 };
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap != INVALID_HANDLE_VALUE)
	{
		// Set the size of the structure before using it.
		me32.dwSize = sizeof(me32);
		if (Module32FirstW(hModuleSnap, &me32) == TRUE)
		{
			do
			{
				if (lstrcmpiW(lpModuleName, me32.szExePath + lstrlenW(me32.szExePath) - lstrlenW(lpModuleName)) == 0)
				{
					lpBaseAddr = me32.modBaseAddr;
					break;
				}
			} while (Module32NextW(hModuleSnap, &me32) == TRUE);
		}
	}

	CloseHandle(hModuleSnap);
	return lpBaseAddr;
}
__inline static
DWORD GetProcessIdByName(LPCSTR lpProcessName = ("xxx.exe"))
{
	DWORD dwProcessID = 0;
	PROCESSENTRY32 pe32 = { 0 };
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(pe32);
		if (Process32First(hProcessSnap, &pe32) == TRUE)
		{
			do
			{
				if (lstrcmpiA(pe32.szExeFile, lpProcessName) == 0)
				{
					// List the modules and threads associated with this process
					dwProcessID = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
	}
	CloseHandle(hProcessSnap);
	return (dwProcessID);
}
__inline static
DWORD GetProcessIdByName(LPCWSTR lpProcessName = (L"xxx.exe"))
{
	DWORD dwProcessID = 0;
	PROCESSENTRY32W pe32 = { 0 };
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(pe32);
		if (Process32FirstW(hProcessSnap, &pe32) == TRUE)
		{
			do
			{
				if (lstrcmpiW(pe32.szExeFile, lpProcessName) == 0)
				{
					// List the modules and threads associated with this process
					dwProcessID = pe32.th32ProcessID;
					break;
				}
			} while (Process32NextW(hProcessSnap, &pe32));
		}
	}
	CloseHandle(hProcessSnap);
	return (dwProcessID);
}
__inline static
BOOL GetProcessMemoryBuffer(HANDLE hProcess, LPCVOID lpBaseAddr, LPSTR lpBuffer, SIZE_T nBufferSize)
{
    SIZE_T nNumberOfBytesRead = 0;
    return (ReadProcessMemory(
        hProcess,
        lpBaseAddr,
		(LPVOID)lpBuffer,
		nBufferSize,
        &nNumberOfBytesRead) == TRUE &&
        (nNumberOfBytesRead > 0));
}
__inline static
BOOL GetProcessMemoryBuffer(HANDLE hProcess, LPCVOID lpBaseAddr, LPWSTR lpBuffer, SIZE_T nBufferSize)
{
	SIZE_T nNumberOfBytesRead = 0;
	return (ReadProcessMemory(
		hProcess,
		lpBaseAddr,
		(LPVOID)lpBuffer,
		nBufferSize * sizeof(WCHAR),
		&nNumberOfBytesRead) == TRUE &&
		(nNumberOfBytesRead > 0));
}

__inline static
HANDLE OpenProcessByName(LPCSTR lpProcName = ("xxx.exe"))
{
    PROCESSENTRY32 pe32 = { 0 };
    HANDLE hProcess = INVALID_HANDLE_VALUE; 
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

    //打开快照
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE)
    {
        pe32.dwSize = sizeof(pe32);
        if (Process32First(hProcessSnap, &pe32) == TRUE)
        {
            while (Process32Next(hProcessSnap, &pe32) == TRUE)
            {
                if (lstrcmpiA(pe32.szExeFile, lpProcName) == 0)
                {
                    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                    break;
                }
            }
        }
    }
    CloseHandle(hProcessSnap);
    return hProcess;
}
__inline static
HANDLE OpenProcessByName(LPCWSTR lpProcName = (L"xxx.exe"))
{
	PROCESSENTRY32W pe32 = { 0 };
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

	//打开快照
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(pe32);
		if (Process32FirstW(hProcessSnap, &pe32) == TRUE)
		{
			while (Process32NextW(hProcessSnap, &pe32) == TRUE)
			{
				if (lstrcmpiW(pe32.szExeFile, lpProcName) == 0)
				{
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
					break;
				}
			}
		}
	}
	CloseHandle(hProcessSnap);
	return hProcess;
}

#include <psapi.h>
#pragma comment(lib, "psapi")
__inline static
int GetProcessFilePathByPid(LPSTR lpProcessFilePath, DWORD dwSize, DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess != NULL)
	{
		// Get a list of all the modules in this process.

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			if ((cbNeeded / sizeof(HMODULE)) > 0)
			{
				if (GetModuleFileNameExA(hProcess, *hMods, lpProcessFilePath, dwSize))
				{
					CloseHandle(hProcess);
					return 0;
				}
			}
		}

		// Release the handle to the process.
		CloseHandle(hProcess);
	}
	return (-1);
}
__inline static
int GetProcessFilePathByPid(LPWSTR lpProcessFilePath, DWORD dwSize, DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess != NULL)
	{
		// Get a list of all the modules in this process.

		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			if ((cbNeeded / sizeof(HMODULE)) > 0)
			{
				if (GetModuleFileNameExW(hProcess, *hMods, lpProcessFilePath, dwSize))
				{
					CloseHandle(hProcess);
					return 0;
				}
			}
		}

		// Release the handle to the process.
		CloseHandle(hProcess);
	}
	return (-1);
}
__inline static
BOOL EnablePrivilege(BOOL bEnable = TRUE, LPCTSTR lpPrivName = SE_DEBUG_NAME)
{
	BOOL fOK = FALSE;
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp = { 0 };
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, lpPrivName, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOK = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOK;
}
__inline static
INT EnumProcessList(std::vector<PROCESSENTRY32> & vProcess, DWORD dwPID = 0)
{
	HANDLE hProcess;                        //进程句柄
	BOOL bProcess = FALSE;                  //获取进程信息的函数返回值
	PROCESSENTRY32 pe32 = {0};                    //保存进程信息

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed(%d)\n", GetLastError());
		exit(1);
	}
	pe32.dwSize = sizeof(pe32);
	bProcess = Process32First(hProcess, &pe32);              //获取第一个进程信息
	while (bProcess)                                         //循环获取其余进程信息
	{
		if ((dwPID != 0) && (pe32.th32ProcessID != dwPID))
		{
			bProcess = Process32Next(hProcess, &pe32);           //继续获取其他进程信息
			continue;
		}

		//printf("\tParent's PID(%d)\tPID(%d)\t%s\n", pe32.th32ParentProcessID, pe32.th32ProcessID, pe32.szExeFile);
		vProcess.emplace_back(pe32);

		bProcess = Process32Next(hProcess, &pe32);           //继续获取其他进程信息
	}

	CloseHandle(hProcess);
	return 0;
}
__inline static
INT EnumProcessList(std::vector<PROCESSENTRY32W>& vProcess, DWORD dwPID = 0)
{
	HANDLE hProcess;                        //进程句柄
	BOOL bProcess = FALSE;                  //获取进程信息的函数返回值
	PROCESSENTRY32W pe32 = {0};                    //保存进程信息

	hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot failed(%d)\n", GetLastError());
		exit(1);
	}

	pe32.dwSize = sizeof(pe32);
	bProcess = Process32FirstW(hProcess, &pe32);              //获取第一个进程信息
	while (bProcess)                                         //循环获取其余进程信息
	{
		if ((dwPID != 0) && (pe32.th32ProcessID != dwPID))
		{
			bProcess = Process32NextW(hProcess, &pe32);           //继续获取其他进程信息
			continue;
		}

		//printf("\tParent's PID(%d)\tPID(%d)\t%s\n", pe32.th32ParentProcessID, pe32.th32ProcessID, pe32.szExeFile);
		vProcess.emplace_back(pe32);

		bProcess = Process32NextW(hProcess, &pe32);           //继续获取其他进程信息
		printf("\n\n");
	}

	CloseHandle(hProcess);
	return 0;
}
__inline static
INT EnumModulesList(std::vector<MODULEENTRY32>& vModules, PROCESSENTRY32& pe32)
{
	HANDLE hModule;                         //模块句柄
	BOOL bModule = FALSE;                   //获取模块信息的函数返回值
	MODULEENTRY32  me32 = {0};                    //保存模块信息

	if (0 != pe32.th32ParentProcessID)                   //获取进程PID不为0的模块信息
	{
		hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);  //获取模块快照
		if (hModule != INVALID_HANDLE_VALUE)
		{
			me32.dwSize = sizeof(me32);
			bModule = Module32First(hModule, &me32);      //获取第一个模块信息,即进程相应可执行文件的信息
			while (bModule)
			{
				//printf("\t%s\n", me32.szExePath);
				vModules.emplace_back(me32);
				bModule = Module32Next(hModule, &me32);  //获取其他模块信息
			}
			CloseHandle(hModule);
		}
	}

	return 0;
}
__inline static
INT EnumModulesList(std::vector<MODULEENTRY32W>& vModules, PROCESSENTRY32W& pe32)
{
	HANDLE hModule;                         //模块句柄
	BOOL bModule = FALSE;                   //获取模块信息的函数返回值
	MODULEENTRY32W me32 = {0};                    //保存模块信息

	if (0 != pe32.th32ParentProcessID)                   //获取进程PID不为0的模块信息
	{
		hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);  //获取模块快照
		if (hModule != INVALID_HANDLE_VALUE)
		{
			me32.dwSize = sizeof(me32);
			bModule = Module32FirstW(hModule, &me32);      //获取第一个模块信息,即进程相应可执行文件的信息
			while (bModule)
			{
				//printf("\t%s\n", me32.szExePath);
				vModules.emplace_back(me32);
				bModule = Module32NextW(hModule, &me32);  //获取其他模块信息
			}
			CloseHandle(hModule);
		}
	}

	return 0;
}
__inline static
BOOL InjectDll(DWORD dwPID, LPCSTR szDllPath)
{
	HANDLE hThread = NULL;
	HANDLE hProcess = NULL;
	HMODULE hModule = NULL;
	LPVOID lpVirtualMemory = NULL;

	SIZE_T stBufSize = (SIZE_T)((lstrlenA(szDllPath) + 1) * sizeof(CHAR));
	LPTHREAD_START_ROUTINE lpLoadLibrary = NULL;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		printf(("OpenProcess(%d) failed!!! [%d]\n"), dwPID, GetLastError());
		return FALSE;
	}

	lpVirtualMemory = VirtualAllocEx(hProcess, NULL, stBufSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpVirtualMemory == NULL)
	{
		printf(("VirtualAllocEx failed!!! [%d]\n"), GetLastError());
		CloseHandle(hProcess);
		return FALSE;
	}
	WriteProcessMemory(hProcess, lpVirtualMemory, szDllPath, stBufSize, NULL);

	hModule = GetModuleHandleA(("kernel32.dll"));
	if (hModule == NULL)
	{
		printf(("GetModuleHandle(kernel32.dll) failed!!! [%d]\n"), GetLastError());
		VirtualFreeEx(hProcess, lpVirtualMemory, stBufSize, MEM_RELEASE | MEM_FREE);
		CloseHandle(hProcess);
		return FALSE;
	}
	lpLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");

	hThread = CreateRemoteThread(hProcess, NULL, 0, lpLoadLibrary, lpVirtualMemory, 0, NULL);
	if (hThread == NULL)
	{
		printf(("GetModuleHandle(kernel32.dll) failed!!! [%d]\n"), GetLastError());
		VirtualFreeEx(hProcess, lpVirtualMemory, stBufSize, MEM_RELEASE | MEM_FREE);
		CloseHandle(hProcess);
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
__inline static
BOOL InjectDll(DWORD dwPID, LPCWSTR szDllPath)
{
	HANDLE hThread = NULL;
	HANDLE hProcess = NULL;
	HMODULE hModule = NULL;
	LPVOID lpVirtualMemory = NULL;

	SIZE_T stBufSize = (SIZE_T)((lstrlenW(szDllPath) + 1) * sizeof(WCHAR));
	LPTHREAD_START_ROUTINE lpLoadLibrary = NULL;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		wprintf((L"OpenProcess(%d) failed!!! [%d]\n"), dwPID, GetLastError());
		return FALSE;
	}

	lpVirtualMemory = VirtualAllocEx(hProcess, NULL, stBufSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpVirtualMemory == NULL)
	{
		wprintf((L"VirtualAllocEx failed!!! [%d]\n"), GetLastError());
		CloseHandle(hProcess);
		return FALSE;
	}
	WriteProcessMemory(hProcess, lpVirtualMemory, szDllPath, stBufSize, NULL);

	hModule = GetModuleHandleW((L"kernel32.dll"));
	if (hModule == NULL)
	{
		wprintf((L"GetModuleHandle(kernel32.dll) failed!!! [%d]\n"), GetLastError());
		VirtualFreeEx(hProcess, lpVirtualMemory, stBufSize, MEM_RELEASE | MEM_FREE);
		CloseHandle(hProcess);
		return FALSE;
	}
	lpLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryW");

	hThread = CreateRemoteThread(hProcess, NULL, 0, lpLoadLibrary, lpVirtualMemory, 0, NULL);
	if (hThread == NULL)
	{
		wprintf((L"GetModuleHandle(kernel32.dll) failed!!! [%d]\n"), GetLastError());
		VirtualFreeEx(hProcess, lpVirtualMemory, stBufSize, MEM_RELEASE | MEM_FREE);
		CloseHandle(hProcess);
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}

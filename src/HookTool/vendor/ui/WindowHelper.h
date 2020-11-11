// WindowHelper.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <olectl.h>
#pragma comment(lib,"oleaut32")
#include <windows.h>
#include <unordered_map>

class WindowHelper {
public:
	INT CreatePpsdlg(LPVOID lpWndProc = NULL, HWND hParent = NULL, BOOL bModal = TRUE, BOOL bCenter = TRUE)
	{
		INT nRet = (-1);
		HWND hWnd = NULL;
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

		bModal &= (hParent != NULL);

		GetClassInfoEx(GetModuleHandle(NULL), WC_DIALOG, &wcex);
		wcex.lpszClassName = TEXT(__func__);
		wcex.lpfnWndProc = ([](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				WNDPROC fnWndProc = NULL;
				switch (uMsg)
				{
				case WM_NCCREATE:
				{
					if ((lParam != NULL) && (fnWndProc = *(WNDPROC*)lParam) != NULL)
					{
						SetProp(hWnd, TEXT(__func__), (HANDLE)(fnWndProc));
					}
				}
				break;
				case WM_NCDESTROY:
				{
					PostMessage(hWnd, WM_QUIT, (WPARAM)(NULL), (LPARAM)(NULL));
				}
				break;
				default:
				{
					switch (uMsg)
					{
					case WM_CREATE:
						EnableWindow(GetParent(hWnd), FALSE);
						break;
					case WM_CLOSE:
						EnableWindow(GetParent(hWnd), TRUE);
						break;
					default:
						break;
					}
					if ((fnWndProc = (WNDPROC)GetProp(hWnd, TEXT(__func__))) != NULL)
					{
						fnWndProc(hWnd, uMsg, wParam, lParam);
					}
				}
				break;
				}
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			});
		RegisterClassEx(&wcex);

		/////////////////////////////////////////////////
		if ((hWnd = CreateWindowEx(
			WS_EX_DLGMODALFRAME,
			wcex.lpszClassName,
			wcex.lpszClassName,
			bModal ? WS_POPUPWINDOW | WS_CAPTION : WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			hParent,
			NULL,
			wcex.hInstance,
			lpWndProc)
			) != NULL) {
			if (bCenter == TRUE)
			{
				if (hParent != NULL)
				{
					CenterWindowInParent(hWnd, hParent);
				}
				else
				{
					CenterWindowInScreen(hWnd);
				}
			}
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			while (hWnd != NULL)
			{
				MSG msg = { 0 };
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						break;
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else
				{
					// 完成某些工作的其他行程式
					Sleep(1);
				}
			}
			nRet = (0);
		}

		return nRet;
	}

public:
	int ROW_NUM = 30;
	int COL_NUM = 6;
	int SPACE_X = 10;
	int SPACE_Y = 10;
public:
	class cc_item {
	public:
		typedef enum ScaleType {
			ST_MIN = 0,
			ST_FIXED,
			ST_X_SCALE,
			ST_Y_SCALE,
			ST_XY_SCALE,
			ST_MAX,
		}ScaleType;
	public:
		cc_item(ScaleType sType, int w, int h, double xScale, double yScale, WNDPROC fnWndProc) :
			sType(sType), w(w), h(h), xScale(xScale), yScale(yScale), fnWndProc(fnWndProc)
		{
		}
	public:
		ScaleType sType;
		int w;
		int h;
		double xScale;//宽度占用比例
		double yScale;//高度占用比例
		int xExclude;//当前行的所有固定项宽度
		int yExclude;//当前列的所有固定项高度
		int xN;//有效行数
		int yN;//有效列数
		WNDPROC fnWndProc;
	};
public:
	//Controls array
	std::unordered_map<HWND, std::vector<std::vector<uint32_t>>> cc_array;
	//Controls layout info array
	std::unordered_map<uint32_t, cc_item> cc_umap;
public:
	void InitGridLayout(HWND hWnd)
	{
		if (this->cc_array.find(hWnd) != this->cc_array.end())
		{
			this->cc_array.at(hWnd).clear();
		}
		else
		{
			this->cc_array.emplace(hWnd, std::vector<std::vector<uint32_t>>{});
		}
		for (auto row = 0; row < this->ROW_NUM; row++)
		{
			this->cc_array.at(hWnd).push_back({});
			for (auto col = 0; col < this->COL_NUM; col++)
			{
				this->cc_array.at(hWnd)[row].push_back(0);
			}
		}
	}
	void InitGridLayout(HWND hWnd, int nRowNum, int nColNum)
	{
		if (this->cc_array.find(hWnd) != this->cc_array.end())
		{
			this->cc_array.at(hWnd).clear();
		}
		else
		{
			this->cc_array.emplace(hWnd, std::vector<std::vector<uint32_t>>{});
		}
		for (auto row = 0; row < nRowNum; row++)
		{
			this->cc_array.at(hWnd).push_back({});
			for (auto col = 0; col < nColNum; col++)
			{
				this->cc_array.at(hWnd)[row].push_back(0);
			}
		}
	}
	void AddControlToLayout(HWND hWnd, uint32_t row, uint32_t col, uint32_t uCtrlID,
		WNDPROC fnWndProc = NULL,
		cc_item::ScaleType scaleType = cc_item::ST_FIXED,
		int W = 0, int H = 0,
		double dScaleX = 0.0f, double dScaleY = 0.0f)
	{
		this->cc_umap.emplace(uCtrlID, cc_item(scaleType, W, H, dScaleX, dScaleY, fnWndProc));
		this->cc_array.at(hWnd)[row][col] = uCtrlID;
	}
	void CreateControl(uint32_t uCtrlID, LPCTSTR lpClassName, LPCTSTR lpWindowName,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE,
		DWORD dwExStyle = 0L,
		HWND hParent = NULL,
		int W = 0, int H = 0,
		int X = 0, int Y = 0,
		HINSTANCE hInstance = NULL,
		LPVOID lpParam = NULL)
	{
		SendMessage(CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, W, H, hParent, (HMENU)uCtrlID, hInstance, lpParam), WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
	}
	void LayoutOptimize(HWND hWnd)
	{
		//行运算
		auto rows = this->cc_array.at(hWnd);
		auto cols = rows.at(0);
		int xN = 0;
		int yN = 0;
		for (auto row = 0; row < rows.size(); row++)
		{
			bool bFound = false;
			int xExclude = 0;
			for (auto col = 0; col < cols.size(); col++)
			{
				auto item = rows.at(row).at(col);
				if (this->cc_umap.find(item) != this->cc_umap.end())
				{
					bFound = true;
					if (this->cc_umap.at(item).sType == cc_item::ST_FIXED || this->cc_umap.at(item).sType == cc_item::ST_Y_SCALE)
					{
						xExclude += this->cc_umap.at(item).w;
					}
				}
			}
			if (bFound == true)
			{
				yN++;
			}
			//修正
			for (auto col = 0; col < cols.size(); col++)
			{
				auto item = rows.at(row).at(col);
				if (this->cc_umap.find(item) != this->cc_umap.end())
				{
					if (this->cc_umap.at(item).sType != cc_item::ST_FIXED)
					{
						this->cc_umap.at(item).xExclude = xExclude;
					}
				}
			}
		}
		//列运算
		for (auto col = 0; col < cols.size(); col++)
		{
			bool bFound = false;
			int yExclude = 0;
			for (auto row = 0; row < rows.size(); row++)
			{
				auto item = rows.at(row).at(col);
				if (this->cc_umap.find(item) != this->cc_umap.end())
				{
					bFound = true;
					if (this->cc_umap.at(item).sType == cc_item::ST_FIXED || this->cc_umap.at(item).sType == cc_item::ST_X_SCALE)
					{
						yExclude += this->cc_umap.at(item).h;
					}
				}
			}
			if (bFound == true)
			{
				xN++;
			}
			//修正
			for (auto row = 0; row < rows.size(); row++)
			{
				auto item = rows.at(row).at(col);
				if (this->cc_umap.find(item) != this->cc_umap.end())
				{
					bFound = true;
					if (this->cc_umap.at(item).sType != cc_item::ST_FIXED)
					{
						this->cc_umap.at(item).yExclude = yExclude;
					}
				}
			}
		}
		//优化数值
		for (auto col = 0; col < cols.size(); col++)
		{
			for (auto row = 0; row < rows.size(); row++)
			{
				auto item = rows.at(row).at(col);
				if (this->cc_umap.find(item) != this->cc_umap.end())
				{
					this->cc_umap.at(item).xN = xN;
					this->cc_umap.at(item).yN = yN;
				}
			}
		}
	}
	void CommandHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (this->cc_umap.find(LOWORD(wParam)) != this->cc_umap.end() && (this->cc_umap.at(LOWORD(wParam)).fnWndProc != NULL)) {
			this->cc_umap.at(LOWORD(wParam)).fnWndProc(hWnd, uMsg, wParam, lParam);
		}
	}
	void RepaintButton(HWND hWnd, LPARAM lParam)
	{
		RECT rect;
		LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;
		UINT itemState = lpDrawItemStruct->itemState;
		TCHAR tClassName[MAXCHAR] = { 0 };
		GetClassName(lpDrawItemStruct->hwndItem, tClassName, sizeof(tClassName) / sizeof(*tClassName));
		if ((lstrcmp(tClassName, WC_BUTTON) == 0) || lpDrawItemStruct->CtlID == IDOK || lpDrawItemStruct->CtlID == IDCANCEL)
		{
			TCHAR tText[MAX_PATH] = { 0 };
			GetWindowText(lpDrawItemStruct->hwndItem, tText, sizeof(tText) / sizeof(*tText));
			GetClientRect(lpDrawItemStruct->hwndItem, &rect);

			UINT edge_type = EDGE_SUNKEN;
			COLORREF bk_color = RGB(0, 128, 0);
			COLORREF text_color = RGB(255, 0, 255);
			COLORREF lt_color = RGB(255, 0, 0);
			COLORREF rb_color = RGB(255, 0, 255);

			if ((itemState & ODS_SELECTED) == ODS_SELECTED)
			{
				edge_type = EDGE_SUNKEN;
				bk_color = RGB(86, 90, 207);
				text_color = RGB(255, 255, 255);
				lt_color = RGB(10, 0, 0);
				rb_color = RGB(10, 0, 10);
			}
			else if ((itemState & ODS_FOCUS) == ODS_FOCUS)
			{
				edge_type = EDGE_RAISED;
				bk_color = RGB(86, 90, 207);
				text_color = RGB(255, 255, 255);
				lt_color = RGB(10, 0, 0);
				rb_color = RGB(10, 0, 10);
			}
			else
			{
				edge_type = EDGE_RAISED;
				bk_color = RGB(86, 90, 207);
				text_color = RGB(255, 255, 255);
				lt_color = RGB(10, 0, 0);
				rb_color = RGB(10, 0, 10);
			}
			SetBkColor(lpDrawItemStruct->hDC, bk_color);
			SetTextColor(lpDrawItemStruct->hDC, text_color);
			FillSolidRect(lpDrawItemStruct->hDC, &rect, bk_color);
			Draw3dRect(lpDrawItemStruct->hDC, &rect, lt_color, rb_color, 3);
			DrawEdge(lpDrawItemStruct->hDC, &rect, edge_type, BF_RECT);
			DrawText(lpDrawItemStruct->hDC, tText, lstrlen(tText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Redraw the Caption of Button Window 
		}
	}
	void RelayoutControls(HWND hWnd)
	{
		if (this->cc_array.find(hWnd) != this->cc_array.end())
		{
			RECT rcWnd = { 0 };
			GetClientRect(hWnd, &rcWnd);

			int left = 0;
			int top = 0;
			auto rows = this->cc_array.at(hWnd);
			for (auto row = 0; row < rows.size(); row++)
			{
				left = 0;
				uint32_t uItem = 0;
				auto cols = rows.at(row);
				for (auto col = 0; col < cols.size(); col++)
				{
					auto item = cols.at(col);
					//存在控件
					if (item > 0)
					{
						RECT rcItem = { 0 };
						cc_item* p_cc_item = &this->cc_umap.at(item);
						if (p_cc_item->sType > cc_item::ST_MIN && p_cc_item->sType < cc_item::ST_MAX)
						{
							switch (p_cc_item->sType)
							{
							case cc_item::ST_FIXED:
								break;
							case cc_item::ST_X_SCALE:
								p_cc_item->w = (rcWnd.right - rcWnd.left - p_cc_item->xExclude) * p_cc_item->xScale;
								break;
							case cc_item::ST_Y_SCALE:
								p_cc_item->h = (rcWnd.bottom - rcWnd.top - p_cc_item->yExclude) * p_cc_item->yScale;
								break;
							case cc_item::ST_XY_SCALE:
								p_cc_item->w = (rcWnd.right - rcWnd.left - p_cc_item->xExclude) * p_cc_item->xScale;
								p_cc_item->h = (rcWnd.bottom - rcWnd.top - p_cc_item->yExclude) * p_cc_item->yScale;
								break;
							default:
								break;
							}
						}
						rcItem.left = left + this->SPACE_X;
						rcItem.top = top + this->SPACE_Y;
						rcItem.right = p_cc_item->w - this->SPACE_X;
						if (((rcItem.left + rcItem.right - rcWnd.right) > this->SPACE_X)
							|| ((rcWnd.right - rcItem.left - rcItem.right) < this->SPACE_X))
						{
							rcItem.right = rcWnd.right - this->SPACE_X - rcItem.left;
						}
						rcItem.bottom = p_cc_item->h - this->SPACE_Y;
						if (((rcItem.top + rcItem.bottom - rcWnd.bottom) > this->SPACE_Y)
							|| ((rcWnd.bottom - rcItem.top - rcItem.bottom) < this->SPACE_Y))
						{
							rcItem.bottom = rcWnd.bottom - this->SPACE_Y - rcItem.top;
						}
						MoveWindow(GetDlgItem(hWnd, item), rcItem.left, rcItem.top, rcItem.right, rcItem.bottom, TRUE);
						left += p_cc_item->w;
						uItem = p_cc_item->h;
					}
				}
				top += uItem;
			}
		}
	}
	std::string AnsiReadFile(const std::string& fileName)
	{
		auto s = FILE_READER(fileName, std::ios::binary);
		switch (DetectEncode((const uint8_t*)s.data(), s.size()))
		{
		case ANSI:
			break;
		case UTF16_LE:
			s.erase(s.begin());
			s.erase(s.begin());
			s = StringConvertUtils::Instance()->WToA(std::wstring((const wchar_t*)s.data(), s.length() / sizeof(wchar_t)));
			break;
		case UTF16_BE:
			s.erase(s.begin());
			s.erase(s.begin());
			s = StringConvertUtils::Instance()->WToA(std::wstring((const wchar_t*)s.data(), s.length() / sizeof(wchar_t)));
			break;
		case UTF8_BOM:
			s.erase(s.begin());
			s.erase(s.begin());
			s.erase(s.begin());
			s = StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(s));
			break;
		case UTF8:
			s = StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(s));
			break;
		default:
			break;
		}
		return (s);
	}
	BOOL ExtractsResource(DWORD dwResId, LPCSTR lpResType, LPCSTR lpFileName)
	{
		DWORD dwNumberOfBytesWritten = 0;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		HMODULE hModule = ::GetModuleHandleA(NULL);
		HRSRC hRsrc = ::FindResourceA(hModule, MAKEINTRESOURCEA(dwResId), lpResType);
		HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
		LPVOID lpData = ::LockResource(hGlobal);
		DWORD dwDataSize = ::SizeofResource(hModule, hRsrc);
		if (hGlobal != NULL && lpData != NULL)
		{
			hFile = CreateFileA(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, lpData, dwDataSize, &dwNumberOfBytesWritten, NULL);
				CloseHandle(hFile);
			}
			FreeResource(hGlobal);
		}
		return ((dwDataSize > 0) && (dwDataSize == dwNumberOfBytesWritten));
	}
	BOOL ExtractsResource(DWORD dwResId, LPCWSTR lpResType, LPCWSTR lpFileName)
	{
		DWORD dwNumberOfBytesWritten = 0;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		HMODULE hModule = ::GetModuleHandleW(NULL);
		HRSRC hRsrc = ::FindResourceW(hModule, MAKEINTRESOURCEW(dwResId), lpResType);
		HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
		LPVOID lpData = ::LockResource(hGlobal);
		DWORD dwDataSize = ::SizeofResource(hModule, hRsrc);
		if (hGlobal != NULL && lpData != NULL)
		{
			hFile = CreateFileW(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, lpData, dwDataSize, &dwNumberOfBytesWritten, NULL);
				CloseHandle(hFile);
			}
			FreeResource(hGlobal);
		}
		return ((dwDataSize > 0) && (dwDataSize == dwNumberOfBytesWritten));
	}
	BOOL GetResourceData(DWORD dwResId, LPCSTR lpResType, LPSIZE lpSize = NULL, HGLOBAL* hGlobalMemory = NULL, IStream** ppIStream = NULL, IPicture** ppIPicture = NULL)
	{
		BOOL bResult = FALSE;
		IStream* pIStream = NULL;
		IPicture* pIPicture = NULL;
		LPVOID lpStreamData = NULL;
		HGLOBAL hStreamGlobal = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HMODULE hModule = NULL;
		HRSRC hRsrc = NULL;
		HGLOBAL hGlobal = NULL;
		LPVOID lpData = NULL;
		DWORD dwDataSize = 0;

		hModule = ::GetModuleHandleA(NULL);
		hRsrc = ::FindResourceA(hModule, MAKEINTRESOURCEA(dwResId), lpResType);
		hGlobal = ::LoadResource(hModule, hRsrc);
		lpData = ::LockResource(hGlobal);
		dwDataSize = ::SizeofResource(hModule, hRsrc);
		if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
		{
			if (hGlobalMemory != NULL)
			{
				if (*hGlobalMemory == NULL)
				{
					(*hGlobalMemory) = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
				}
				hStreamGlobal = (*hGlobalMemory);
			}
			else
			{
				hStreamGlobal = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
			}
			lpStreamData = GlobalLock(hStreamGlobal);
			if (hStreamGlobal != NULL && lpStreamData != NULL)
			{
				CopyMemory(lpStreamData, lpData, dwDataSize);
				if (SUCCEEDED(CreateStreamOnHGlobal(hStreamGlobal, FALSE, &pIStream)))
				{
					if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
					{
						if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
						{
							if (lpSize != NULL)
							{
								lpSize->cx = hmWidth;
								lpSize->cy = hmHeight;
								bResult = TRUE;
							}
						}
						if (ppIPicture == NULL)
						{
							pIPicture->Release();
						}
						else
						{
							*ppIPicture = pIPicture;
						}
					}
					if (ppIStream == NULL)
					{
						pIStream->Release();
					}
					else
					{
						*ppIStream = pIStream;
					}
					FreeResource(hGlobal);
				}
				if (hGlobalMemory == NULL)
				{
					GlobalUnlock(hStreamGlobal);
					GlobalFree(hStreamGlobal);
				}
			}
		}
		return bResult;
	}
	BOOL GetResourceData(DWORD dwResId, LPCWSTR lpResType, LPSIZE lpSize = NULL, HGLOBAL* hGlobalMemory = NULL, IStream** ppIStream = NULL, IPicture** ppIPicture = NULL)
	{
		BOOL bResult = FALSE;
		IStream* pIStream = NULL;
		IPicture* pIPicture = NULL;
		LPVOID lpStreamData = NULL;
		HGLOBAL hStreamGlobal = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HMODULE hModule = NULL;
		HRSRC hRsrc = NULL;
		HGLOBAL hGlobal = NULL;
		LPVOID lpData = NULL;
		DWORD dwDataSize = 0;

		hModule = ::GetModuleHandleW(NULL);
		hRsrc = ::FindResourceW(hModule, MAKEINTRESOURCEW(dwResId), lpResType);
		hGlobal = ::LoadResource(hModule, hRsrc);
		lpData = ::LockResource(hGlobal);
		dwDataSize = ::SizeofResource(hModule, hRsrc);
		if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
		{
			if (hGlobalMemory != NULL)
			{
				if (*hGlobalMemory == NULL)
				{
					(*hGlobalMemory) = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
				}
				hStreamGlobal = (*hGlobalMemory);
			}
			else
			{
				hStreamGlobal = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
			}
			lpStreamData = GlobalLock(hStreamGlobal);
			if (hStreamGlobal != NULL && lpStreamData != NULL)
			{
				CopyMemory(lpStreamData, lpData, dwDataSize);
				if (SUCCEEDED(CreateStreamOnHGlobal(hStreamGlobal, FALSE, &pIStream)))
				{
					if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
					{
						if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
						{
							if (lpSize != NULL)
							{
								lpSize->cx = hmWidth;
								lpSize->cy = hmHeight;
								bResult = TRUE;
							}
						}
						if (ppIPicture == NULL)
						{
							pIPicture->Release();
						}
						else
						{
							*ppIPicture = pIPicture;
						}
					}
					if (ppIStream == NULL)
					{
						pIStream->Release();
					}
					else
					{
						*ppIStream = pIStream;
					}
					FreeResource(hGlobal);
				}
				if (hGlobalMemory == NULL)
				{
					GlobalUnlock(hStreamGlobal);
					GlobalFree(hStreamGlobal);
				}
			}
		}
		return bResult;
	}
	void DisplayPicture(HDC hDC, LPCSTR szImagePath, LPRECT lpRect = NULL)
	{
		DWORD dwFileSize = 0;
		HGLOBAL hGlobal = NULL;
		IStream* pIStream = NULL;
		LPVOID lpFileData = NULL;
		IPicture* pIPicture = NULL;
		DWORD dwNumberOfBytesRead = 0;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = CreateFileA(szImagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			dwFileSize = GetFileSize(hFile, NULL);
			if (dwFileSize > 0)
			{
				hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
				lpFileData = GlobalLock(hGlobal);
				if (hGlobal != NULL && lpFileData != NULL)
				{
					ReadFile(hFile, lpFileData, dwFileSize, &dwNumberOfBytesRead, NULL);
					if (dwFileSize > 0 && dwFileSize == dwNumberOfBytesRead)
					{
						if (SUCCEEDED(CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream)))
						{
							if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
							{
								if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
								{
									if (lpRect != NULL)
									{
										pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
									}
									else
									{
										pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
									}
								}
								pIPicture->Release();
							}
							pIStream->Release();
						}
					}
					GlobalUnlock(hGlobal);
					GlobalFree(hGlobal);
				}
			}
			CloseHandle(hFile);
		}
	}
	void DisplayPicture(HDC hDC, LPCWSTR szImagePath, LPRECT lpRect = NULL)
	{
		DWORD dwFileSize = 0;
		HGLOBAL hGlobal = NULL;
		IStream* pIStream = NULL;
		LPVOID lpFileData = NULL;
		IPicture* pIPicture = NULL;
		DWORD dwNumberOfBytesRead = 0;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		hFile = CreateFileW(szImagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			dwFileSize = GetFileSize(hFile, NULL);
			if (dwFileSize > 0)
			{
				hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
				lpFileData = GlobalLock(hGlobal);
				if (hGlobal != NULL && lpFileData != NULL)
				{
					ReadFile(hFile, lpFileData, dwFileSize, &dwNumberOfBytesRead, NULL);
					if (dwFileSize > 0 && dwFileSize == dwNumberOfBytesRead)
					{
						if (SUCCEEDED(CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream)))
						{
							if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
							{
								if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
								{
									if (lpRect != NULL)
									{
										pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
									}
									else
									{
										pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
									}
								}
								pIPicture->Release();
							}
							pIStream->Release();
						}
					}
					GlobalUnlock(hGlobal);
					GlobalFree(hGlobal);
				}
			}
			CloseHandle(hFile);
		}
	}
	void DisplayPicture(HDC hDC, DWORD dwResId, LPCSTR lpResType, LPRECT lpRect = NULL, HGLOBAL* hGlobalMemory = NULL)
	{
		IStream* pIStream = NULL;
		IPicture* pIPicture = NULL;
		LPVOID lpStreamData = NULL;
		HGLOBAL hStreamGlobal = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HMODULE hModule = NULL;
		HRSRC hRsrc = NULL;
		HGLOBAL hGlobal = NULL;
		LPVOID lpData = NULL;
		DWORD dwDataSize = 0;
		if (hGlobalMemory != NULL)
		{
			if ((*hGlobalMemory) == NULL)
			{
				hModule = ::GetModuleHandleA(NULL);
				hRsrc = ::FindResourceA(hModule, MAKEINTRESOURCEA(dwResId), lpResType);
				hGlobal = ::LoadResource(hModule, hRsrc);
				lpData = ::LockResource(hGlobal);
				dwDataSize = ::SizeofResource(hModule, hRsrc);
				if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
				{
					(*hGlobalMemory) = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
					hStreamGlobal = (*hGlobalMemory);
					lpStreamData = GlobalLock(hStreamGlobal);
					if (hStreamGlobal != NULL && lpStreamData != NULL)
					{
						CopyMemory(lpStreamData, lpData, dwDataSize);
					}
				}
			}
			else
			{
				hStreamGlobal = (*hGlobalMemory);
			}
		}
		else
		{
			hModule = ::GetModuleHandleA(NULL);
			hRsrc = ::FindResourceA(hModule, MAKEINTRESOURCEA(dwResId), lpResType);
			hGlobal = ::LoadResource(hModule, hRsrc);
			lpData = ::LockResource(hGlobal);
			dwDataSize = ::SizeofResource(hModule, hRsrc);
			if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
			{
				hStreamGlobal = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
				lpStreamData = GlobalLock(hStreamGlobal);
				if (hStreamGlobal != NULL && lpStreamData != NULL)
				{
					CopyMemory(lpStreamData, lpData, dwDataSize);
				}
			}
		}
		if (hStreamGlobal != NULL)
		{
			if (SUCCEEDED(CreateStreamOnHGlobal(hStreamGlobal, FALSE, &pIStream)))
			{
				if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
				{
					if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
					{
						if (lpRect != NULL)
						{
							pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
						}
						else
						{
							pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
						}
					}
					pIPicture->Release();
				}
				pIStream->Release();
				FreeResource(hGlobal);
			}
			if (hGlobalMemory == NULL)
			{
				GlobalUnlock(hStreamGlobal);
				GlobalFree(hStreamGlobal);
			}
		}
	}
	void DisplayPicture(HDC hDC, DWORD dwResId, LPCWSTR lpResType, LPRECT lpRect = NULL, HGLOBAL* hGlobalMemory = NULL)
	{
		IStream* pIStream = NULL;
		IPicture* pIPicture = NULL;
		LPVOID lpStreamData = NULL;
		HGLOBAL hStreamGlobal = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		HMODULE hModule = NULL;
		HRSRC hRsrc = NULL;
		HGLOBAL hGlobal = NULL;
		LPVOID lpData = NULL;
		DWORD dwDataSize = 0;
		if (hGlobalMemory != NULL)
		{
			if ((*hGlobalMemory) == NULL)
			{
				hModule = ::GetModuleHandleW(NULL);
				hRsrc = ::FindResourceW(hModule, MAKEINTRESOURCEW(dwResId), lpResType);
				hGlobal = ::LoadResource(hModule, hRsrc);
				lpData = ::LockResource(hGlobal);
				dwDataSize = ::SizeofResource(hModule, hRsrc);
				if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
				{
					(*hGlobalMemory) = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
					hStreamGlobal = (*hGlobalMemory);
					lpStreamData = GlobalLock(hStreamGlobal);
					if (hStreamGlobal != NULL && lpStreamData != NULL)
					{
						CopyMemory(lpStreamData, lpData, dwDataSize);
					}
				}
			}
			else
			{
				hStreamGlobal = (*hGlobalMemory);
			}
		}
		else
		{
			hModule = ::GetModuleHandleA(NULL);
			hRsrc = ::FindResourceW(hModule, MAKEINTRESOURCEW(dwResId), lpResType);
			hGlobal = ::LoadResource(hModule, hRsrc);
			lpData = ::LockResource(hGlobal);
			dwDataSize = ::SizeofResource(hModule, hRsrc);
			if (hGlobal != NULL && lpData != NULL && dwDataSize > 0)
			{
				hStreamGlobal = GlobalAlloc(GMEM_MOVEABLE, dwDataSize);
				lpStreamData = GlobalLock(hStreamGlobal);
				if (hStreamGlobal != NULL && lpStreamData != NULL)
				{
					CopyMemory(lpStreamData, lpData, dwDataSize);
				}
			}
		}
		if (hStreamGlobal != NULL)
		{
			if (SUCCEEDED(CreateStreamOnHGlobal(hStreamGlobal, FALSE, &pIStream)))
			{
				if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
				{
					if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
					{
						if (lpRect != NULL)
						{
							pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
						}
						else
						{
							pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
						}
					}
					pIPicture->Release();
				}
				pIStream->Release();
				FreeResource(hGlobal);
			}
			if (hGlobalMemory == NULL)
			{
				GlobalUnlock(hStreamGlobal);
				GlobalFree(hStreamGlobal);
			}
		}
	}

	void DisplayPictureWithGlobal(HDC hDC, HGLOBAL hStreamGlobal, LPRECT lpRect = NULL)
	{
		IStream* pIStream = NULL;
		IPicture* pIPicture = NULL;
		LPVOID lpStreamData = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		if (hStreamGlobal != NULL)
		{
			lpStreamData = GlobalLock(hStreamGlobal);
			if (lpStreamData != NULL)
			{
				if (SUCCEEDED(CreateStreamOnHGlobal(hStreamGlobal, FALSE, &pIStream)))
				{
					if (SUCCEEDED(OleLoadPicture(pIStream, 0, FALSE, IID_IPicture, (LPVOID*)&(pIPicture))))
					{
						if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
						{
							if (lpRect != NULL)
							{
								pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
							}
							else
							{
								pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
							}
						}
						pIPicture->Release();
					}
					pIStream->Release();
				}
			}
		}
	}
	void DisplayPictureWithIPicture(HDC hDC, IPicture* pIPicture = NULL, LPRECT lpRect = NULL)
	{
		LPVOID lpStreamData = NULL;
		OLE_XSIZE_HIMETRIC hmWidth = 0;
		OLE_YSIZE_HIMETRIC hmHeight = 0;
		if (pIPicture != NULL)
		{
			if (SUCCEEDED(pIPicture->get_Width(&hmWidth)) && SUCCEEDED(pIPicture->get_Height(&hmHeight)))
			{
				if (lpRect != NULL)
				{
					pIPicture->Render(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL);//在指定的DC上绘出图片
				}
				else
				{
					pIPicture->Render(hDC, 0, 0, hmWidth, hmHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
				}
			}
		}
	}
public:
	static WindowHelper* Inst() {
		static WindowHelper windowHelperInstance;
		return &windowHelperInstance;
	}
};
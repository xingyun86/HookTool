// HookTool.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <operator.h>
#include <interface.h>
#include <securityprotects.h>
#include <converter.h>
#include <randnonce.h>
#include <windowapi.h>
#include <windowfwp.h>
#include <Authority.h>
#include <nifnotify.h>
#include <dbgoutput.h>

#include <tlhelp32.h>
#include <ppsyqm/json.hpp>

#include <WindowHelper.h>
#include <clog.h>
#include <sstream>

__CLOG_GLOBAL_MACROS_DEFINE__

__EXT_APP_DIR__

__inline static
BOOL move_file(const std::string& dst, const std::string& src)
{
	return ExecuteCommand(("cmd /c MOVE /Y \"" + src + "\" \"" + dst + "\"").c_str());
}
__inline static
BOOL clean_path(const std::string& dir)
{
	return ExecuteCommand(("cmd /c DEL /S /Q \"" + dir + "\"").c_str());
}
__inline static
BOOL copy_file(const std::string& dst, const std::string& src)
{
	return ExecuteCommand(("cmd /c COPY /Y \"" + src + "\" \"" + dst + "\"").c_str());
}
__inline static
BOOL xcopy_files(const std::string& dst, const std::string& src)
{
	return ExecuteCommand(("cmd /c XCOPY \"" + src + "\" \"" + dst + "\" /S /E /C /Q /D /Y").c_str());
}
class HookToolWindowHelper : public WindowHelper {
	typedef enum {
		IDCC_MINIMUM = WM_USER + WM_APPCOMMAND,
		IDCC_STATIC_MESSAGE,
		IDCC_BUTTON_START,
		IDCC_BUTTON_CLOSE,
		IDCC_LISTCTRL_PROCESS,
		IDCC_LISTCTRL_MODULES,
		IDCC_MAXIMUM,
		WM_NOTIFY_BUTTON_STATUS,
	} ChildrenControls;
public:
	bool bStartState = false;
	ppsyqm::json m_config = nullptr;
	boolean m_thread_status = true;
	StreamGlobal m_stream_global = {};
	Gdiplus::Bitmap * m_p_bitmap_memory = nullptr;
	std::shared_ptr<std::thread> m_thread = nullptr;
	std::vector<PROCESSENTRY32> m_vProcess = {};
	std::vector<MODULEENTRY32> m_vModules = {};
	INT ListProcess(DWORD dwPID = 0)
	{
		return EnumProcessList(m_vProcess, dwPID);
	}
	INT ListModules(PROCESSENTRY32 &pe32)
	{
		return EnumModulesList(m_vModules, pe32);
	}
	void start_thread(HWND hWnd)
	{
		LoadData(m_data_path);
		m_thread = std::make_shared<std::thread>([hWnd, this]()
			{
				HookToolWindowHelper* thiz = HookToolWindowHelper::Inst();
				/*std::string commandline("");
				thiz->m_thread_status = true;
				while (thiz->m_thread_status)
				{
					std::string code_text = "";
					std::string code_path = "";
					std::string code_value = "";
					thiz->m_task_locker.lock();
					if (!thiz->m_task_queue.empty())
					{
						code_text = thiz->m_task_queue.front();
						thiz->m_task_queue.pop_front();
					}
					thiz->m_task_locker.unlock();
					if (!code_text.empty())
					{
						LOG(main, LOG_INFO, "Scan code=%s.\n", code_text.c_str());
						auto itFind = thiz->m_code_text_map.find(code_text);
						if (itFind != thiz->m_code_text_map.end())
						{
							code_value = itFind->second.at(0);
							code_path = itFind->second.at(1) + "\\";
							LOG(main, LOG_INFO, "Scan value=%s.\n", code_value.c_str());
							auto locale_file_olds = thiz->m_locale_path + "*";
							auto locale_file = thiz->m_locale_path + code_value;
							auto remote_file = thiz->m_remote_path + code_path + code_value;
							clean_path(locale_file_olds);
							copy_file(locale_file, remote_file);
							LOG(main, LOG_INFO, "Copy %s to %s.\n", remote_file.c_str(), locale_file.c_str());
						}
						else
						{
							LOG(main, LOG_INFO, "Scan value not Found.\n");
						}
					}
				}*/
			});
	}
	void stop_thread(HWND hWnd) {
		m_thread_status = false;
		if (m_thread != nullptr && m_thread->joinable())
		{
			m_thread->join();
		}
		SaveData(m_data_path);
	}
	static LRESULT CALLBACK HandleButtonProxyProcessWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowHelper::Inst()->CreatePpsdlg((LPVOID)(WNDPROC)(
			[](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				WindowHelper* thiz = WindowHelper::Inst();

				switch (uMsg)
				{
				case WM_CREATE:
				{
					HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(128));
					SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
					SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
					SendMessage(hWnd, WM_SETICON, ICON_SMALL2, (LPARAM)hIcon);

					//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_MAXIMIZEBOX) & (~WS_THICKFRAME));
					DisplayRequestRepaint(hWnd, 300, 300, TRUE);

					SetWindowText(hWnd, TEXT("软件列表"));

					RegisterDropFilesEvent(hWnd);

					thiz->CreateControl(IDCC_BUTTON_START, WC_BUTTON, TEXT("确定"), BS_PUSHBUTTON | BS_OWNERDRAW | WS_BORDER | WS_CHILD | WS_VISIBLE, 0L, hWnd);
					thiz->CreateControl(IDCC_BUTTON_CLOSE, WC_BUTTON, TEXT("取消"), BS_PUSHBUTTON | BS_OWNERDRAW | WS_BORDER | WS_CHILD | WS_VISIBLE, 0L, hWnd);

					thiz->InitGridLayout(hWnd);

					thiz->AddControlToLayout(hWnd, 0, 0, IDCC_BUTTON_START, NULL, WindowHelper::cc_item::ST_FIXED, 80, 28, 0.0, 0.0);
					thiz->AddControlToLayout(hWnd, 1, 0, IDCC_BUTTON_CLOSE, NULL, WindowHelper::cc_item::ST_XY_SCALE, 80, 28, 1.0, 1.0);

					thiz->LayoutOptimize(hWnd);
				}
				return 0;
				case WM_COMMAND:
				{
					thiz->CommandHandler(hWnd, uMsg, wParam, lParam);
				}
				break;
				case WM_DROPFILES:
				{
					std::vector<std::string> sv;
					GetDropFiles(&sv, (HDROP)(wParam));
					if (!sv.empty()) {
						auto s = FILE_READER(sv.begin()->c_str(), std::ios::binary);
					}
				}
				break;
				case WM_DRAWITEM:
				{
					thiz->RepaintButton(hWnd, lParam);
					return 0;
				}
				break;
				case WM_SIZE:
				{
					thiz->RelayoutControls(hWnd);
				}
				break;
				case WM_PAINT:
				{
					HDC hDC = nullptr;
					PAINTSTRUCT ps = { 0 };
					hDC = BeginPaint(hWnd, &ps);
					if (hDC != nullptr)
					{
						RECT rc = { 0 };
						GetClientRect(hWnd, &rc);
						FillRect(hDC, &rc, (HBRUSH)HOLLOW_BRUSH);
						//TextOut(hDC, 0, 0, TEXT("Hello world!"), lstrlen(TEXT("Hello world!")));
						//TextOut(hDC, 32, 32, TEXT("Win32 Api!"), lstrlen(TEXT("Win32 Api!")));
						EndPaint(hWnd, &ps);
					}
				}
				break;
				case WM_CLOSE:
				{
				}
				break;
				}
				return 0;
			}), hWnd);
		return 0;
	}
	static void NotifyStatus(HWND hWnd, BOOL bState)
	{
		PostMessage(hWnd, WM_NOTIFY_BUTTON_STATUS, MAKEWPARAM(bState, 0), (LPARAM)NULL);
	}
	static std::string OutputFolder()
	{
		return (std::string(APP_DIR) + "\\out\\");
	}
	static std::string GetConfigPrefix()
	{
		return ("HookTool_main");
	}
	static LRESULT CALLBACK HandleButtonStartWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HookToolWindowHelper* thiz = HookToolWindowHelper::Inst();
		thiz->m_vProcess.clear();
		thiz->ListProcess();
		thiz->ShowProcessList(GetDlgItem(hWnd, IDCC_LISTCTRL_PROCESS));
		
		if (thiz->bStartState == false)
		{
			//((void (*)(HWND, BOOL))GetProp(hWnd, TEXT("NOTIFY_STATUS")))(hWnd, TRUE);
			//AutoBingUtil::Inst()->start_ScanSaveTool_service((void*)hWnd, HookToolWindowHelper::Inst()->GetConfigPrefix());
		}
		else
		{
			//AutoBingUtil::Inst()->stop_service(hWnd);
			//((void (*)(HWND, BOOL))GetProp(hWnd, TEXT("NOTIFY_STATUS")))(hWnd, FALSE);
		}

		return 0;
	}
	static LRESULT CALLBACK HandleButtonCloseWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SendMessage(hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		return 0;
	}
	static LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BOOL bHandled = FALSE;
		HookToolWindowHelper* thiz = HookToolWindowHelper::Inst();

		NotifyTray::Inst()->HandleEvent(hWnd, uMsg, wParam, lParam, bHandled);

		switch (uMsg)
		{
		case WM_CREATE:
		{
			HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(128));
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL2, (LPARAM)hIcon);

			//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & (~WS_MAXIMIZEBOX) & (~WS_THICKFRAME));
			DisplayRequestRepaint(hWnd, 750, 650, TRUE);

			SetWindowText(hWnd, TEXT("HookTool"));
			NotifyTray::Inst()->init(hWnd, hIcon, TEXT("HookTool"));

			RegisterDropFilesEvent(hWnd);

			SetProp(hWnd, TEXT("NOTIFY_STATUS"), (HANDLE)NotifyStatus);
			SetProp(hWnd, TEXT("OUTPUT_FOLDER"), (HANDLE)OutputFolder);

			thiz->CreateControl(IDCC_STATIC_MESSAGE, WC_STATIC, TEXT("Running status:not running."), SS_LEFT | WS_BORDER | WS_CHILD | WS_VISIBLE, 0L, hWnd);
			
			thiz->CreateControl(IDCC_BUTTON_START, WC_BUTTON, TEXT("Refresh"), BS_PUSHBUTTON | BS_OWNERDRAW | WS_BORDER | WS_CHILD | WS_VISIBLE, 0L, hWnd);
			thiz->CreateControl(IDCC_BUTTON_CLOSE, WC_BUTTON, TEXT("Close"), BS_PUSHBUTTON | BS_OWNERDRAW | WS_BORDER | WS_CHILD | WS_VISIBLE, 0L, hWnd);

			thiz->CreateControl(IDCC_LISTCTRL_PROCESS, WC_LISTVIEW, TEXT(""), LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | WS_CHILD | WS_VISIBLE, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES, hWnd);
			thiz->CreateControl(IDCC_LISTCTRL_MODULES, WC_LISTVIEW, TEXT(""), LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | WS_CHILD | WS_VISIBLE, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES, hWnd);

			thiz->InitGridLayout(hWnd);

			auto row = 1;
			thiz->AddControlToLayout(hWnd, row, 0, IDCC_STATIC_MESSAGE, NULL, WindowHelper::cc_item::ST_X_SCALE, 160, 36, 1.0, 0.0);
			row++;			
			thiz->AddControlToLayout(hWnd, row, 0, IDCC_BUTTON_START, HandleButtonStartWndProc, WindowHelper::cc_item::ST_FIXED, 120, 39, 0.0, 0.0);
			thiz->AddControlToLayout(hWnd, row, 1, IDCC_BUTTON_CLOSE, HandleButtonCloseWndProc, WindowHelper::cc_item::ST_FIXED, 120, 39, 0.0, 0.0);
			row++;
			thiz->AddControlToLayout(hWnd, row, 0, IDCC_LISTCTRL_PROCESS, NULL, WindowHelper::cc_item::ST_XY_SCALE, 120, 39, 1.0, 0.5);
			row++;
			thiz->AddControlToLayout(hWnd, row, 0, IDCC_LISTCTRL_MODULES, NULL, WindowHelper::cc_item::ST_XY_SCALE, 120, 39, 1.0, 0.5);
			row++;

			thiz->LayoutOptimize(hWnd);

			//CreateDirectory(TEXT(OutputFolder().c_str()), NULL);

			thiz->start_thread(hWnd);

			{
				HWND hWndList = GetDlgItem(hWnd, IDCC_LISTCTRL_PROCESS);
				ListView_SetExtendedListViewStyle(hWndList, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
				std::vector<std::string> vColumns = { "PID", "ProgramName", "Path", };
				LV_COLUMN lvc = { 0 };
				for (int n = 0; n < vColumns.size(); n++)
				{
					lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
					lvc.cx = 200;
					lvc.fmt = LVCFMT_LEFT;
					lvc.pszText = (LPSTR)vColumns.at(n).c_str();
					ListView_InsertColumn(hWndList, n, &lvc);
				}
				HookToolWindowHelper::Inst()->ShowProcessList(hWndList);
			}
			{
				HWND hWndList = GetDlgItem(hWnd, IDCC_LISTCTRL_MODULES);
				ListView_SetExtendedListViewStyle(hWndList, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
				std::vector<std::string> vColumns = { "ModuleName", "BaseAddress", "Size", "Path", };
				LV_COLUMN lvc = { 0 };
				for (int n = 0; n < vColumns.size(); n++)
				{
					lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
					lvc.cx = 200;
					lvc.fmt = LVCFMT_LEFT;
					lvc.pszText = (LPSTR)vColumns.at(n).c_str();
					ListView_InsertColumn(hWndList, n, &lvc);
				}
				HookToolWindowHelper::Inst()->ShowModulesList(hWndList);
			}
		}
		return 0;
		case WM_NOTIFY_BUTTON_STATUS:
		{
			if (LOWORD(wParam) == FALSE)
			{
				thiz->bStartState = false;
				SetDlgItemText(hWnd, IDCC_BUTTON_START, TEXT("Start"));
				SetDlgItemText(hWnd, IDCC_STATIC_MESSAGE, TEXT("Running status:not running"));
				//ShellExecute(NULL, TEXT("OPEN"), TEXT("EXPLORER.EXE"), TEXT(OutputFolder().c_str()), NULL, SW_SHOWNORMAL);
				//AppendMenu(GetSystemMenu(hWnd, TRUE), SC_CLOSE, MF_BYCOMMAND, NULL);
			}
			else
			{
				thiz->bStartState = true;
				SetDlgItemText(hWnd, IDCC_BUTTON_START, TEXT("Stop"));
				SetDlgItemText(hWnd, IDCC_STATIC_MESSAGE, TEXT("Running status:running"));
				//RemoveMenu(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND);
			}
		}
		break;
		case WM_COMMAND:
		{
			thiz->CommandHandler(hWnd, uMsg, wParam, lParam);
		}
		break;
		case WM_NOTIFY:
		{
			if (((LPNMHDR)lParam)->idFrom == IDCC_LISTCTRL_PROCESS)
			{
				switch (((LPNMHDR)lParam)->code)
				{
				case NM_CLICK:
				{
					NMLISTVIEW* nmlv = (NMLISTVIEW*)lParam;
					int nselitem = nmlv->iItem;
					int nsubitem = nmlv->iSubItem;
					if (nselitem >= 0 && nsubitem >= 0)
					{
						std::string pid(MAX_PATH, '\0');
						ListView_GetItemText(((LPNMHDR)lParam)->hwndFrom, nselitem, 0, (LPSTR)pid.data(), pid.size());
						PROCESSENTRY32 pe32 = { 0 };
						pe32.dwSize = sizeof(pe32);
						pe32.th32ProcessID = std::stoul(pid.c_str());
						pe32.th32ParentProcessID = pe32.th32ProcessID;
						thiz->m_vModules.clear();
						thiz->ListModules(pe32);
						thiz->ShowModulesList(GetDlgItem(hWnd, IDCC_LISTCTRL_MODULES));
					}
				}
				break;
				case NM_DBLCLK:
				{
					NMLISTVIEW* nmlv = (NMLISTVIEW*)lParam;
					int nselitem = nmlv->iItem;
					int nsubitem = nmlv->iSubItem;
					if (nselitem >= 0 && nsubitem >= 0)
					{
						std::string pid(MAX_PATH, '\0');
						ListView_GetItemText(((LPNMHDR)lParam)->hwndFrom, nselitem, 0, (LPSTR)pid.data(), pid.size());
						PROCESSENTRY32 pe32 = { 0 };
						pe32.dwSize = sizeof(pe32);
						pe32.th32ProcessID = std::stoul(pid.c_str());
						pe32.th32ParentProcessID = pe32.th32ProcessID;
						CHAR szDllPath[MAX_PATH] = { 0 };
						if (SelectOpenFile(szDllPath, ("Dll Files (*.dll)\0*.DLL\0All Files (*.*)\0*.*\0\0"), ("Select Dll File")) &&
							InjectDll(pe32.th32ProcessID, szDllPath))
						{
							thiz->m_vModules.clear();
							thiz->ListModules(pe32);
							thiz->ShowModulesList(GetDlgItem(hWnd, IDCC_LISTCTRL_MODULES));
							MessageBox(hWnd, TEXT("Inject Success!"), TEXT("Messgae"), MB_OK);
						}						
					}
				}
				break;
				}
			}
			break;
		}
		break;
		case WM_DROPFILES:
		{
			std::vector<std::string> sv;
			GetDropFiles(&sv, (HDROP)(wParam));
			if (!sv.empty()) {
			}
		}
		break;
		case WM_DRAWITEM:
		{
			thiz->RepaintButton(hWnd, lParam);
			return 0;
		}
		break;
		case WM_SIZE:
		{
			thiz->RelayoutControls(hWnd);
			thiz->RenderMemory(hWnd);
		}
		break;
		case WM_PAINT:
		{
			HDC hDC = nullptr;
			PAINTSTRUCT ps = { 0 };
			hDC = BeginPaint(hWnd, &ps);
			if (hDC != nullptr)
			{
				thiz->RenderWindow(hWnd, hDC);
				EndPaint(hWnd, &ps);
			}
		}
		break;
		case WM_CLOSE:
		{
			NotifyTray::Inst()->DelNotifyIcon();
			thiz->stop_thread(hWnd);
			CSecurityProtects(2032, 9, 8).CheckValid();
		}
		break;
		}
		return 0;
	}
	void RenderWindow(HWND hWnd, HDC hDC)
	{
		RECT rcWnd = { 0 };
		GetClientRect(hWnd, &rcWnd);
		Gdiplus::Graphics graphicsPhysics(hDC);
		if (m_p_bitmap_memory != nullptr)
		{
			graphicsPhysics.DrawImage(m_p_bitmap_memory, 0, 0, rcWnd.right, rcWnd.bottom);
			delete m_p_bitmap_memory;
			m_p_bitmap_memory = nullptr;
		}
		graphicsPhysics.ReleaseHDC(hDC);
	}
	void RenderMemory(HWND hWnd)
	{
		RECT rcWnd = { 0 };
		GetClientRect(hWnd, &rcWnd);
		if (m_p_bitmap_memory != nullptr)
		{
			if ((m_p_bitmap_memory->GetWidth() != rcWnd.right) || (m_p_bitmap_memory->GetWidth() != rcWnd.bottom))
			{
				delete m_p_bitmap_memory;
				m_p_bitmap_memory = nullptr;
			}
		}
		if (m_p_bitmap_memory == nullptr)
		{
			m_p_bitmap_memory = new Gdiplus::Bitmap(rcWnd.right, rcWnd.bottom);
		}
		Gdiplus::Graphics graphicsMemory(m_p_bitmap_memory);
		/*{
			Gdiplus::FontFamily fontFamily(L"幼圆");
			Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
			Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
			Gdiplus::StringFormat stringformat = {};
			stringformat.SetAlignment(Gdiplus::StringAlignmentCenter);
			stringformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			graphicsMemory.FillRectangle(&brush, Gdiplus::Rect(0, 0, rcWnd.right, rcWnd.bottom));
			graphicsMemory.DrawString(L"12345", -1, &font, Gdiplus::RectF(0, 0, rcWnd.right, rcWnd.bottom), &stringformat, &brush);
		}*/
		if (m_stream_global.pIStream == NULL)
		{
			LoadResourceData(m_stream_global, 129, "IMG_JPG");
		}
		Gdiplus::Image imgBg(m_stream_global.pIStream);
		//Gdiplus::Image imgBg(L"D:/bg.jpg");
		graphicsMemory.DrawImage(&imgBg, 0, 0, rcWnd.right, rcWnd.bottom);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	INT Run()
	{
		const struct log_arg largl[] = {
		{-1,30,0,0,"main","", DATA_SIZE, 0},
		};
		long n_rotatetime = DEFAULT_LOG_ROTATETIME;
		std::string log_fmt = "%Y%m%d%H%M%S";// DEFAULT_LOG_FMT;
		std::string log_path = APP_DIR + std::string("\\logs");//DEFAULT_LOG_PATH;
		CreateCascadeDirectory(log_path.c_str());
		init_log(log_path.c_str(), log_fmt.c_str(), DEFAULT_LOG_EXT, DEFAULT_LOG_CONS,
			n_rotatetime, LOG_VERBOSE, DEFAULT_LOG_LIMIT_SIZE * 5, largl, sizeof(largl) / sizeof(*largl));
		return CreatePpsdlg(MainProc);
	}
	
public:
	void LoadData(const std::string& jsonName)
	{
		m_task_locker.lock();
		try
		{
			ppsyqm::json json = ppsyqm::json::parse(FILE_READER(jsonName, std::ios::binary));
			if (json["locale_path"].is_string()) {
				std::cout << GCU2A(json["locale_path"].get<std::string>()) << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			printf("%s\n", e.what());//pass
		}
		m_task_locker.unlock();
	}
	void SaveData(const std::string& jsonName)
	{
		m_task_locker.lock();
		ppsyqm::json json;
		json["locale_path"] = GCA2U("local_path");
		auto jsonData = json.dump(2);
		FILE_WRITER(jsonData.data(), jsonData.size(), jsonName, std::ios::binary);
		m_task_locker.unlock();
	}
	void ShowProcessList(HWND hWndList)
	{
		ListView_DeleteAllItems(hWndList);
		LV_ITEM lvi = { 0 };
		int row = 0;
		for (auto it : m_vProcess)
		{
			std::string PID = std::to_string(it.th32ProcessID);
			std::string ProgramName = it.szExeFile;
			std::string Path(MAX_PATH, '\0');
			GetProcessFilePathByPid((LPSTR)Path.data(), Path.size(), it.th32ProcessID);
			lvi.mask = LVIF_TEXT;
			lvi.iItem = row;
			lvi.iSubItem = 0;
			lvi.pszText = (LPSTR)PID.c_str();
			ListView_InsertItem(hWndList, &lvi);

			lvi.iSubItem += 1;
			lvi.pszText = (LPSTR)ProgramName.c_str();
			ListView_SetItem(hWndList, &lvi);

			lvi.iSubItem += 1;
			lvi.pszText = (LPSTR)Path.c_str();
			ListView_SetItem(hWndList, &lvi);
			row++;
		}
	}
	void ShowModulesList(HWND hWndList)
	{
		ListView_DeleteAllItems(hWndList);
		LV_ITEM lvi = { 0 };
		int row = 0;
		for (auto it : m_vModules)
		{
			std::string ModuleName = it.szModule;
			
			std::string BaseAddr = "0x" + (std::stringstream() << std::hex << ((DWORD)it.modBaseAddr)).str();
			std::string BaseAddrSize = "0x" + (std::stringstream() << std::hex << ((DWORD)it.modBaseSize)).str();
			std::string Path = it.szExePath;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = row;
			lvi.iSubItem = 0;
			lvi.pszText = (LPSTR)ModuleName.c_str();
			ListView_InsertItem(hWndList, &lvi);

			lvi.iSubItem += 1;
			lvi.pszText = (LPSTR)BaseAddr.c_str();
			ListView_SetItem(hWndList, &lvi);

			lvi.iSubItem += 1;
			lvi.pszText = (LPSTR)BaseAddrSize.c_str();
			ListView_SetItem(hWndList, &lvi);

			lvi.iSubItem += 1;
			lvi.pszText = (LPSTR)Path.c_str();
			ListView_SetItem(hWndList, &lvi);

			row++;
		}
	}

	std::string m_data_path = std::string(APP_DIR) + "\\data.json";
	std::mutex m_task_locker;
	std::deque<std::string> m_task_queue = {};
public:
	static HookToolWindowHelper* Inst() {
		static HookToolWindowHelper HookToolWindowHelperInstance;
		return &HookToolWindowHelperInstance;
	}
};

__inline static
DWORD WINAPI ThreadStartRoutine(LPVOID lpThreadParameter)
{
	UNREFERENCED_PARAMETER(lpThreadParameter);
	INITCOMMONCONTROLSEX iccex = { 0 };
	InitCommonControls();
	iccex.dwSize = sizeof(iccex);
	iccex.dwICC = 
		ICC_LISTVIEW_CLASSES | // listview, header
		ICC_TREEVIEW_CLASSES | // treeview, tooltips
		ICC_BAR_CLASSES | // toolbar, statusbar, trackbar, tooltips
		ICC_TAB_CLASSES | // tab, tooltips
		ICC_UPDOWN_CLASS | // updown
		ICC_PROGRESS_CLASS | // progress
		ICC_HOTKEY_CLASS | // hotkey
		ICC_ANIMATE_CLASS | // animate
		ICC_WIN95_CLASSES |
		ICC_DATE_CLASSES | // month picker, date picker, time picker, updown
		ICC_USEREX_CLASSES | // comboex
		ICC_COOL_CLASSES | // rebar (coolbar) control
		ICC_INTERNET_CLASSES |
		ICC_PAGESCROLLER_CLASS |   // page scroller
		ICC_NATIVEFNTCTL_CLASS |   // native font control
		ICC_STANDARD_CLASSES |
		ICC_LINK_CLASS;
	InitCommonControlsEx(&iccex);
	EnablePrivilege();
	HookToolWindowHelper::Inst()->Run();
	return 0;
}
// InjectDll.cpp : Defines the entry point for the application.
//

// Windows Header Files
#include <windows.h>
#include <tchar.h>
#include <string>

#if !defined(_UNICODE) && !defined(UNICODE)
#define TSTRING std::string
#else
#define TSTRING std::wstring
#endif
class AppDllMod {
#define APP_NAME "APPDLL"
#define KEY_NAME "APPKEY"
public:
    HMODULE m_hMod = NULL;
public:
    std::string GetProcessFileName() {
        CHAR czModuleFileName[MAX_PATH] = { 0 };
        GetModuleFileNameA(NULL, czModuleFileName, sizeof(czModuleFileName) / sizeof(*czModuleFileName));
        return czModuleFileName;
    }
    HWND GetWindowHwndByPorcessID(DWORD dwProcessID)
    {
        DWORD dwPID = 0;
        HWND hwndRet = NULL;
        // 取得第一个窗口句柄
        HWND hwndWindow = GetTopWindow(NULL);
        while (hwndWindow)
        {
            dwPID = 0;
            // 通过窗口句柄取得进程ID
            DWORD dwTheardID = ::GetWindowThreadProcessId(hwndWindow, &dwPID);
            if (dwTheardID != 0)
            {
                // 判断和参数传入的进程ID是否相等
                if (dwPID == dwProcessID)
                {
                    // 进程ID相等，则记录窗口句柄
                    hwndRet = hwndWindow;
                    break;
                }
            }
            // 取得下一个窗口句柄
            hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT);
        }
        // 上面取得的窗口，不一定是最上层的窗口，需要通过GetParent获取最顶层窗口
        HWND hwndWindowParent = NULL;
        // 循环查找父窗口，以便保证返回的句柄是最顶层的窗口句柄
        while (hwndRet != NULL)
        {
            hwndWindowParent = ::GetParent(hwndRet);
            if (hwndWindowParent == NULL)
            {
                break;
            }
            hwndRet = hwndWindowParent;
        }
        // 返回窗口句柄
        return hwndRet;
    }
    VOID SetValue(const std::string& value, const std::string& appName, const std::string& keyName, const std::string& fileName)
    {
        WritePrivateProfileStringA(appName.c_str(), keyName.c_str(), value.c_str(), fileName.c_str());
    }
    VOID SetValue(const std::wstring& value, const std::wstring& appName, const std::wstring& keyName, const std::wstring& fileName)
    {
        WritePrivateProfileStringW(appName.c_str(), keyName.c_str(), value.c_str(), fileName.c_str());
    }
    std::string GetValue(const std::string& appName, const std::string& keyName, const std::string& fileName, const std::string& defValue = "")
    {
        DWORD value_size = 32;
        std::string value(value_size, '\0');
        while ((value_size = GetPrivateProfileStringA(appName.c_str(), keyName.c_str(), defValue.c_str(), (LPSTR)value.data(), value.size(), fileName.c_str())) == (value.size() - 1))
        {
            value_size += value_size;
            value.resize(value_size, '\0');
        }
        return value.substr(0, value_size);
    }
    std::wstring GetValue(const std::wstring& appName, const std::wstring& keyName, const std::wstring& fileName, const std::wstring& defValue = L"")
    {
        DWORD value_size = 32;
        std::wstring value(value_size, L'\0');
        while ((value_size = GetPrivateProfileStringW(appName.c_str(), keyName.c_str(), defValue.c_str(), (LPWSTR)value.data(), value.size(), fileName.c_str())) == (value.size() - 1))
        {
            value_size += value_size;
            value.resize(value_size, L'\0');
        }
        return value.substr(0, value_size);
    }
public:
    static AppDllMod* Inst() {
        static AppDllMod AppDllModInstance;
        return &AppDllModInstance;
    }
};
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    AppDllMod::Inst()->m_hMod = (HMODULE)hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        OutputDebugString(TEXT("<AppDll.dll> Injection!!!"));

        AppDllMod::Inst()->SetValue("", APP_NAME, KEY_NAME, AppDllMod::Inst()->GetProcessFileName() + std::string(".ini"));
        //创建远程线程
        HANDLE hThread = NULL;
        hThread = CreateThread(NULL, 0, [](LPVOID lParam)->DWORD
            {
                while (TRUE)
                {
                    DWORD dwProcessID = GetCurrentProcessId();
                    HWND hWnd = AppDllMod::Inst()->GetWindowHwndByPorcessID(dwProcessID);
                    if (hWnd)
                    {
                        if (AppDllMod::Inst()->GetValue(APP_NAME, KEY_NAME, AppDllMod::Inst()->GetProcessFileName() + std::string(".ini")).length() > 0)
                        {
                            AppDllMod::Inst()->SetValue("", APP_NAME, KEY_NAME, AppDllMod::Inst()->GetProcessFileName() + std::string(".ini"));
                        }
                        else
                        {
                            AppDllMod::Inst()->SetValue("1", APP_NAME, KEY_NAME, AppDllMod::Inst()->GetProcessFileName() + std::string(".ini"));
                        }
                        if (AppDllMod::Inst()->GetValue(APP_NAME, KEY_NAME, AppDllMod::Inst()->GetProcessFileName() + std::string(".ini")).length() > 0)
                        {
                            SetWindowDisplayAffinity(hWnd, WDA_MONITOR);
                            SetWindowDisplayAffinity(hWnd, WDA_MONITOR | 0x10);
                        }
                        else
                        {
                            SetWindowDisplayAffinity(hWnd, WDA_NONE);
                        }
                    }
                    Sleep(3000);
                }

                return 0;
            }
        , NULL, 0, NULL);
        if (hThread != NULL && hThread != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hThread);
        }
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
// AutoBingUtil.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <deque>
#include <thread>

#include <webdriverxx/picojson.h>
#include <webdriverxx/webdriver.h>
#include <webdriverxx/browsers/ie.h>
#include <webdriverxx/browsers/opera.h>
#include <webdriverxx/browsers/chrome.h>
#include <webdriverxx/browsers/firefox.h>
#include <webdriverxx/browsers/phantom.h>
using namespace webdriverxx;

#include <simdjson.h>
#include <Operator.h>
#include <Converter.h>
#include <AesHelper.h>
#include <CurlHelper.h>
#include <Windowapi.h>
#include <Randnonce.h>
#include <WindowHelper.h>

#include <xlnt/xlnt.hpp>

#define CMD_PATH_NAME			"CMD.EXE" 
#define SEO_URL					"http://xjp.ppsbbs.tech:18001"

__inline static //获取系统路径
std::string GetSystemPath()
{
	std::string strSystemPath = ("");
	CHAR czSystemPath[MAX_PATH] = { 0 };
	::GetSystemDirectoryA(czSystemPath, MAX_PATH);
	if (*czSystemPath)
	{
		strSystemPath = std::string(czSystemPath) + ("\\");
	}
	return strSystemPath;
}
__inline static //获取系统路径
std::string GetSystemPathX64()
{
	std::string strSystemPath = ("");
	CHAR czSystemPath[MAX_PATH] = { 0 };
	::GetSystemWow64DirectoryA(czSystemPath, MAX_PATH);
	if (*czSystemPath)
	{
		strSystemPath = std::string(czSystemPath) + ("\\");
	}
	return strSystemPath;
}
__inline static //获取cmd.exe文件路径
std::string GetCmdPath()
{
	return GetSystemPath() + (CMD_PATH_NAME);
}
__inline static //获取程序工作路径
std::string GetWorkPath()
{
	std::string strWorkPath = ("");
	CHAR czWorkPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, czWorkPath);
	if (*czWorkPath)
	{
		strWorkPath = std::string(czWorkPath) + ("\\");
	}
	return strWorkPath;
}
__inline static //获取系统临时路径
std::string GetTemporyPath()
{
	CHAR czTempPath[MAX_PATH] = { 0 };
	::GetTempPathA(MAX_PATH, czTempPath);
	return std::string(czTempPath);
}
__inline static //获取程序文件路径
std::string GetProgramPath()
{
	std::string strFilePath = ("");
	CHAR* pFoundPosition = 0;
	CHAR czFilePath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, czFilePath, MAX_PATH);
	if (*czFilePath)
	{
		pFoundPosition = strrchr(czFilePath, ('\\'));
		if (*(++pFoundPosition))
		{
			*pFoundPosition = ('\0');
		}
		strFilePath = czFilePath;
	}
	return strFilePath;
}

__inline static //获取程序文件名称
std::string GetProgramName()
{
	std::string strFilePath = ("");
	CHAR* pFoundPosition = 0;
	CHAR czFilePath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, czFilePath, MAX_PATH);
	if (*czFilePath)
	{
		pFoundPosition = strrchr(czFilePath, ('\\'));
		strFilePath = pFoundPosition + 1;
	}
	return strFilePath;
}


/////////////////////////////////////////////////////////////////

const char iv[16] = { 0 };

#define APP_KEY	"base64:cHBzaHVhaTZlOHA5M2F2OHpqZHBwc2h1YWlkd2V0ZHQ="
namespace std
{
	class values {
	public:
		values(std::int8_t _type, const std::string& _data)
		{
			this->type = _type;
			this->data.assign(_data.begin(), _data.end());
		}
		std::int8_t type;//是否为字符串。1-字符串，0-非字符串
		std::string data;
	};
	class words {
	public:
		words(const std::string& _word, const std::string& _sub_word)
		{
			word.assign(_word.begin(), _word.end());
			sub_word.assign(_sub_word.begin(), _sub_word.end());
		}
		std::string word;
		std::string sub_word;
	};
}
__inline static std::string post_data_crypt(const std::string& url, const std::map<std::string, std::values>& svmap)
{
	std::string result = ("");
	std::string json_data = ("");
	json_data = ("{");
	for (auto it : svmap)
	{
		if (json_data.size() > 1)
		{
			json_data.append(",");
		}
		if (it.second.type == 1)
		{
			json_data.append("\"").append(it.first.c_str()).append("\":\"").append(it.second.data.c_str()).append("\"");
		}
		else
		{
			json_data.append("\"").append(it.first.c_str()).append("\":").append(it.second.data.c_str());
		}
	}
	json_data.append("}");
	std::size_t data_size = json_data.length();
	for (std::size_t i = 0; data_size % 16 && i < (16 - data_size % 16); i++)
	{
		json_data.append(1, (16 - data_size % 16));
	}
	json_data = AES_Encrypt(json_data, ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
	json_data = base64_encode((const unsigned char*)json_data.data(), json_data.size());
	json_data = "{\"data\":\"" + json_data + "\"}";
	result = post_data(url, json_data);
	if (result.length() > 10 && result.find("{\"data\":\"") == 0)
	{
		result = base64_decode(result.substr(9, result.length() - 11));
		result = AES_Decrypt(result, ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
		for (std::string::reverse_iterator it = result.rbegin(); it != result.rend() && (*it >= '\x00') && (*it <= '\x0F'); it++)(*it) = '\x00';
	}
	return std::move(result.c_str());
}
__inline static std::string post_form_crypt(const std::string& url, const std::map<std::string, std::string>& ssmap)
{
	std::string result = ("");
	auto _ssmap = std::move(ssmap);
	std::size_t data_size = _ssmap.at("data").length();
	for (std::size_t i = 0; data_size % 16 && i < (16 - data_size % 16); i++)
	{
		_ssmap.at("data").append(1, (16 - data_size % 16));
	}
	_ssmap.at("data") = AES_Encrypt(_ssmap.at("data"), ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
	_ssmap.at("data") = base64_encode((const unsigned char*)_ssmap.at("data").c_str(), _ssmap.at("data").size());
	result = post_form(url, _ssmap);
	if (result.length() && *result.begin() == '\n')
	{
		result = base64_decode(result.substr(1));
		result = AES_Decrypt(result, ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
		for (std::string::reverse_iterator it = result.rbegin(); it != result.rend() && (*it >= '\x00') && (*it <= '\x0F'); it++)(*it) = '\x00';
	}
	return std::move(result.c_str());
}
__inline static std::string encrypt_machine(std::string s)
{
	std::size_t data_size = s.length();
	for (std::size_t i = 0; data_size % 16 && i < (16 - data_size % 16); i++)
	{
		s.append(1, (16 - data_size % 16));
	}
	s = AES_Encrypt(s, ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
	s = base64_encode((const unsigned char*)s.c_str(), s.size());
	return s.c_str();
}
__inline static std::string decrypt_machine(std::string s)
{
	s = base64_decode(s);
	s = AES_Decrypt(s, ECMTYPE_ECB, SKBTYPE_256, PMTYPE_NULL, base64_decode(std::string(APP_KEY).substr(7)).c_str(), iv);
	for (std::string::reverse_iterator it = s.rbegin(); it != s.rend() && (*it >= '\x00') && (*it <= '\x0F'); it++)(*it) = '\x00';
	return s.c_str();
}
#include <nb30.h>
__inline static char* NcbMac(char* ncb_mac, unsigned int mac_len)
{
	int index = 0;
	NCB ncb = { 0 };
	ADAPTER_STATUS adapt;
	LANA_ENUM lana_enum = { 0 };

	FARPROC fnNetbios = 0;
	const char* _METHOD_NAME = ("Netbios");
	const TCHAR* _MODULE_NAME = TEXT("NETAPI32");
	HMODULE hModule = GetModuleHandle(_MODULE_NAME);
	if (hModule)
	{
		fnNetbios = GetProcAddress(hModule, _METHOD_NAME);
		hModule = 0;
	}
	else
	{
		hModule = LoadLibrary(_MODULE_NAME);
		if (hModule)
		{
			fnNetbios = GetProcAddress(hModule, _METHOD_NAME);
		}
	}

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBENUM;
	ncb.ncb_length = sizeof(lana_enum);
	ncb.ncb_buffer = (unsigned char*)&lana_enum;

	if (fnNetbios && ((UCHAR(APIENTRY*)(PNCB))fnNetbios)(&ncb) == 0 && lana_enum.length > 0)
	{
		for (index = 0; index < lana_enum.length; index++)
		{
			//发送命令进行初始化
			ncb.ncb_command = NCBRESET;
			ncb.ncb_lana_num = lana_enum.lana[index];

			//发送ncbastat获取网卡信息
			if (((UCHAR(APIENTRY*)(PNCB))fnNetbios)(&ncb) == 0)
			{
				ncb.ncb_command = NCBASTAT;
				strcpy((char*)ncb.ncb_callname, "*");
				ncb.ncb_lana_num = lana_enum.lana[index];
				ncb.ncb_buffer = (unsigned char*)&adapt;
				ncb.ncb_length = sizeof(adapt);
				if (((UCHAR(APIENTRY*)(PNCB))fnNetbios)(&ncb) == 0)
				{
					snprintf(ncb_mac, mac_len,
						"%s%02X%02X%02X%02X%02X%02X",
						ncb_mac,
						adapt.adapter_address[0],
						adapt.adapter_address[1],
						adapt.adapter_address[2],
						adapt.adapter_address[3],
						adapt.adapter_address[4],
						adapt.adapter_address[5]);
				}
			}
		}
		{
			const int LEN = 12;
			char temp[LEN] = { 0 };
			size_t i = 0, j = 0, min = 0, len = strlen(ncb_mac) / LEN;
			for (i = 0; i < len - 1; i++)
			{
				min = i;
				for (j = i + 1; j < len; j++)
				{
					if (strncmp(ncb_mac + j * LEN, ncb_mac + min * LEN, LEN) < 0)
					{
						min = j;
					}
				}
				if (min != i)
				{
					strncpy(temp, ncb_mac + min * LEN, LEN);
					strncpy(ncb_mac + min * LEN, ncb_mac + i * LEN, LEN);
					strncpy(ncb_mac + i * LEN, temp, LEN);
				}
			}
		}
	}
	else
	{
		return 0;
	}
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = 0;
	}
	return ncb_mac;
}

#define TIMES_1S 1000
#define TIMES_1M 60 * 1000
#define TIMES_2S 2 * TIMES_1S
#define TIMES_3S 3 * TIMES_1S
#define TIMES_5S 5 * TIMES_1S
#define TIMES_5M 5 * TIMES_1M
#define TIMES_8S 8 * TIMES_1S
#define TIMES_10S 10 * TIMES_1S

#define RAND_WAIT(MAX, MIN) std::this_thread::sleep_for(std::chrono::milliseconds((rand() % (MAX - MIN + 1)) + MIN))

#if defined(DEBUG) || defined(_DEBUG)
//#define SEO_URL "http://192.168.100.1:18001"
#endif
#define SEOAREA_QA "/seodropdown/queryAll"
#define SEOAREA_UA "/seodropdown/updateAll"

class AutoBingUtil {
public:
	AutoBingUtil() {
	}
	virtual ~AutoBingUtil() {
	}
public:
	BOOL m_bRunning = FALSE;
	BOOL m_bStarted = FALSE;
	BOOL m_bHeadless = TRUE;
	std::string m_javajarexe = "";
	std::string m_javajarexe_org = "jj.exe";
	std::string m_firefoxexe = "";
	std::string m_firefoxexe_org = "ff.exe";
	std::string m_geckodriverexe = "";
	std::string m_geckodriverexe_org = "gg.exe";
	std::string m_firefoxfilepath = "";
	std::string m_ffdriverfilepath = "";
	std::string m_current_timestamp = std::to_string(time(nullptr));
	std::string m_work_directory = GetTemporyPath() + GetProgramName();
	std::string m_webdriverurl = "http://localhost:4444/wd/hub/";
	std::string m_webdriverport = CRandomString::GetInstance()->make_random_long(9999,65535);//"4444";
	DWORD dwUID = 0;
	DWORD dwRID = 0;
public:
	int environment_initialized(void* p, DWORD dwUID = 100, DWORD dwRID = 101)
	{
		this->dwUID = dwUID; this->dwRID = dwRID;
		WindowHelper::Inst()->CreatePpsdlg((LPVOID)(WNDPROC)[]
		(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
#define MESSAGE_TEXT_ID 1001
#define WM_USER_MESSAGE WM_USER + WM_NOTIFY
#define MESSAGE_DEFINES_TEXT "MESSAGE_DEFINES_TEXT"
				WindowHelper* thiz = WindowHelper::Inst();
				switch (uMsg)
				{
				case WM_CREATE:
				{
					RemoveMenu(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND);

					HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(128));
					SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
					SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
					SendMessage(hWnd, WM_SETICON, ICON_SMALL2, (LPARAM)hIcon);

					DisplayRequestRepaint(hWnd, 300, 200, TRUE);

					SetWindowText(hWnd, TEXT("环境准备工具"));

					SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));

					thiz->CreateControl(MESSAGE_TEXT_ID, WC_STATIC, TEXT(""), SS_CENTER | WS_CHILD | WS_VISIBLE, 0L, hWnd);
					thiz->InitGridLayout(hWnd);
					thiz->AddControlToLayout(hWnd, 0, 0, MESSAGE_TEXT_ID, NULL, WindowHelper::cc_item::ST_XY_SCALE, 160, 40, 1.0, 1.0);
					thiz->LayoutOptimize(hWnd);

					SetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT), (HANDLE)new TCHAR[MAX_PATH]());
					memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
					lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中..."));
					PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
					std::thread([hWnd]()
						{
							std::string commandline("");

							CreateCascadeDirectory(AutoBingUtil::Inst()->m_work_directory.c_str(), NULL);
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第一步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);

							auto uexe = TakeResourceData(AutoBingUtil::Inst()->dwUID, TEXT("FILE"));
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第二步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							auto drar = TakeResourceData(AutoBingUtil::Inst()->dwRID, TEXT("FILE"));
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第三步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							FILE_WRITER(uexe.data(), uexe.size(), (AutoBingUtil::Inst()->m_work_directory + TEXT("\\u.exe")), std::ios::binary);
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第四步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							FILE_WRITER(drar.data(), drar.size(), (AutoBingUtil::Inst()->m_work_directory + TEXT("\\d.rar")), std::ios::binary);
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第五步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							commandline = (AutoBingUtil::Inst()->m_work_directory + "\\u.exe x -inul -o+ -pPpsWebNetApp118 "
								+ AutoBingUtil::Inst()->m_work_directory + "\\d.rar " + AutoBingUtil::Inst()->m_work_directory);
							ExecuteCommand(commandline.c_str());
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第六步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							AutoBingUtil::Inst()->service_restart();
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第七步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							commandline = ("CMD /C \"DEL /S /Q " + AutoBingUtil::Inst()->m_work_directory + "\\d.rar\"");
							ExecuteCommand(commandline.c_str());
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第八步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);
							commandline = ("CMD /C \"DEL /S /Q " + AutoBingUtil::Inst()->m_work_directory + "\\u.exe\"");
							ExecuteCommand(commandline.c_str());
							memset((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), 0, MAX_PATH * sizeof(TCHAR));
							lstrcpy((LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)), TEXT("(共九步)环境准备中...(第九步完成)"));
							PostMessage(hWnd, WM_USER_MESSAGE, (WPARAM)NULL, (LPARAM)NULL);

							AppendMenu(GetSystemMenu(hWnd, TRUE), SC_CLOSE, MF_BYCOMMAND, NULL);
							PostMessage(hWnd, WM_QUIT, (WPARAM)NULL, (LPARAM)NULL);
						}
					).detach();
				}
				return 0;
				case WM_USER_MESSAGE:
				{
					SetDlgItemText(hWnd, MESSAGE_TEXT_ID, (LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT)));
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
						EndPaint(hWnd, &ps);
					}
				}
				break;
				case WM_CLOSE:
				{
					delete (LPTSTR)GetProp(hWnd, TEXT(MESSAGE_DEFINES_TEXT));
				}
				break;
				}
				return 0;
			}, (HWND)p);

		return 0;
	}
	void service_restart()
	{
		std::string commandline("");
		m_current_timestamp = std::to_string(time(nullptr));

		m_javajarexe = "jjj" + m_current_timestamp + ".exe";
		m_firefoxexe = "fff" + m_current_timestamp + ".exe";
		m_geckodriverexe = "ggg" + m_current_timestamp + ".exe";

		m_firefoxfilepath = m_work_directory + "\\data\\" + m_firefoxexe;
		putenv(("PATH=" + m_work_directory + "\\data\\jre\\bin;" + getenv("PATH")).c_str());
		std::cout << getenv("PATH") << std::endl;
		m_webdriverurl = "http://127.0.0.1:" + m_webdriverport + "/wd/hub/";
		m_ffdriverfilepath = "-Dwebdriver.gecko.driver=\""
			+ m_work_directory + "\\data\\" + m_geckodriverexe + "\" \"" + m_work_directory + "\\data\\ss\"";

		commandline = (GetCmdPath() + " /C COPY \"" + m_work_directory + "\\data\\jre\\bin\\"
			+ m_javajarexe_org + "\" \"" + m_work_directory + "\\data\\jre\\bin\\" + m_javajarexe + "\" /Y");
		ExecuteCommand(commandline.c_str());
		commandline = (GetCmdPath() + " /C COPY \"" + m_work_directory + "\\data\\"
			+ m_firefoxexe_org + "\" \"" + m_work_directory + "\\data\\" + m_firefoxexe + "\" /Y");
		ExecuteCommand(commandline.c_str());
		commandline = (GetCmdPath() + " /C COPY \"" + m_work_directory + "\\data\\"
			+ m_geckodriverexe_org + "\" \"" + m_work_directory + "\\data\\" + m_geckodriverexe + "\" /Y");
		ExecuteCommand(commandline.c_str());
	}
	int start_selenium_server() {
		
		CurlHttpUtil::Inst();

		std::thread([]()
			{
				AutoBingUtil::Inst()->m_bRunning = TRUE;
				while (AutoBingUtil::Inst()->m_bRunning && !AutoBingUtil::Inst()->m_bStarted)
				{
					std::string commandline("");
					AutoBingUtil::Inst()->service_restart();
					commandline = (GetCmdPath() + " /C "
						+ AutoBingUtil::Inst()->m_javajarexe
						+ " -jar " + AutoBingUtil::Inst()->m_ffdriverfilepath
						+ " -host 127.0.0.1 -port " + AutoBingUtil::Inst()->m_webdriverport);
					std::cout << commandline << std::endl;

					ExecuteCommand(commandline.c_str(), AutoBingUtil::Inst()->m_bStarted);
					AutoBingUtil::Inst()->m_bStarted = FALSE;
					RAND_WAIT(TIMES_1S, TIMES_1S);
				}
			}
		).detach();

		return 0;
	}
	int stop_service(void* p)
	{
		m_bRunning = FALSE;
		std::thread([p]() {
			std::string commandline("");
			commandline = GetCmdPath() + " /C TASKKILL /T /F /IM " + AutoBingUtil::Inst()->m_javajarexe;
			ExecuteCommand(commandline.c_str());
			commandline = GetCmdPath() + " /C TASKKILL /T /F /IM " + AutoBingUtil::Inst()->m_firefoxexe;
			ExecuteCommand(commandline.c_str());
			commandline = GetCmdPath() + " /C TASKKILL /T /F /IM " + AutoBingUtil::Inst()->m_geckodriverexe;
			ExecuteCommand(commandline.c_str());
			if ((HWND)p != NULL)
			{
				((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
			}
			}
		).join();
		return 0;
	}
	int environment_exitialized(void *p)
	{
		stop_service(p);
		std::string commandline("");
		commandline = GetCmdPath() + " /C DEL /S /Q " + m_work_directory + "\\data\\jjj*.exe";
		ExecuteCommand(commandline.c_str());
		commandline = GetCmdPath() + " /C DEL /S /Q " + m_work_directory + "\\data\\fff*.exe";
		ExecuteCommand(commandline.c_str());
		commandline = GetCmdPath() + " /C DEL /S /Q " + m_work_directory + "\\data\\ggg*.exe";
		ExecuteCommand(commandline.c_str());
		return 0;
	}
	int seo_pps_bbs()
	{
		try
		{
			auto MIN_NUM = 1;
			auto MAX_NUM = 600;
			std::string root_url = "http://www.ppsbbs.tech/";
			std::string item_url = "http://www.ppsbbs.tech/thread-" + std::to_string((rand() % (MAX_NUM - MIN_NUM + 1)) + MIN_NUM) + ".htm";
			auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
			firefox.Set("acceptInsecureCerts", true);
			if (AutoBingUtil::Inst()->m_bHeadless)
			{
				firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
			}
			Capabilities desired = firefox;
			Capabilities required = Capabilities();
			//desired = desired.SetProxy(HttpProxy(sv.at(rand() % sv.size()).c_str()));
			WebDriver browser = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);
			try
			{
				browser.Navigate(root_url);
				RAND_WAIT(TIMES_10S, TIMES_8S);
				browser.Navigate(item_url);
				RAND_WAIT(TIMES_10S, TIMES_8S);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			browser.DeleteCookies();
			browser.CloseCurrentWindow();
			browser.DeleteSession();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		return 0;
	}
	int seo_baidu_seo(const std::string& word, const std::string& sub_word)
	{
		try
		{
			long nOffset = 0;
			long MAX_OFFSET = 10000;
			const std::string url = "https://www.baidu.com";
			auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
			firefox.Set("acceptInsecureCerts", true);
			if (AutoBingUtil::Inst()->m_bHeadless)
			{
				firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
			}
			Capabilities desired = firefox;
			Capabilities required = Capabilities();
			//desired = desired.SetProxy(HttpProxy(sv.at(rand() % sv.size()).c_str()));
			WebDriver browser = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);
			browser.Navigate(url);

			browser.FindElement(ById("kw"))
				.Clear()
				.SendKeys(word)
				.Submit();
			nOffset = rand() % MAX_OFFSET;
			browser.Execute("window.scrollBy(0," + std::to_string(nOffset) + ")");
			RAND_WAIT(TIMES_10S, TIMES_5S);
			browser.Execute("window.scrollBy(0," + std::to_string(MAX_OFFSET - nOffset) + ")");
			RAND_WAIT(TIMES_10S, TIMES_5S);

			browser.FindElement(ById("kw"))
				.Clear()
				.SendKeys(sub_word)
				.Submit();
			nOffset = rand() % MAX_OFFSET;
			browser.Execute("window.scrollBy(0," + std::to_string(nOffset) + ")");
			RAND_WAIT(TIMES_10S, TIMES_5S);
			browser.Execute("window.scrollBy(0," + std::to_string(MAX_OFFSET - nOffset) + ")");
			RAND_WAIT(TIMES_10S, TIMES_5S);
			{
				std::string resp = post_data_crypt(SEO_URL SEOAREA_UA,
					{
						{"w",{1,StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(word))}},
						{"sw",{1,StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(sub_word))}},
					}
				);
			}

			browser.DeleteCookies();
			browser.CloseCurrentWindow();
			browser.DeleteSession();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		return 0;
	}
	int seo_baidu_map_tel(const std::deque<std::string>& queue, const std::string& path)
	{
		try
		{
			const std::string url = "https://map.baidu.com/search";
			auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
			firefox.Set("acceptInsecureCerts", true);
			if (AutoBingUtil::Inst()->m_bHeadless)
			{
				firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
			}
			Capabilities desired = firefox;
			Capabilities required = Capabilities();
			//desired = desired.SetProxy(HttpProxy(sv.at(rand() % sv.size()).c_str()));
			WebDriver webdriver = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);
			try
			{
				for (auto& it : queue)
				{
					seo_pps_bbs();
					webdriver.Navigate(url);
					RAND_WAIT(TIMES_3S, TIMES_1S);
					webdriver.FindElement(ById("sole-input")).Clear();
					webdriver.FindElement(ById("sole-input")).SendKeys(StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW(it)));
					webdriver.FindElement(ById("search-button")).Click();
					RAND_WAIT(TIMES_3S, TIMES_1S);
					webdriver.FindElement(ById("search-button")).Click();
					RAND_WAIT(TIMES_3S, TIMES_1S);
					int current_page_index = 0;
					boolean have_next_page = true;
					int first_enter_count = 0;
					std::string first_enter_texts("");
					std::string str_data_list = ("");
					while (have_next_page == true) {
						current_page_index += 1;
						have_next_page = false;
						try {
							auto cards = webdriver.FindElements(ByCss("[class='card status-fold fold']"));
							if (!cards.empty())
							{
								JsArgs jsArgs;
								jsArgs << webdriver.FindElement(ByCss("[class='card status-fold fold']"));
								webdriver.Execute("arguments[0].setAttribute('class','card')", jsArgs);
								RAND_WAIT(TIMES_3S, TIMES_1S);
							}
							auto curr_page = webdriver.FindElements(ByCss("[class='curPage']"));
							if (!curr_page.empty())
							{
								auto text = curr_page.begin()->GetText();
								std::cout << text << std::endl;
								if (!text.empty() && first_enter_texts.empty() || text.compare(first_enter_texts) == 0)
								{
									first_enter_texts.swap(text);
									first_enter_count++;
								}
								if (first_enter_count >= 2)
								{
									break;
								}
							}
							auto we_poilist = webdriver.FindElements(ByClass("poilist"));
							for (auto we_poiitem : we_poilist) {
								try {
									auto we_poiitem_li_list = we_poiitem.FindElements(ByTag("li"));
									for (auto we_poiitem_li : we_poiitem_li_list) {
										try {
											auto we_poiitem_li_ml_30 = we_poiitem_li
												.FindElement(ByClass("ml_30"));
											std::string strRow = we_poiitem_li_ml_30
												.FindElement(ByClass("row")).GetText();
											std::string strAddr = we_poiitem_li_ml_30
												.FindElement(ByClass("addr")).GetText();
											std::string strTel = we_poiitem_li_ml_30
												.FindElement(ByClass("tel")).GetText();

											if (strRow.length() > 0 && strAddr.length() > 0 && strTel.length() > 0) {
												str_data_list.append("\"" + StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strRow)) + "\",\""
													+ StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAddr)) + "\",\""
													+ StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strTel)) + "\"").append("\r\n");
												std::cout << current_page_index << " Page: This data is: " << strRow + "," + strAddr + "," + strTel << std::endl;
											}
										}
										catch (std::exception e) {
											// TODO: handle exception
											std::cout << current_page_index << " Page: Data Error pass " << e.what() << std::endl;
										}
									}
								}
								catch (std::exception e) {
									// TODO: handle exception
									std::cout << current_page_index << " Page: Poillist_li Error pass " << e.what() << std::endl;
								}
							}
						}
						catch (const std::exception& e) {
							// TODO: handle exception
							std::cout << current_page_index << " Page: Poillist Error pass " << e.what() << std::endl;
						}
						try {
							auto next_page = webdriver.FindElements(ByCss("[class='next next-none']"));
							if (!next_page.empty())
							{
								break;
							}
							auto we_poi_page_a_list = webdriver.FindElement(ById("poi_page")).FindElements(ByTag("a"));
							for (auto we_poi_page_a_item : we_poi_page_a_list) {
								try {
									if (we_poi_page_a_item.IsEnabled() && we_poi_page_a_item.GetAttribute("tid").compare("toNextPage") == 0) {
										if (we_poi_page_a_item.GetAttribute("class").find("next-none") == std::string::npos) {
											we_poi_page_a_item.Click();
											std::cout << current_page_index << " Page: Click Next Page" << std::endl;
											have_next_page = true;
											break;
										}
									}
								}
								catch (std::exception e) {
									// TODO: handle exception
									std::cout << current_page_index << " Page: NextPageItem Error pass " << e.what() << std::endl;
								}
							}
						}
						catch (const std::exception& e) {
							// TODO: handle exception
							std::cout << current_page_index << " Page: NextPage Error pass " << e.what() << std::endl;
						}
						RAND_WAIT(TIMES_3S, TIMES_1S);
					}

					FILE_WRITER(str_data_list.data(), str_data_list.size(), path + it + "-out.csv", std::ios::binary);
				}
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			webdriver.DeleteCookies();
			webdriver.CloseCurrentWindow();
			webdriver.DeleteSession();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		return 0;
	}
	int start_service(void* p)
	{
		start_selenium_server();
		std::thread([p]() {
			std::string url = "";
			std::string user = "";
			std::string word = "";
			std::string sub_word = "";
			std::string proxy_list = "";
			int32_t nPos = 0;
			int32_t nNum = 100;
			int32_t nCnt = 100;
			simdjson::dom::parser parser;
			std::deque<std::words> wordsQueue;
			if ((HWND)p != NULL)
			{
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0xFFFFFFFF);
				while ((HWND)p != NULL)
				{
					if (GetProp((HWND)p, TEXT("RUNNING")) == 0)
					{
						break;
					}
					if (!AutoBingUtil::Inst()->m_bStarted)
					{
						//std::this_thread::sleep_for(std::chrono::milliseconds(TIMES_1S));
						AutoBingUtil::Inst()->seo_pps_bbs();
						continue;
					}
					if (wordsQueue.empty())
					{
						simdjson::dom::element dome;
						std::string resp = post_data_crypt(SEO_URL SEOAREA_QA,
							{
								{"pos",{0,std::to_string(nPos)}},
								{"num",{0,std::to_string(nNum)}},
							}
						);
						if (resp.size() <= 0 || *resp.data() != '{')
						{
							//std::this_thread::sleep_for(std::chrono::milliseconds(TIMES_10S));
							AutoBingUtil::Inst()->seo_pps_bbs();
							continue;
						}
						dome = parser.parse(resp.data(), resp.size());
						if (dome.is_object())
						{
							std::cout << dome["code"].is_int64() << std::endl;
							if (dome["code"].is_int64() && dome["code"].get_int64().value() == 0)
							{
								if (dome["data"]["size"].is_int64())
								{
									nCnt = dome["data"]["size"].get_int64().value();
									if (nCnt > nNum)
									{
										nPos = (nPos + nNum);
										if (nPos > nCnt)
										{
											nPos = 0;
										}
									}
								}
								std::cout << dome["data"]["list"].is_array() << std::endl;
								if (dome["data"]["list"].is_array())
								{
									auto _array = dome["data"]["list"].get_array();
									for (size_t i = 0; i < _array.size(); i++)
									{
										//std::cout << "w=" << _array.at(i)["w"].get_c_str().value() << std::endl;
										//std::cout << "sw=" << _array.at(i)["sw"].get_c_str().value() << std::endl;
										wordsQueue.emplace_back(std::words(_array.at(i)["w"].get_c_str().value(), _array.at(i)["sw"].get_c_str().value()));
									}
								}
							}
						}
					}
					if (!wordsQueue.empty())
					{
						break;
					}
					//std::this_thread::sleep_for(std::chrono::milliseconds(TIMES_5M));
					AutoBingUtil::Inst()->seo_pps_bbs();
					continue;
				}
				while (!wordsQueue.empty())
				{
					std::words& words = wordsQueue.front();
					word = words.word;
					sub_word = words.sub_word;
					wordsQueue.pop_front();
					AutoBingUtil::Inst()->seo_baidu_seo(word, sub_word);
				}
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0);
				((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
			}
			}
		).detach();

		return 0;
	}
	int start_baidu_map_tel_service(void* p, const std::string& keyword)
	{
		start_selenium_server();
		std::thread([p, keyword]() {
			if ((HWND)p != NULL)
			{
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0xFFFFFFFF);
				while ((HWND)p != NULL)
				{
					if (GetProp((HWND)p, TEXT("RUNNING")) == (HANDLE)0)
					{
						break;
					}
					if (!AutoBingUtil::Inst()->m_bStarted)
					{
						RAND_WAIT(TIMES_3S, TIMES_1S);
						continue;
					}
					AutoBingUtil::Inst()->seo_baidu_map_tel({ keyword }, ((std::string(*)())GetProp((HWND)p, TEXT("OUTPUT_FOLDER")))());
					SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0);
					((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
				}
			}
			}
		).detach();

		return 0;
	}
	int start_aliexpress_service(void* p, const std::string& prefix = "aliexpress_main")
	{
		start_selenium_server();
		std::thread([p, prefix]() {
#define MBDFS_DELAY_TIME	5000
			long nDelayTime = MBDFS_DELAY_TIME;
			std::string strInput = "";
			std::string strFirstLine = "";
			std::string strPassword = "";
			std::string strDelayTime = "";
			std::string strLastPosition = "";
			std::vector<std::string> sv;
			std::string strInputFileName = GetProgramPath() + prefix + "-input.csv";
			if (access(strInputFileName.c_str(), 0) != 0)
			{
				strInputFileName = GetProgramPath() + prefix + "-input.xlsx";
				if (access(strInputFileName.c_str(), 0) == 0)
				{
					xlnt::workbook wb;
					wb.load(strInputFileName);
					auto ws = wb.active_sheet();
					for (size_t i = 1; i < 65535; i++)
					{
						std::string cellIndex = "A" + std::to_string(i);
						if (ws.cell(cellIndex).has_value())
						{
							sv.emplace_back(ws.cell(cellIndex).value<std::string>());
							//std::cout << ws.cell(cellIndex).value<std::string>() << std::endl;
						}
					}
				}
			}
			
			std::string strOutputFileName = GetProgramPath() + prefix + "-app-output.csv";
			std::string strPasswordFileName = GetProgramPath() + prefix + "-password.txt";
			std::string strDelayTimeFileName = GetProgramPath() + prefix + "-delay_time.txt";
			std::string strFirstLineFileName = GetProgramPath() + prefix + "-first_line.txt";
			std::string strErrorInfoFileName = GetProgramPath() + prefix + "-error_info.log";
			std::string strLastPositionFileName = GetProgramPath() + prefix + "-app-last_position.ini";

			std::size_t nCount = 0;
			std::size_t nLastPosition = 0;
			if ((HWND)p != NULL)
			{
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0xFFFFFFFF);
				while ((HWND)p != NULL)
				{
					if (GetProp((HWND)p, TEXT("RUNNING")) == (HANDLE)0)
					{
						break;
					}
					if (!AutoBingUtil::Inst()->m_bStarted)
					{
						RAND_WAIT(TIMES_3S, TIMES_1S);
						continue;
					}
					std::string strInfo = "错误信息:";
					FILE_WRITER(strInfo.data(), strInfo.size(), strErrorInfoFileName, std::ios::binary);
					strLastPosition = FILE_READER(strLastPositionFileName, std::ios::binary);
					strFirstLine = FILE_READER(strFirstLineFileName, std::ios::binary);
					strDelayTime = FILE_READER(strDelayTimeFileName, std::ios::binary);
					if (sv.empty() && strInputFileName.rfind(".csv") != std::string::npos)
					{
						strInput = FILE_READER(strInputFileName, std::ios::binary);
					}
					strPassword = FILE_READER(strPasswordFileName, std::ios::binary);

					if (strDelayTime.length() <= 0)
					{
						nDelayTime = MBDFS_DELAY_TIME;
					}
					else
					{
						nDelayTime = std::stol(strDelayTime);
						if (nDelayTime <= 0)
						{
							nDelayTime = MBDFS_DELAY_TIME;
						}
					}
					if (strLastPosition.length() <= 0)
					{
						nLastPosition = 0;
						FILE_WRITER(strFirstLine.data(), strFirstLine.size(), strOutputFileName, std::ios::binary);
					}
					else
					{
						nLastPosition = std::stoul(strLastPosition);
					}

					if (!strInput.empty())
					{
						StringConvertUtils::Instance()->string_split_to_vector(sv, strInput, "\r\n");
					}

					nCount = sv.size();
					if (nCount)
					{
						int nId = ::MessageBoxA(NULL, ("是否继续上次操作进行?\r\n\r\n【是】-继续进行 【否】-重新开始 【取消】-退出程序"), ("开始提示"), MB_YESNOCANCEL);
						switch (nId)
						{
						case IDNO:
						{
							nLastPosition = 0;
							FILE_WRITER(strFirstLine.data(), strFirstLine.size(), strOutputFileName, std::ios::binary);
						}
						case IDYES:
						{
							nLastPosition = nLastPosition;
						}
						{
							FILE_WRITER(std::to_string(nLastPosition).data(), std::to_string(nLastPosition).size(), strLastPositionFileName, std::ios::binary);
							if (nLastPosition <= nCount)
							{
								std::size_t i = 0;
								std::size_t j = 0;
								std::size_t k = 0;
								try
								{
									const std::string firstUrl = "https://login.aliexpress.com/";
									const std::string secondUrl = "https://bp.aliexpress.com/manager/keyword_tools.htm";
									auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
									firefox.Set("acceptInsecureCerts", true);
									if (AutoBingUtil::Inst()->m_bHeadless)
									{
										firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
									}
									Capabilities desired = firefox;
									Capabilities required = Capabilities();
									//desired = desired.SetProxy(HttpProxy(sv.at(rand() % sv.size()).c_str()));
									WebDriver webdriver = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);

									webdriver.Navigate(firstUrl);
									RAND_WAIT(TIMES_3S, TIMES_1S);
									webdriver.FindElement(ById("fm-login-id")).Clear().SendKeys("3082213955@qq.com");
									webdriver.FindElement(ById("fm-login-password")).Clear().SendKeys(strPassword);
									RAND_WAIT(TIMES_3S, TIMES_1S);
									webdriver.FindElement(ByClass("fm-btn")).Click();
									RAND_WAIT(TIMES_3S, TIMES_1S);
									::MessageBoxA(NULL, ("请您在登陆后关闭此窗口继续......"), ("提示"), MB_OK);
									webdriver.Navigate(secondUrl);
									RAND_WAIT(TIMES_2S, TIMES_1S);

									for (i = nLastPosition; nLastPosition < nCount; nLastPosition++)
									{
										RAND_WAIT(TIMES_1S, TIMES_1S);
										i++;
										j = 0;
										std::string strLines = "";
										std::string strKeyword = sv.at(nLastPosition);
										std::string strHotKeyword = "";//热度词
										std::string strHotDegree = "";//热度
										std::string strVieDegree = "";//竞争度
										std::string strAveragePrice = "";//平均价
										webdriver.FindElement(ByClass("ui-search-wraper")).FindElement(ByName("keyword")).Clear().SendKeys(StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW(strKeyword)));
										//RAND_WAIT(TIMES_2S, TIMES_1S);

										auto intputMap = webdriver.FindElements(ByName("area-type"));
										if (intputMap.size() <= 1)
										{
											continue;
										}
										intputMap.at(1).Click();
										//RAND_WAIT(TIMES_1S, TIMES_1S);
										webdriver.FindElement(ByClass("ui-search-wraper")).FindElement(ByClass("ui-search-button")).Click();
										//RAND_WAIT(TIMES_1S, TIMES_1S);
										try
										{
											bool found = false;
											try
											{
												if (webdriver.FindElement(ByClass("ui-data-empty")).GetText() == StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("暂无数据")))
												{
													//无数据
													//strLines.append("\"").append(strKeyword).append("\"").append(",").
													//	append("\"").append("").append("\"").append(",").
													//	append("\"").append("").append("\"").append(",").
													//	append("\"").append("").append("\"").append("\n");
													//found = true;
												}
											}
											catch (const std::exception&)
											{
												//有数据
												auto tr_list_selectable = webdriver.FindElements(ByClass("selectable"));
												auto tr_list_selectable_selected = webdriver.FindElements(ByClass("selectable selected"));
												if (!found)
												{
													for (auto& it : tr_list_selectable)
													{
														std::string data_id = it.GetAttribute("data-id");
														if (stricmp(data_id.c_str(), strKeyword.c_str()) == 0)
														{
															auto td_list = it.FindElements(ByTag("td"));
															if (td_list.size() >= 8)
															{
																strHotKeyword = td_list.at(1).GetText();//热度词
																strHotDegree = td_list.at(5).GetText();//热度
																strVieDegree = td_list.at(6).GetText();//竞争度
																strAveragePrice = td_list.at(7).GetText();//平均价
																StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
																strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																	append("\"").append(strKeyword).append("\"").append("\n");
																found = true;
															}
															break;
														}
													}
												}
												if (!found)
												{
													for (auto& it : tr_list_selectable_selected)
													{
														std::string data_id = it.GetAttribute("data-id");
														if (stricmp(data_id.c_str(), strKeyword.c_str()) == 0)
														{
															auto td_list = it.FindElements(ByTag("td"));
															if (td_list.size() >= 8)
															{
																strHotKeyword = td_list.at(1).GetText();//热度词
																strHotDegree = td_list.at(5).GetText();//热度
																strVieDegree = td_list.at(6).GetText();//竞争度
																strAveragePrice = td_list.at(7).GetText();//平均价
																StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
																strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																	append("\"").append(strKeyword).append("\"").append("\n");
																found = true;
															}
															break;
														}
													}
												}
												if (!found)
												{
													if (tr_list_selectable.size() > 0)
													{
														auto td_list = tr_list_selectable.at(0).FindElements(ByTag("td"));
														if (td_list.size() >= 8)
														{
															strHotKeyword = td_list.at(1).GetText();//热度词
															strHotDegree = td_list.at(5).GetText();//热度
															strVieDegree = td_list.at(6).GetText();//竞争度
															strAveragePrice = td_list.at(7).GetText();//平均价
															StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
															strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																append("\"").append(strKeyword).append("\"").append("\n");
															found = true;
														}
													}
													if (tr_list_selectable.size() > 1)
													{
														auto td_list = tr_list_selectable.at(1).FindElements(ByTag("td"));
														if (td_list.size() >= 8)
														{
															strHotKeyword = td_list.at(1).GetText();//热度词
															strHotDegree = td_list.at(5).GetText();//热度
															strVieDegree = td_list.at(6).GetText();//竞争度
															strAveragePrice = td_list.at(7).GetText();//平均价
															StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
															if (strHotKeyword.length() > 0 &&
																strHotDegree.length() > 0 &&
																strVieDegree.length() > 0 &&
																strAveragePrice.length() > 0)
															{
																strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																	append("\"").append(strKeyword).append("\"").append("\n");
																found = true;
															}
														}
													}
													if (!found)
													{
														if (tr_list_selectable_selected.size() > 0)
														{
															auto td_list = tr_list_selectable_selected.at(0).FindElements(ByTag("td"));
															if (td_list.size() >= 8)
															{
																strHotKeyword = td_list.at(1).GetText();//热度词
																strHotDegree = td_list.at(5).GetText();//热度
																strVieDegree = td_list.at(6).GetText();//竞争度
																strAveragePrice = td_list.at(7).GetText();//平均价
																StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
																strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																	append("\"").append(strKeyword).append("\"").append("\n");
																found = true;
															}
														}
														if (tr_list_selectable_selected.size() > 1)
														{
															auto td_list = tr_list_selectable_selected.at(1).FindElements(ByTag("td"));
															if (td_list.size() >= 8)
															{
																strHotKeyword = td_list.at(1).GetText();//热度词
																strHotDegree = td_list.at(5).GetText();//热度
																strVieDegree = td_list.at(6).GetText();//竞争度
																strAveragePrice = td_list.at(7).GetText();//平均价
																StringConvertUtils::Instance()->string_replace_all(strAveragePrice, "", StringConvertUtils::Instance()->WToUTF8(StringConvertUtils::Instance()->AToW("￥")));
																strLines.append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotKeyword))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strHotDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strVieDegree))).append("\"").append(",").
																	append("\"").append(StringConvertUtils::Instance()->WToA(StringConvertUtils::Instance()->UTF8ToW(strAveragePrice))).append("\"").append(",").
																	append("\"").append(strKeyword).append("\"").append("\n");
																found = true;
															}
														}
													}
												}
											}

											FILE_WRITER(strLines.data(), strLines.size(), strOutputFileName, std::ios::binary | std::ios::app);
											FILE_WRITER(std::to_string(nLastPosition + 1).data(), std::to_string(nLastPosition + 1).size(), strLastPositionFileName, std::ios::binary);
										}
										catch (const std::exception& e)
										{
											//std::cout << e.what() << std::endl;
											FILE_WRITER(std::string("**************************************************************************************\n").data(),
												std::string("**************************************************************************************\n").size(),
												strErrorInfoFileName, std::ios::binary | std::ios::app);
											FILE_WRITER((std::string("[") + __FILE__ + std::string("]") + std::string(__func__) + std::string("(") + std::to_string(__LINE__) +
												std::string("):<i=") + std::to_string(i) + ",j=" + std::to_string(j) + ",k=" + std::to_string(k) + (">") + e.what()).data(),
												(std::string("[") + __FILE__ + std::string("]") + std::string(__func__) + std::string("(") + std::to_string(__LINE__) +
													std::string("):<i=") + std::to_string(i) + ",j=" + std::to_string(j) + ",k=" + std::to_string(k) + (">") + e.what()).size(),
												strErrorInfoFileName, std::ios::binary | std::ios::app);
										}
									}

									::MessageBoxA(NULL, (std::to_string(nLastPosition + 1) + ("页记录统计已经完成!")).c_str(), ("完成提示"), MB_OK);
								}
								catch (const std::exception& e)
								{
									//std::cout << e.what() << std::endl;
									FILE_WRITER(std::string("**************************************************************************************\n").data(),
										std::string("**************************************************************************************\n").size(),
										strErrorInfoFileName, std::ios::binary | std::ios::app);
									FILE_WRITER((std::string("[") + __FILE__ + std::string("]") + std::string(__func__) + std::string("(") + std::to_string(__LINE__) +
										std::string("):<i=") + std::to_string(i) + ",j=" + std::to_string(j) + ",k=" + std::to_string(k) + (">") + e.what()).data(),
										(std::string("[") + __FILE__ + std::string("]") + std::string(__func__) + std::string("(") + std::to_string(__LINE__) +
											std::string("):<i=") + std::to_string(i) + ",j=" + std::to_string(j) + ",k=" + std::to_string(k) + (">") + e.what()).size(),
										strErrorInfoFileName, std::ios::binary | std::ios::app);
								}
							}
						}
						break;
						}
					}
					else
					{
						::MessageBoxA(NULL, ("请检查文件" + strInputFileName + "！如不存在，请创建;否则检查是否有记录。").c_str(), ("提示"), MB_OK);
					}
					break;
				}

				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0);
				((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
			}
		}).detach();

		return 0;
	}

	int start_xingxin_service(void* p, const std::string& prefix = "xingxin_main")
	{
		start_selenium_server();
		std::thread([p, prefix]() {
			if ((HWND)p != NULL)
			{
				std::string strErrorInfoFileName = GetProgramPath() + prefix + "-error_info.log";
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0xFFFFFFFF);
				while ((HWND)p != NULL)
				{
					if (GetProp((HWND)p, TEXT("RUNNING")) == (HANDLE)0)
					{
						break;
					}
					if (!AutoBingUtil::Inst()->m_bStarted)
					{
						RAND_WAIT(TIMES_3S, TIMES_1S);
						continue;
					}
					try
					{
						const std::string firstUrl = "http://www.xingxin7.net";
						auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
						firefox.Set("acceptInsecureCerts", true);
						if (AutoBingUtil::Inst()->m_bHeadless)
						{
							firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
						}
						Capabilities desired = firefox;
						Capabilities required = Capabilities();
						//desired = desired.SetProxy(HttpProxy(sv.at(rand() % sv.size()).c_str()));
						WebDriver webdriver = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);

						webdriver.Navigate(firstUrl);
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByClass("username")).SendKeys("huocaitest88");
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByClass("password")).SendKeys("aa123456");
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByClass("login")).Click();
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByClass("agrComfirm")).Click();
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByClass("team")).Click();
						RAND_WAIT(TIMES_3S, TIMES_1S);
						::MessageBoxA(NULL, ("登陆已经完成!"), ("完成提示"), MB_OK);
					}
					catch (const std::exception& e)
					{
						//std::cout << e.what() << std::endl;
						FILE_WRITER(e.what(), strlen(e.what()), strErrorInfoFileName, std::ios::binary | std::ios::app);
					}
					break;
				}

				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0);
				((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
			}
			}).detach();

			return 0;
	}

	int start_pcoptimum_service(void* p, const std::string& prefix = "pcoptimum_main")
	{
		start_selenium_server();
		std::thread([p, prefix]() {
			if ((HWND)p != NULL)
			{
				std::string strErrorInfoFileName = GetProgramPath() + prefix + "-error_info.log";
				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0xFFFFFFFF);
				while ((HWND)p != NULL)
				{
					if (GetProp((HWND)p, TEXT("RUNNING")) == (HANDLE)0)
					{
						break;
					}
					if (!AutoBingUtil::Inst()->m_bStarted)
					{
						RAND_WAIT(TIMES_3S, TIMES_1S);
						continue;
					}
					try
					{
						const std::string firstUrl = "https://www.pcoptimum.ca/login";
						auto firefox = Firefox().SetFirefoxBinary(AutoBingUtil::Inst()->m_firefoxfilepath.c_str());
						firefox.Set("acceptInsecureCerts", true);
						if (AutoBingUtil::Inst()->m_bHeadless)
						{
							firefox.Set("moz:firefoxOptions", JsonObject().Set("args", picojson::array({ picojson::value("-headless") })).Set("prefs", JsonObject()));
						}
						Capabilities desired = firefox;
						Capabilities required = Capabilities();
						//desired = desired.SetProxy(HttpProxy("52.147.88.104:3128"));
						WebDriver webdriver = Start(desired, required, AutoBingUtil::Inst()->m_webdriverurl);

						webdriver.Navigate(firstUrl);
						RAND_WAIT(TIMES_3S, TIMES_1S);
						//webdriver.FindElement(ByClass("link menu-desktop__list-item")).Click();
						RAND_WAIT(TIMES_3S, TIMES_1S);
						::MessageBoxA(NULL, ("登陆已经准备!"), ("完成提示"), MB_OK);
						webdriver.FindElement(ByName("email")).SendKeys("rgould_ppl@hotmail.com");
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByName("password")).SendKeys("Homealone1");
						RAND_WAIT(TIMES_3S, TIMES_1S);
						webdriver.FindElement(ByCss("[class='button button--block button--submit button--theme-base button--theme-dark submit-button']")).Click();
						RAND_WAIT(TIMES_3S, TIMES_1S);
						::MessageBoxA(NULL, ("登陆已经完成!"), ("完成提示"), MB_OK);
					}
					catch (const std::exception& e)
					{
						//std::cout << e.what() << std::endl;
						FILE_WRITER(e.what(), strlen(e.what()), strErrorInfoFileName, std::ios::binary | std::ios::app);
					}
					break;
				}

				SetProp((HWND)p, TEXT("RUNNING"), (HANDLE)0);
				((void (*)(HWND, BOOL))GetProp((HWND)p, TEXT("NOTIFY_STATUS")))((HWND)p, FALSE);
			}
			}).detach();

			return 0;
	}
public:
	static AutoBingUtil* Inst() {
		static AutoBingUtil autoBingUtilInstance;
		return &autoBingUtilInstance;
	}
};

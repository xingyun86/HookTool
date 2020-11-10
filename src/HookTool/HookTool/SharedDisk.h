#pragma once

#include <windows.h>
#include <winnetwk.h>
#pragma comment(lib, "mpr")

__inline static
DWORD ShareDiskToLocalDisk(
    LPCSTR lpLocalName/*="Z:"*/, //映射成本地驱动器Z:
    LPCSTR lpRemoteName/*="\\\\192.168.1.2\\SharedDocs"*/,// \\servername\共享资源名
    LPCSTR lpUserName/*="UserName"*/,//共享资源授权的用户
    LPCSTR lpPassWord/*="PassWord"*///共享资源授权用户的密码
    )
{
    DWORD dwResult = NO_ERROR;
    NETRESOURCEA netResource = { 0 };
    DWORD dwFlags = CONNECT_UPDATE_PROFILE;
    netResource.dwDisplayType = RESOURCEDISPLAYTYPE_DIRECTORY;
    netResource.dwScope = RESOURCE_CONNECTED;
    netResource.dwType = RESOURCETYPE_ANY;
    netResource.dwUsage = 0;
    netResource.lpComment = ("");
    netResource.lpLocalName = (LPSTR)lpLocalName;
    netResource.lpProvider = NULL;
    netResource.lpRemoteName = (LPSTR)lpRemoteName;
    dwResult = WNetAddConnection2A(&netResource, lpPassWord, lpUserName, dwFlags);
    switch (dwResult)
    {
    case ERROR_SUCCESS:
        ShellExecuteA(NULL, ("OPEN"), lpLocalName, NULL, NULL, SW_SHOWNORMAL);
        break;
    case ERROR_ACCESS_DENIED:
        printf("没有权访问！");
        break;
    case ERROR_ALREADY_ASSIGNED:
        ShellExecuteA(NULL, ("OPEN"), lpLocalName, NULL, NULL, SW_SHOWNORMAL);
        break;
    case ERROR_INVALID_ADDRESS:
        printf("IP地址无效");
        break;
    case ERROR_NO_NETWORK:
        printf("网络不可达!");
        break;
    }
    return dwResult;
}
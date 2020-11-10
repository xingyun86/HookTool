#pragma once

#include <windows.h>
#include <winnetwk.h>
#pragma comment(lib, "mpr")

__inline static
DWORD ShareDiskToLocalDisk(
    LPCSTR lpLocalName/*="Z:"*/, //ӳ��ɱ���������Z:
    LPCSTR lpRemoteName/*="\\\\192.168.1.2\\SharedDocs"*/,// \\servername\������Դ��
    LPCSTR lpUserName/*="UserName"*/,//������Դ��Ȩ���û�
    LPCSTR lpPassWord/*="PassWord"*///������Դ��Ȩ�û�������
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
        printf("û��Ȩ���ʣ�");
        break;
    case ERROR_ALREADY_ASSIGNED:
        ShellExecuteA(NULL, ("OPEN"), lpLocalName, NULL, NULL, SW_SHOWNORMAL);
        break;
    case ERROR_INVALID_ADDRESS:
        printf("IP��ַ��Ч");
        break;
    case ERROR_NO_NETWORK:
        printf("���粻�ɴ�!");
        break;
    }
    return dwResult;
}
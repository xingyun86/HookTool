// ScanSaveTool.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <windows.h>
#include <stdio.h>
#include <winnt.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <hidsdi.h>
#pragma comment(lib,"hid")
#include <setupapi.h>
#pragma comment(lib,"setupapi")

/*********************************************************************************************************************/
//                           模块宏定义
/*********************************************************************************************************************/
// 是否要启用重叠异步模式打开HID设备文件
#define OVERLAP_MODE         0                                                 

/*********************************************************************************************************************/
// 函数功能：主函数 - 用来列举出系统中所有可用的HID设备信息，并尝试打开他们获取其属性(有可能会打开失败)
// 输入参数：
// 输出参数：
// 返回参数：
/*********************************************************************************************************************/
__inline static
int test_main()
{
    int                       deviceNo;
    bool                      result;
    HANDLE                    hidHandle;
    GUID                      hidGuid;
    ULONG                     requiredLength;
    SP_DEVICE_INTERFACE_DATA  devInfoData;

    deviceNo = 0;
    hidHandle = NULL;
    devInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    printf("Begin to list all HID device...\r\n\r\n");

    // HidD_GetHidGuid routine returns the device interfaceGUID for HIDClass devices - from MS
    HidD_GetHidGuid(&hidGuid);

    printf("Get HID Guid: Data1[0x%X]. Data2[0x%X]. Data3[0x%X]. Data4[0x%X%X%X%X%X%X%X%X]\r\n", hidGuid.Data1, hidGuid.Data2, hidGuid.Data3,
        hidGuid.Data4[0], hidGuid.Data4[1], hidGuid.Data4[2], hidGuid.Data4[3], hidGuid.Data4[4], hidGuid.Data4[5], hidGuid.Data4[6], hidGuid.Data4[7]);

    // SetupDiGetClassDevs返回一个包含本机上所有被请求的设备信息的设备信息集句柄
    // SetupDiGetClassDevs function returns a handle to a device information set that contains requested device information elements for a local computer - from MS
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        printf("Fatal Error: SetupDiGetClassDevs Fail!!!\r\n");
        return 1;
    }

    // 首先清空错误代码，以便于后面的使用
    SetLastError(NO_ERROR);                                                    

    while (1) {

        printf("\r\ntry deviceNo %d.\r\n", deviceNo);

        // The SetupDiEnumDeviceInterfaces function enumerates the device interfaces that are contained in a device information set - from MS
        result = SetupDiEnumInterfaceDevice(hDevInfo, 0, &hidGuid, deviceNo, &devInfoData);

        if ((result == false) || (GetLastError() == ERROR_NO_MORE_ITEMS)) {    
            // 出现ERROR_NO_MORE_ITEMS错误表示已经找完了所有的设备
            printf("No More Item Left!!!\r\n\r\n");
            break;
        }
        else {
            printf("----Get devInfoData: cbSize[%d]. Flags[0x%.2X]. InterfaceClassGuid([0x%X] [0x%X] [0x%X] [0x%X%X%X%X%X%X%X%X])\r\n",
                devInfoData.cbSize, devInfoData.Flags, devInfoData.InterfaceClassGuid.Data1, devInfoData.InterfaceClassGuid.Data2, devInfoData.InterfaceClassGuid.Data3,
                devInfoData.InterfaceClassGuid.Data4[0], devInfoData.InterfaceClassGuid.Data4[1], devInfoData.InterfaceClassGuid.Data4[2], devInfoData.InterfaceClassGuid.Data4[3],
                devInfoData.InterfaceClassGuid.Data4[4], devInfoData.InterfaceClassGuid.Data4[5], devInfoData.InterfaceClassGuid.Data4[6], devInfoData.InterfaceClassGuid.Data4[7]);
        }

        // The SetupDiGetDeviceInterfaceDetail function returns details about a device interface - From MS
        // 先将变量置零，以便于下一步进行获取
        requiredLength = 0;
        // 第一次调用，为了获取requiredLength
        SetupDiGetInterfaceDeviceDetail(hDevInfo, &devInfoData, NULL, 0, &requiredLength, NULL);
        // 根据获取到的长度申请动态内存
        PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*)malloc(requiredLength);
        // 先对变量进行部分初始化
        devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
        // 第二次调用，为了获取devDetail
        result = SetupDiGetInterfaceDeviceDetail(hDevInfo, &devInfoData, devDetail, requiredLength, NULL, NULL);

        if (result == false) {
            printf("Fatal Error: SetupDiGetInterfaceDeviceDetail fail!!!\r\n");
            free(devDetail);
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return 1;
        }
        else {
            printf("----Get devDetail: cbSize[%d] DevicePath[%s]\r\n", devDetail->cbSize, devDetail->DevicePath);
        }

        if (OVERLAP_MODE == 1) {
            hidHandle = CreateFile(devDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        }
        else {
            hidHandle = CreateFile(devDetail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        }

        free(devDetail);

        // 系统会将部分HID设备设置成独占模式
        if (hidHandle == INVALID_HANDLE_VALUE) {                               
            printf("CreateFile fail!!! dev maybe in_use. continue to try next one...\r\n");
            ++deviceNo;
            continue;
        }

        _HIDD_ATTRIBUTES hidAttributes;

        // 获取HID设备的属性
        result = HidD_GetAttributes(hidHandle, &hidAttributes);               

        if (result == false) {
            printf("Fatal Error: HidD_GetAttributes fail!!!\r\n");
            CloseHandle(hidHandle);
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return 1;
        }
        else {
            printf("----Get hidAttributes: Size[%d]. VersionNumber[%d]. ProductID[0x%X]. VendorID[0x%X]\r\n",
                hidAttributes.Size, hidAttributes.VersionNumber, hidAttributes.ProductID, hidAttributes.VendorID);
        }

        CloseHandle(hidHandle);
        ++deviceNo;
    }

    // The SetupDiDestroyDeviceInfoList function deletes a device information set and frees all associated memory - From MS
    SetupDiDestroyDeviceInfoList(hDevInfo);
    printf("Search is Over!!! find %d HID_Dev altogether in your computer...\r\n\r\n", deviceNo);

    printf("任务完成，按任意键退出...\r\n");
    getchar();

    return 0;
}

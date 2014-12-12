#include "ThorlabsAPTController.h"

#include <windows.h>
#include "FTD2XX.h"
#include <iostream>

/*
   Information on communicating using FTD2xx:
   http://www.ftdichip.com/Support/Documents/ProgramGuides/D2XX_Programmer's_Guide(FT_000071).pdf
*/

void ThorlabsAPTController::GetDevices()
{
   FT_STATUS ftStatus;
   FT_DEVICE_LIST_INFO_NODE *devInfo;
   DWORD numDevs;
   // create the device information list
   ftStatus = FT_CreateDeviceInfoList(&numDevs);
   if (ftStatus == FT_OK) {
      printf("Number of devices is %d\n", numDevs);
   }
   if (numDevs > 0) {
      // allocate storage for list based on numDevs
      devInfo =
         (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
      // get the device information list
      ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
      if (ftStatus == FT_OK) {
         for (int i = 0; i < numDevs; i++) {
            printf("Dev %d:\n", i);
            printf(" Flags=0x%x\n", devInfo[i].Flags);
            printf(" Type=0x%x\n", devInfo[i].Type);
            printf(" ID=0x%x\n", devInfo[i].ID);
            printf(" LocId=0x%x\n", devInfo[i].LocId);
            printf(" SerialNumber=%s\n", devInfo[i].SerialNumber);
            printf(" Description=%s\n", devInfo[i].Description);
            printf(" ftHandle=0x%x\n", devInfo[i].ftHandle);
         }
      }
   }

}

void ThorlabsAPTController::Init()
{
   GetDevices();

   FT_STATUS status;
   ULONG baud_rate = 115200;
   int purge_dwell_time = 50;

   FT_HANDLE m_hFTDevice;

   DWORD n_dev;

   status = FT_ListDevices(&n_dev, NULL, FT_LIST_NUMBER_ONLY);
   if (status != FT_OK)
      throw;

   DWORD index;
   char Buffer[64]; // more than enough room!
   for (DWORD index = 0; index < n_dev; index++)
   {
      status = FT_ListDevices((PVOID)index, Buffer, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER);
      std::cout << Buffer << "\n";
      if (status != FT_OK)
         throw;

   }

   

   // Set baud rate to 115200.
   status = FT_SetBaudRate(m_hFTDevice, baud_rate);
   // 8 data bits, 1 stop bit, no parity
   status = FT_SetDataCharacteristics(m_hFTDevice, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
   // Pre purge dwell 50ms.
   QThread::msleep(purge_dwell_time);
   // Purge the device.
   status = FT_Purge(m_hFTDevice, FT_PURGE_RX | FT_PURGE_TX);
   // Post purge dwell 50ms.
   QThread::msleep(purge_dwell_time);
   // Reset device.
   status = FT_ResetDevice(m_hFTDevice);
   // Set flow control to RTS/CTS.
   status = FT_SetFlowControl(m_hFTDevice, FT_FLOW_RTS_CTS, 0, 0);
   // Set RTS.
   status = FT_SetRts(m_hFTDevice);
}
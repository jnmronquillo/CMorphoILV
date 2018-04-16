#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winusb.h>
#include <usb.h>
#include <cstdlib>

#include "device.h"
//#include "MORPHO_Types.h"
//#include "Errors.h"
//#include "Ilv_definitions.h"
//#include "SPIlv.h"

struct PIPE_ID
{
    UCHAR  PipeInId;
    UCHAR  PipeOutId;
    UCHAR  PipeInterruptId;
};

#define ILV_BUFFER_SIZE			10000			
#define READ_TIMEOUT			500

BOOL GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed);
BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid);
BOOL WriteToBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten);
BOOL ReadFromBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG cbSize);
//VOID testMSO(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* outID, UCHAR* inID, ULONG* pcbWritten);
/*BOOL Winusb_Send(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten, UC * i_puc_Data, UL   i_ul_DataSize);
BOOL Winusb_Receive(
    WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID,
    UC *  i_puc_Buffer,
    UL	  i_ul_BufferSize,
    //UL *  o_pul_ILVSize,
    UL i_ul_ReadTotalTimeoutConstant);
I Process_ILV_Get_Descriptor(
    WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* outID, UCHAR* inID, ULONG* pcbWritten,
    UC		i_uc_DescFormat,
    PUC		o_puc_ILV_Status,
    void *	i_pv_DescProduct,
    UL		i_ul_SizeOfDescProduct,
    void *	i_pv_DescSensor,
    UL		i_ul_SizeOfDescSensor,
    void *	i_pv_DescSoftware,
    UL		i_ul_SizeOfDescSoftware
);*/
static void print_buf(const char *title, const unsigned char *buf, size_t buf_len);
VOID testWinusbPING(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID testWinusbFINGER(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID testWinusbVERIFY(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID testWinusbVERIFY_MATCH(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID getFingerPrint(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID int2array(int i, UCHAR *arr);
VOID short2array(short i, UCHAR *arr);
BOOL sendILV(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten, UCHAR *data, int dataSize);
VOID processILV(UCHAR * buffer, int size, int offset, BOOL *imageProcessed, BOOL *pckProcessed);
void pipe_policy(DEVICE_DATA deviceData, PIPE_ID pipeid);
VOID sendVerify(DEVICE_DATA deviceData, PIPE_ID pipeid, UCHAR * reference_template_1, short L1);
VOID sendVerifyMatch(DEVICE_DATA deviceData, PIPE_ID pipeid, UCHAR * search_template, short Ls, UCHAR * reference_template_1, short L1);
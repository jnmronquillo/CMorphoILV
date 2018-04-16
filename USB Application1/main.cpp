#include "pch.h"
#include<process.h>
#include <cstring>
//#include <iostream>
//using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include "headers.h"
//#include "ppl.h"
//using namespace Concurrency;


struct myparams {
    WINUSB_INTERFACE_HANDLE hDeviceHandle;
    UCHAR* pID;
    ULONG cbSize;
    BOOL imageProcessed;
    BOOL pkcProcessed;
} myparams_obj;

UCHAR ILVCommand = 0x00;

LONG __cdecl
_tmain(
    LONG     Argc,
    LPTSTR * Argv
    )
/*++

Routine description:

    Sample program that communicates with a USB device using WinUSB

--*/
{
    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);
    DEVICE_DATA           deviceData;
    HRESULT               hr;
    USB_DEVICE_DESCRIPTOR deviceDesc;
    BOOL                  bResult;
    BOOL                  noDevice;
    ULONG                 lengthReceived;



    //system("pause");
    //
    // Find a device connected to the system that has WinUSB installed using our
    // INF
    //
    hr = OpenDevice(&deviceData, &noDevice);

    if (FAILED(hr)) {

        if (noDevice) {

            wprintf(L"Device not connected or driver not installed\n");

        }
        else {

            wprintf(L"Failed looking for device, HRESULT 0x%x\n", hr);
        }

        return 0;
    }

    //
    // Get device descriptor
    //
    bResult = WinUsb_GetDescriptor(deviceData.WinusbHandle[1],
        USB_DEVICE_DESCRIPTOR_TYPE,
        0,
        0,
        (PBYTE)&deviceDesc,
        sizeof(deviceDesc),
        &lengthReceived);

    if (FALSE == bResult || lengthReceived != sizeof(deviceDesc)) {

        wprintf(L"Error among LastError %d or lengthReceived %d\n",
            FALSE == bResult ? GetLastError() : 0,
            lengthReceived);
        CloseDevice(&deviceData);
        return 0;
    }

    //
    // Print a few parts of the device descriptor
    //
    wprintf(L"Device found: VID_%04X&PID_%04X; bcdUsb %04X\n",
        deviceDesc.idVendor,
        deviceDesc.idProduct,
        deviceDesc.bcdUSB);

    UCHAR pDeviceSpeed;
    if (!GetUSBDeviceSpeed(deviceData.WinusbHandle[0], &pDeviceSpeed))
    {
        wprintf(L"Failed to get speed");
    }
    PIPE_ID pipeid;

    for (int k = 0; k < 2; k++)
    {
        if (!QueryDeviceEndpoints(deviceData.WinusbHandle[k], &pipeid))
        {
            wprintf(L"Failed to query endpoints");
        }
    }
    //testWinusbPING(deviceData, pipeid);
    //testWinusbFINGER(deviceData, pipeid);
    //testWinusbVERIFY(deviceData, pipeid);
    testWinusbVERIFY_MATCH(deviceData, pipeid);
    //getFingerPrint(deviceData, pipeid); //just sent ilv do not read
    //pipe_policy(deviceData, pipeid);
    CloseDevice(&deviceData);

    //testMSO();
    return 0;
}

char* PATH_PKC_FILE = "C:/Users/jrojas/Pictures/2019/April/22_fingerprint.pkc";
char* PATH_IMAGE_FILE = "C:/Users/jrojas/Pictures/2019/April/22_fingerprint.raw";
char* PATH_BMP_FILE = "C:/Users/jrojas/Pictures/2019/April/22_fingerprint.bmp";

//char* PATH_PKC_FILE_VERIFY = "C:/Users/jrojas/Documents/jm/fuentes_zt/pruebas/Windows-Biometric-Framework-FingerPrint-Example/data/template_2018.4.11.20.43.20.597.pkc";
char* PATH_PKC_FILE_VERIFY = "C:/Users/jrojas/Pictures/2019/April/1_fingerprint.pkc";
char* PATH_PKC_FILE_VERIFY_MATCH = "C:/Users/jrojas/Pictures/2019/April/2_fingerprint.pkc";

void pipe_policy(DEVICE_DATA deviceData, PIPE_ID pipeid)
{
    ULONG value = NULL;
    ULONG size = sizeof(ULONG);
    //https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/winusb-functions-for-pipe-policy-modification
    //PIPE_TRANSFER_TIMEOUT
    BOOL bResult = WinUsb_GetPipePolicy(deviceData.WinusbHandle[1], pipeid.PipeInId, MAXIMUM_TRANSFER_SIZE, &size, &value);
    if (!bResult) {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        printf("failed read policy %lu", hr);
    }
    else {
        printf("policy value %d, size: %d", value, size);
        //printf("policy size: %d", size);
    }
}


unsigned int __stdcall  read_fingerprint(void* data)
{
    myparams deviceData = *((myparams*)data);

    if (deviceData.hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    ULONG timeout;
    timeout = 500;
    BOOL bResult = WinUsb_SetPipePolicy(deviceData.hDeviceHandle, *deviceData.pID, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

    if (FALSE == bResult) {

        //hr = HRESULT_FROM_WIN32(GetLastError());        
        //return hr;
        //printf(HRESULT_FROM_WIN32(GetLastError()));
        return FALSE;
    }

    UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*deviceData.cbSize);


    ULONG cbRead = 0;    

    //BOOL imageProcessed = FALSE;
    //BOOL pkcProcessed = FALSE;
        
    int counterFailed = 0;
    //while (!imageProcessed && counterFailed < 10) {//when image is processed stop//20:arbitrary
    while (!(deviceData.imageProcessed && deviceData.pkcProcessed) && counterFailed < 10) {
        cbRead = 0;

        bResult = WinUsb_ReadPipe(deviceData.hDeviceHandle, *deviceData.pID, szBuffer, deviceData.cbSize, &cbRead, 0);
        if (!bResult)
        {
            counterFailed++;
            //break;
            continue;
        }
        counterFailed = 0;
        printf("\n cbRead: %d \n", cbRead);

        
        if (cbRead < 6)
        {
            printf("Communication error with biometric reader");
            continue;
        }
        if (szBuffer[0] == 0x53 && szBuffer[1] == 0x59 && szBuffer[2] == 0x4E && szBuffer[3] == 0x43)//Header found, start reading
        {
            printf("\n'SYNC' found\n");
            if (szBuffer[cbRead - 1] == 0x4E && szBuffer[cbRead - 2] == 0x45)
            {
                printf("\nEnd found, process complete chunk\n");
                processILV(szBuffer, cbRead, 12, &deviceData.imageProcessed, &deviceData.pkcProcessed);    //12 : DataFrame (SYNC + Lenght(4 bytes) + Len'complement(b bytes))            
            }
            else//Else, start new reading buffer
            {
                printf("\n serial reading not implemented\n");                
            }
        }        

    }

    //print_buf("\nRead from Pipe IN", szBuffer, cbRead);

    LocalFree(szBuffer);
    return 0;
}

unsigned int __stdcall  read_fingerprint_serial(void* data)
{
    myparams deviceData = *((myparams*)data);

    if (deviceData.hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    } 

    ULONG timeout;
    timeout = 500;
    BOOL bResult = WinUsb_SetPipePolicy(deviceData.hDeviceHandle, *deviceData.pID, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

    if (FALSE == bResult) {

        //hr = HRESULT_FROM_WIN32(GetLastError());        
        //return hr;
        //printf(HRESULT_FROM_WIN32(GetLastError()));
        return FALSE;
    }

    UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*deviceData.cbSize);

    
    ULONG cbRead = 0;
    ULONG serialDataSize = 0;
    BOOL serialReading = FALSE;
    BOOL serialErrorReported = FALSE;
    UCHAR *serialData = NULL;

    BOOL imageProcessed = FALSE;
    BOOL pckProcessed = FALSE;

    //int i = 0;
    while (!imageProcessed && !pckProcessed) {//instead of an infinite loop
        /*i++;
        if (i > 30) {
            if (!serialReading) {
                break;
            }
            else {
                i = 0;
            }
        }*/
        

        cbRead = 0;
        
        bResult = WinUsb_ReadPipe(deviceData.hDeviceHandle, *deviceData.pID, szBuffer, deviceData.cbSize, &cbRead, 0);
        if (!bResult)
        {          
            continue;
        }
        printf("\n cbRead: %d \n", cbRead);        

        if (!serialReading)//Start reading buffer
        {
            if (cbRead < 6)
            {
                printf("Communication error with biometric reader");
                continue;
            }
            if (szBuffer[0] == 0x53 && szBuffer[1] == 0x59 && szBuffer[2] == 0x4E && szBuffer[3] == 0x43)//Header found, start reading
            {
                printf("\n'SYNC' found\n");
                if (szBuffer[cbRead - 1] == 0x4E && szBuffer[cbRead - 2] == 0x45)
                {
                    printf("\nEnd found, process complete chunk\n");
                    processILV(szBuffer, cbRead, 12, &imageProcessed, &pckProcessed);
                    //out_q.put(self.processILV(data, len(data), 12))
                    //break;
                }
                else//Else, start new reading buffer
                {
                    printf("\nstart serial reading\n");
                    serialData = (UCHAR*)malloc(serialDataSize = cbRead);
                    memcpy(serialData, szBuffer, cbRead);
                    serialReading = TRUE;
                }
                serialErrorReported = FALSE;
            }
            else
            {
                if (!serialErrorReported)
                {
                    printf("'SYNC' not found");
                    serialErrorReported = TRUE;                    
                }
            }

        }
        else
        {
            serialData = (UCHAR*)realloc(serialData, serialDataSize += cbRead);
            memcpy(serialData + serialDataSize - cbRead, szBuffer, cbRead);

            if (szBuffer[cbRead - 1] == 0x4E && szBuffer[cbRead - 2] == 0x45)//End found, read completed
            {
                printf("\n reading ended\n");
                processILV(serialData, serialDataSize, 12, &imageProcessed, &pckProcessed);
                //out_q.put(self.processILV(serialData, len(serialData), 12))
                serialReading = FALSE;
                //break;
            }
        }


    }

    printf("dataSize: %d", serialDataSize);
    printf("serialReading: %d", serialReading);
    //const int siz_ar = sizeof(szBuffer) / sizeof(UCHAR);
    //print_buf("\nRead from Pipe IN", serialData, serialDataSize);

    LocalFree(szBuffer);
    free(serialData);
    return 0;
}

unsigned int __stdcall  read_ping(void* data)
{
    myparams deviceData = *((myparams*)data);
    
    if (deviceData.hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    
    UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*deviceData.cbSize);

    ULONG cbRead = 0;

    ULONG timeout;
    timeout = 2000;//500;
    BOOL bResult = WinUsb_SetPipePolicy(deviceData.hDeviceHandle, *deviceData.pID, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

    if (FALSE == bResult) {

        //hr = HRESULT_FROM_WIN32(GetLastError());
        //CloseHandle(DeviceData->DeviceHandle);
        //return hr;
        //printf(HRESULT_FROM_WIN32(GetLastError()));
        return FALSE;
    }

    bResult = WinUsb_ReadPipe(deviceData.hDeviceHandle, *deviceData.pID, szBuffer, deviceData.cbSize, &cbRead, 0);
    if (!bResult)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());

        //goto done;
        if (FAILED(hr)) {
            LocalFree(szBuffer);
            printf("\nFailed to Read from Pipe IN");
            return 0;
        }

    }

    //const int siz_ar = sizeof(szBuffer) / sizeof(UCHAR);
    print_buf("\nRead from Pipe IN", szBuffer, cbRead);

    //printf("Read from pipe %d: %s \nActual data read: %d.\n", *pID, szBuffer, cbRead);


//done:
    LocalFree(szBuffer);
    
    //int i;

    // Most compiler won't eliminate the loop
    // since i is volatile
    //printf("j: %d", j.cbSize);
    //for (i = 0; i < 10; i++) {
    //    printf("i: %d", i);
    //}

    return 0;
}

VOID sendVerify(DEVICE_DATA deviceData, PIPE_ID pipeid, UCHAR * reference_template_1, short L1)
{
    short longitud = 5 + (3 + L1) + 4;
    UCHAR espera = 0x07;//Finger Detection Timeout (seconds): When set to 0x0000, the timeout check is disabled (infinite timeout)
    short matching_threshold = 5;//Value range: 1 to 9. Recommanded value: 5
    UCHAR template_format = 0x02;//ID_PKCOMP (0x02)

    //util vars
    UCHAR idEvent = 0x00;
    short size = 0;
    UCHAR command = 0x00;
    UCHAR arr_idEvent[] = { 0x00 };

    //0x20 : ILV_VERIFY 
    //I
    int cbSize = 1;
    UCHAR *data = (UCHAR*)malloc(cbSize);
    UCHAR arr_start[] = { 0x20 };
    memcpy(data, arr_start, 1);
    //L
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_longitud[2];
    short2array(longitud, arr_longitud);
    memcpy(data + cbSize - 2, arr_longitud, 2);
    //V
    //Finger Detection Timeout 
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_espera[2];
    short2array(espera, arr_espera);
    memcpy(data + cbSize - 2, arr_espera, 2);
    //Matching Threshold 
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_threshold[2];
    short2array(matching_threshold, arr_threshold);
    memcpy(data + cbSize - 2, arr_threshold, 2);
    //Acquisition Quality Threshold : Ignored
    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_base[] = { 0x00 };
    memcpy(data + cbSize - 1, arr_base, 1);
    //Reference Template # 1 
    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_format[] = { template_format };
    memcpy(data + cbSize - 1, arr_format, 1);
    
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_size[2];
    short2array(L1, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += L1);
    memcpy(data + cbSize - L1, reference_template_1, L1);

    //Asynchronous event ILV
    /*idEvent = 0x34; //ID_ASYNCHRONOUS_EVENT
    size = 4;
    command = 0x03; //(COMMAND) | (IMAGE) // | ¿(CODE_QUALITY)? // Command Message and Image Message

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 4);
    UCHAR arr_command_four[] = { command, 0x00, 0x00, 0x00 };
    memcpy(data + cbSize - 4, arr_command_four, 4);*/

    //Matching Score required ILV
    idEvent = 0x56; //ID_MATCHING_SCORE
    size = 1;
    command = 0x01; //other than 0x00 (default): Return of Matching score value is required

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_command_one[] = { command };
    memcpy(data + cbSize - 1, arr_command_one, 1);

    //sendILV
    ULONG pcbWritten;
    
    if (!sendILV(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten, data, cbSize))
    {
        wprintf(L"Failed to write to outPipe");
    }
    else {

    }
    free(data);

}

VOID sendVerifyMatch(DEVICE_DATA deviceData, PIPE_ID pipeid, UCHAR * search_template, short Ls, UCHAR * reference_template_1, short L1)
{
    short longitud = 2 + (3 + Ls) + (3 + L1) + 4;
    short matching_threshold = 5;//Value range: 1 to 9. Recommanded value: 5
    UCHAR template_format = 0x02;//ID_PKCOMP (0x02)

    //util vars
    UCHAR idEvent = 0x00;
    short size = 0;
    UCHAR command = 0x00;
    UCHAR arr_idEvent[] = { 0x00 };

    //0x23 : ILV_VERIFY_MATCH
    //I
    int cbSize = 1;
    UCHAR *data = (UCHAR*)malloc(cbSize);
    UCHAR arr_start[] = { 0x23 };
    memcpy(data, arr_start, 1);
    //L
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_longitud[2];
    short2array(longitud, arr_longitud);
    memcpy(data + cbSize - 2, arr_longitud, 2);
    //V
    //Matching Threshold 
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_threshold[2];
    short2array(matching_threshold, arr_threshold);
    memcpy(data + cbSize - 2, arr_threshold, 2);
    
    //Search Template 
    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_format[] = { template_format };
    memcpy(data + cbSize - 1, arr_format, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_size[2];
    short2array(Ls, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += Ls);
    memcpy(data + cbSize - Ls, search_template, Ls);

    //Reference Template # 1 
    data = (UCHAR*)realloc(data, cbSize += 1);
    //UCHAR arr_format[] = { template_format };
    arr_format[0] = template_format;
    memcpy(data + cbSize - 1, arr_format, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(L1, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += L1);
    memcpy(data + cbSize - L1, reference_template_1, L1);
    
    //Matching Score required ILV
    idEvent = 0x56; //ID_MATCHING_SCORE
    size = 1;
    command = 0x01; //other than 0x00 (default): Return of Matching score value is required

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_command_one[] = { command };
    memcpy(data + cbSize - 1, arr_command_one, 1);

    //sendILV
    ULONG pcbWritten;

    if (!sendILV(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten, data, cbSize))
    {
        wprintf(L"Failed to write to outPipe");
    }
    else {

    }
    free(data);

}

VOID getFingerPKCOMP(DEVICE_DATA deviceData, PIPE_ID pipeid)
{
    short longitud = 8 + 4 + 9 + 4;
    UCHAR base = 0x00;//Database Identifier
    UCHAR espera = 0x07;//Finger Detection Timeout (seconds): wait for the first detection of a finger
    UCHAR calidad = 0x00;//Acquisition Quality Threshold // 0: Fingerprint quality threshold is 20
    UCHAR pasadas = 0x03;//Enrollment Type : Number of acquisitions per finger.
    UCHAR dedos = 0x01;//Number of Fingers 
    UCHAR guardar = 0x00;//Create a Record 
    UCHAR tamano = 0x01;//Export Minutiae Size 

    //util vars
    UCHAR idEvent = 0x00;
    short size = 0;
    UCHAR arr_size[2];
    UCHAR arr_idEvent[] = { 0x00 };
    UCHAR command = 0x00;
    
    //0x21 : ILV_ENROLL 
    //I
    int cbSize = 1;
    UCHAR *data = (UCHAR*)malloc(cbSize);
    UCHAR arr_start[] = { 0x21 };
    memcpy(data, arr_start, 1);
    //L
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_longitud[2];
    short2array(longitud, arr_longitud);
    memcpy(data + cbSize - 2, arr_longitud, 2);
    
    //V
    
    //Database Identifier (8 bytes)
    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_base[] = { base };
    memcpy(data + cbSize - 1, arr_base, 1);
    //Finger Detection Timeout 
    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_espera[2];
    short2array(espera, arr_espera);
    memcpy(data + cbSize - 2, arr_espera, 2);

    data = (UCHAR*)realloc(data, cbSize += 5);
    UCHAR arr_param[] = { calidad, pasadas, dedos, guardar, tamano };
    memcpy(data + cbSize - 5, arr_param, 5);

    //Asynchronous event ILV (7 bytes)
    /*idEvent = 0x34; //ID_ASYNCHRONOUS_EVENT
    size = 4;
    command = 0x03; //(COMMAND) | (IMAGE) // | ¿(CODE_QUALITY)? // Command Message and Image Message

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 4);
    UCHAR arr_command_four[] = { command, 0x00, 0x00, 0x00 };
    memcpy(data + cbSize - 4, arr_command_four, 4);*/

    //Biometric Algorithm Parameter (Parameter of ILV_ENROLL):
    //requires Export minutiae size >= 0x01
    //(4 bytes)
    idEvent = 0x38;//1i
    size = 1;//L
    UCHAR TemplateFormat = 0x00;//V: Fingerprint Template Format //0x00: PK_COMP V2 

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_TemplateFormat[] = { TemplateFormat };
    memcpy(data + cbSize - 1, arr_TemplateFormat, 1);

    //Export Image ILV(4 bytes)
    idEvent = 0x3D; //ID_EXPORT_IMAGE
    size = 6;
    UCHAR imageType = 0x00; //ID_DEFAULT_IMAGE

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;    
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_imageType[] = { imageType };
    memcpy(data + cbSize - 1, arr_imageType, 1);

    //Export Image ILV : Compression ILV(5 bytes)
    idEvent = 0x3E; //ID_COMPRESSION
    size = 2;
    command = 0x2C; //Compression Type// ID_COMPRESSION_NULL:The image is returned without any compression

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_command_two[] = { command, 0x00 };
    memcpy(data + cbSize - 2, arr_command_two, 2);
    
    //Latent fingerprint ILV
    //required when Enrollment Type = 1
    //(4 bytes)
    idEvent = 0x39; // ID_LATENT_SETTING
    size = 1;
    command = 0x01; // ENABLED

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_command_one[] = { command };
    memcpy(data + cbSize - 1, arr_command_one, 1);

    //sendILV
    ULONG pcbWritten;
    if (!sendILV(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten, data, cbSize))
    {
        wprintf(L"Failed to write to outPipe");
    }
    else {

    }
    
    free(data);
}


VOID getFingerPrint(DEVICE_DATA deviceData, PIPE_ID pipeid)
{
    short longitud = 8 + 9 + 4;
    UCHAR base = 0x00;//Database Identifier
    UCHAR espera = 0x07;//Finger Detection Timeout (seconds): wait for the first detection of a finger
    UCHAR calidad = 0x00;//Acquisition Quality Threshold // 0: Fingerprint quality threshold is 20
    UCHAR pasadas = 0x03;//Enrollment Type : Number of acquisitions per finger.
    UCHAR dedos = 0x01;//Number of Fingers 
    UCHAR guardar = 0x00;//Create a Record 
    UCHAR tamano = 0x00;//Export Minutiae Size 

    //util vars
    UCHAR idEvent = 0x00;
    short size = 0;
    UCHAR arr_size[2];
    UCHAR arr_idEvent[] = { 0x00 };
    UCHAR command = 0x00;

    //Image
    //0x21 : ILV_ENROLL
    int cbSize = 1;
    UCHAR *data = (UCHAR*)malloc(cbSize);
    UCHAR arr_start[] = { 0x21 };
    memcpy(data, arr_start, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_longitud[2];
    short2array(longitud, arr_longitud);
    memcpy(data + cbSize - 2, arr_longitud, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_base[] = { base };
    memcpy(data + cbSize - 1, arr_base , 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_espera[2];
    short2array(espera, arr_espera);
    memcpy(data + cbSize - 2, arr_espera, 2);

    data = (UCHAR*)realloc(data, cbSize += 5);
    UCHAR arr_param[] = { calidad, pasadas, dedos, guardar, tamano };
    memcpy(data + cbSize - 5, arr_param, 5);

    //Asynchronous event ILV (7 bytes)
    /*idEvent = 0x34; //ID_ASYNCHRONOUS_EVENT
    size = 4;
    command = 0x03; //(COMMAND) | (IMAGE) // | ¿(CODE_QUALITY)? // Command Message and Image Message

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_idEvent[] = { idEvent };
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 4);
    UCHAR arr_command_four[] = { command, 0x00, 0x00, 0x00 };
    memcpy(data + cbSize - 4, arr_command_four, 4);*/

    //Biometric Algorithm Parameter:
    //requires Export minutiae size >= 0x01

    //(4 bytes)
    /*idEvent = 0x38;//1i
    size = 1;//L
    UCHAR TemplateFormat = 0x00;//V: Fingerprint Template Format //0x00: PK_COMP V2

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_TemplateFormat[] = { TemplateFormat };
    memcpy(data + cbSize - 1, arr_TemplateFormat, 1);*/

    //Export Image ILV
    idEvent = 0x3D; //ID_EXPORT_IMAGE
    size = 6;
    UCHAR imageType = 0x00; //ID_DEFAULT_IMAGE

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_imageType[] = { imageType };
    memcpy(data + cbSize - 1, arr_imageType, 1);

    //Export Image ILV : Compression ILV
    idEvent = 0x3E; //ID_COMPRESSION
    size = 2;
    command = 0x2C; //Compression Type// ID_COMPRESSION_NULL:The image is returned without any compression

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 2);
    UCHAR arr_command_two[] = { command, 0x00 };
    memcpy(data + cbSize - 2, arr_command_two, 2);

    //Latent fingerprint ILV
    //required when Enrollment Type = 1
    idEvent = 0x39; // ID_LATENT_SETTING
    size = 1;
    command = 0x01; // ENABLED

    data = (UCHAR*)realloc(data, cbSize += 1);
    arr_idEvent[0] = idEvent;
    memcpy(data + cbSize - 1, arr_idEvent, 1);

    data = (UCHAR*)realloc(data, cbSize += 2);
    //UCHAR arr_size[2];
    short2array(size, arr_size);
    memcpy(data + cbSize - 2, arr_size, 2);

    data = (UCHAR*)realloc(data, cbSize += 1);
    UCHAR arr_command_one[] = { command };
    memcpy(data + cbSize - 1, arr_command_one, 1);
    
    //sendILV
    ULONG pcbWritten;
    if (!sendILV(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten, data, cbSize))
    {
        wprintf(L"Failed to write to outPipe");
    }
    else {

    }
    
    free(data);
}

VOID testWinusbVERIFY(DEVICE_DATA deviceData, PIPE_ID pipeid) {

    //UCHAR data[] = { 0x08, 0x01, 0x00, 0x02 };

    ///ULONG pcbWritten;

    //The size of the returned image depends on the MorphoSmart™ device:
    //  416 x 416 pixels for the MorphoSmart™ MSO(173056 bytes when not compressed)
    //  400 x 256 pixels for the MorphoSmart™ CBM(102400 bytes when not compressed)
    //  400 x 400 pixels for the MorphoSmart™ FINGER VP(160000 bytes when not compressed)
    //VERIFY
    //The MorphoSmart™ FINGER VP does not support fingerprint reference template without a specific license. For more information, please contact Idemia.

    //reading start before writting
    myparams_obj.hDeviceHandle = deviceData.WinusbHandle[1];
    myparams_obj.pID = &pipeid.PipeInId;
    myparams_obj.cbSize = 262144;//MAXIMUM_TRANSFER_SIZE Winusb policy PipeIn:262144 // 102446;//2048;//1024;// MAX_DATA_SIZE//mas o menos bytes a leer (siempre lee)
    myparams_obj.imageProcessed = FALSE;
    myparams_obj.pkcProcessed = FALSE;

    HANDLE myhandleB;
    myhandleB = (HANDLE)_beginthreadex(0, 0, &read_fingerprint, &myparams_obj, 0, 0);

    //writting
    //getFingerPrint(deviceData, pipeid);
    //getFingerPKCOMP(deviceData, pipeid);
    
    CHAR * l_cs_File = PATH_PKC_FILE_VERIFY;

    //CFile raw(l_cs_File);
    //raw.write(huella, lenght);
    //raw.close();

    std::ifstream fin(l_cs_File);
    // get pointer to associated buffer object
    std::filebuf* pbuf = fin.rdbuf();
    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff(0, fin.end, fin.in);
    printf("\n size: %d \n", (int)size);
    pbuf->pubseekpos(0, fin.in);
    // allocate memory to contain file data
    UCHAR* buffer = new UCHAR[size];
    // get file data
    pbuf->sgetn(reinterpret_cast<char *>(buffer), size);
    fin.close();


    sendVerify(deviceData, pipeid, buffer, (short)size);

    //close reading
    WaitForSingleObject(myhandleB, INFINITE);
    CloseHandle(myhandleB);
}

VOID testWinusbVERIFY_MATCH(DEVICE_DATA deviceData, PIPE_ID pipeid) {

    //UCHAR data[] = { 0x08, 0x01, 0x00, 0x02 };

    ///ULONG pcbWritten;

    //The size of the returned image depends on the MorphoSmart™ device:
    //  416 x 416 pixels for the MorphoSmart™ MSO(173056 bytes when not compressed)
    //  400 x 256 pixels for the MorphoSmart™ CBM(102400 bytes when not compressed)
    //  400 x 400 pixels for the MorphoSmart™ FINGER VP(160000 bytes when not compressed)
    //VERIFY
    //The MorphoSmart™ FINGER VP does not support fingerprint reference template without a specific license. For more information, please contact Idemia.

    //reading start before writting
    myparams_obj.hDeviceHandle = deviceData.WinusbHandle[1];
    myparams_obj.pID = &pipeid.PipeInId;
    myparams_obj.cbSize = 262144;//MAXIMUM_TRANSFER_SIZE Winusb policy PipeIn:262144 // 102446;//2048;//1024;// MAX_DATA_SIZE//mas o menos bytes a leer (siempre lee)
    myparams_obj.imageProcessed = FALSE;
    myparams_obj.pkcProcessed = FALSE;

    HANDLE myhandleB;
    myhandleB = (HANDLE)_beginthreadex(0, 0, &read_fingerprint, &myparams_obj, 0, 0);

    //writting
    //getFingerPrint(deviceData, pipeid);
    //getFingerPKCOMP(deviceData, pipeid);

    CHAR * l_cs_File = PATH_PKC_FILE_VERIFY;
    CHAR * l_cs_File_2 = PATH_PKC_FILE_VERIFY_MATCH;

    //CFile raw(l_cs_File);
    //raw.write(huella, lenght);
    //raw.close();

    std::ifstream fin(l_cs_File);
    // get pointer to associated buffer object
    std::filebuf* pbuf = fin.rdbuf();
    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff(0, fin.end, fin.in);
    printf("\n size: %d \n", (int)size);
    pbuf->pubseekpos(0, fin.in);
    // allocate memory to contain file data
    UCHAR* buffer = new UCHAR[size];
    // get file data
    pbuf->sgetn(reinterpret_cast<char *>(buffer), size);
    fin.close();

    std::ifstream fin_2(l_cs_File_2);
    // get pointer to associated buffer object
    std::filebuf* pbuf_2 = fin_2.rdbuf();
    // get file size using buffer's members
    std::size_t size_2 = pbuf_2->pubseekoff(0, fin_2.end, fin_2.in);
    printf("\n size: %d \n", (int)size_2);
    pbuf_2->pubseekpos(0, fin_2.in);
    // allocate memory to contain file data
    UCHAR* buffer_2 = new UCHAR[size_2];
    // get file data
    pbuf_2->sgetn(reinterpret_cast<char *>(buffer_2), size_2);
    fin_2.close();


    sendVerifyMatch(deviceData, pipeid, buffer, (short)size, buffer_2, (short)size_2);

    //close reading
    WaitForSingleObject(myhandleB, INFINITE);
    CloseHandle(myhandleB);
}

VOID testWinusbFINGER(DEVICE_DATA deviceData, PIPE_ID pipeid) {

    //UCHAR data[] = { 0x08, 0x01, 0x00, 0x02 };

    ///ULONG pcbWritten;

    //The size of the returned image depends on the MorphoSmart™ device:
    //  416 x 416 pixels for the MorphoSmart™ MSO(173056 bytes when not compressed)
    //  400 x 256 pixels for the MorphoSmart™ CBM(102400 bytes when not compressed)
    //  400 x 400 pixels for the MorphoSmart™ FINGER VP(160000 bytes when not compressed)
    //VERIFY
    //The MorphoSmart™ FINGER VP does not support fingerprint reference template without a specific license. For more information, please contact Idemia.

    //reading start before writting
    myparams_obj.hDeviceHandle = deviceData.WinusbHandle[1];
    myparams_obj.pID = &pipeid.PipeInId;
    myparams_obj.cbSize = 262144;//MAXIMUM_TRANSFER_SIZE Winusb policy PipeIn:262144 // 102446;//2048;//1024;// MAX_DATA_SIZE//mas o menos bytes a leer (siempre lee)
    myparams_obj.imageProcessed = FALSE;
    myparams_obj.pkcProcessed = FALSE; // TRUE: only will capture an image

    HANDLE myhandleB;
    myhandleB = (HANDLE)_beginthreadex(0, 0, &read_fingerprint, &myparams_obj, 0, 0);

    //writting
    //getFingerPrint(deviceData, pipeid);//only capture an image
    getFingerPKCOMP(deviceData, pipeid);//capture image and pkc
    
    //close reading
    WaitForSingleObject(myhandleB, INFINITE);
    CloseHandle(myhandleB);
}

VOID testWinusbPING(DEVICE_DATA deviceData, PIPE_ID pipeid) {
    

    //PUC		l_puc_Buffer;

    //l_puc_Buffer = (PUC)malloc(ILV_BUFFER_SIZE);
    //if (!l_puc_Buffer)
    //{
    //    printf("Cannot allocate l_puc_Buffer\r\n");
    //    return ILV_ERROR_NOT_ENOUGH_MEMORY;
    //}

    //PING
    UCHAR data[] = { 0x08, 0x01, 0x00, 0x02 };

    ULONG pcbWritten;
    //testMSO(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pipeid.PipeInId, &pcbWritten);

    myparams_obj.hDeviceHandle = deviceData.WinusbHandle[1];
    myparams_obj.pID = &pipeid.PipeInId;
    myparams_obj.cbSize = 19 + 2;//mas o menos bytes a leer (siempre lee)

    HANDLE myhandleB;
    myhandleB = (HANDLE)_beginthreadex(0, 0, &read_ping, &myparams_obj, 0, 0);

    //if (!WriteToBulkEndpoint(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten))
    if(!sendILV(deviceData.WinusbHandle[1], &pipeid.PipeOutId, &pcbWritten, data, 4))
    {
        wprintf(L"Failed to write to outPipe");
    }
    else {
        
    }    
    WaitForSingleObject(myhandleB, INFINITE);
    CloseHandle(myhandleB);

    //else {

    //    if (!ReadFromBulkEndpoint(deviceData.WinusbHandle[1], &pipeid.PipeInId, 19))
      //  {
        //    wprintf(L"\nFailed to read from inPipe");
        //}

    //}
    

}

/*
VOID testMSO(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* outID, UCHAR* inID, ULONG* pcbWritten)
{
    I			l_i_Ret;
    UC			l_uc_ILV_status;
    C			l_ac_product_desc[1024];
    C			l_ac_sensor_desc[1024];
    C			l_ac_software_desc[1024];

    // Get MSO descriptor
    l_i_Ret = Process_ILV_Get_Descriptor(
        hDeviceHandle, outID, inID, pcbWritten,
        ID_FORMAT_TEXT,
        &l_uc_ILV_status,
        l_ac_product_desc, 1024,
        l_ac_sensor_desc, 1024,
        l_ac_software_desc, 1024);

    if (l_i_Ret)
    {
        printf("Process_ILV_Get_Descriptor ret %d", l_i_Ret);
        return;
    }

    // Display descriptor
    printf("Product Descriptor :\r\n");
    printf(l_ac_product_desc);
    printf("\r\n\r\n");
    printf("Sensor Descriptor :\r\n");
    printf(l_ac_sensor_desc);
    printf("\r\n\r\n");
    printf("Software Descriptor :\r\n");
    printf(l_ac_software_desc);
    printf("\r\n\r\n");
}

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
)
{
    I		l_i_Ret;
    PUC		l_puc_Buffer;
    UL		l_ul_ILVSize;
//    UL		l_ul_L;
    //UC*		l_puc_V;
//   UC		l_uc_I_Desc;
//    UL		l_ul_L_Desc;
 //   PUC		l_puc_V_Desc;

    // Initialize 
    *o_puc_ILV_Status = ILV_OK;
    l_i_Ret = RETURN_NO_ERROR;

    if (i_pv_DescProduct)
        memset(i_pv_DescProduct, 0, i_ul_SizeOfDescProduct);

    if (i_pv_DescSensor)
        memset(i_pv_DescSensor, 0, i_ul_SizeOfDescSensor);

    if (i_pv_DescSoftware)
        memset(i_pv_DescSoftware, 0, i_ul_SizeOfDescSoftware);

    l_puc_Buffer = (PUC)malloc(ILV_BUFFER_SIZE);
    if (!l_puc_Buffer)
    {
        printf("Cannot allocate l_puc_Buffer\r\n");
        return ILV_ERROR_NOT_ENOUGH_MEMORY;
    }

    l_ul_ILVSize = 0;

    // Build request ILV
    l_i_Ret = ILV_Init(l_puc_Buffer, &l_ul_ILVSize, ILV_GET_DESCRIPTOR);
    if (l_i_Ret)
    {
        printf("ILV_Init ret 0x%08X\r\n", l_i_Ret);
        free(l_puc_Buffer);
        return l_i_Ret;
    }

    l_i_Ret = ILV_AddValue(l_puc_Buffer, &l_ul_ILVSize, &i_uc_DescFormat, sizeof(UC));
    if (l_i_Ret)
    {
        printf("ILV_AddValue ret 0x%08X\rn", l_i_Ret);
        free(l_puc_Buffer);
        return l_i_Ret;
    }

    if (!Winusb_Send(hDeviceHandle, outID, pcbWritten, l_puc_Buffer, l_ul_ILVSize))
    {
        free(l_puc_Buffer);
        wprintf(L"Failed to write to outPipe");
        return 1;
    }

    if (!Winusb_Receive(hDeviceHandle, inID, l_puc_Buffer, ILV_BUFFER_SIZE, READ_TIMEOUT))
    {
        free(l_puc_Buffer);
        wprintf(L"Failed to receive from inPipe");
        return 1;
    }
    ////
    return 0;
}

BOOL Winusb_Send(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten, UC * i_puc_Data, UL   i_ul_DataSize)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE || !pID || !pcbWritten)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    //UCHAR szBuffer[] = "Hello World";
    //ULONG cbSize = (ULONG)strlen("Hello World");
    //UCHAR szBuffer[3];
    //ULONG cbSize = 3;

    ULONG cbSent = 0;

    //szBuffer[0] = 0x08;
    //szBuffer[1] = 0x00;
    //szBuffer[2] = 0x00;

    bResult = WinUsb_WritePipe(hDeviceHandle, *pID, i_puc_Data, i_ul_DataSize, &cbSent, 0);
    if (!bResult)
    {
        goto done;
    }

    printf("Wrote to pipe %d: %s \nActual data transferred: %d.\n", *pID, i_puc_Data, cbSent);
    *pcbWritten = cbSent;


done:
    return bResult;

}

BOOL Winusb_Receive(
    WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID,
    UC *  i_puc_Buffer,
    UL	  i_ul_BufferSize,
    //UL *  o_pul_ILVSize,
    UL i_ul_ReadTotalTimeoutConstant)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    //UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*cbSize);
    //UCHAR szBuffer[4];


    ULONG cbRead = 0;

    //ULONG timeout;
    //timeout = 10;
    bResult = WinUsb_SetPipePolicy(hDeviceHandle, *pID, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &i_ul_ReadTotalTimeoutConstant);

    if (FALSE == bResult) {

        //hr = HRESULT_FROM_WIN32(GetLastError());
        //CloseHandle(DeviceData->DeviceHandle);
        //return hr;
        //printf(HRESULT_FROM_WIN32(GetLastError()));
        return FALSE;
    }

    //ZeroMemory(i_puc_Buffer, i_ul_BufferSize);
    //UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*i_ul_BufferSize);
    int timeCounter = 0;

    do {

        bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, i_puc_Buffer, i_ul_BufferSize, &cbRead, 0);
        if (!bResult)
        {
            HRESULT hr = HRESULT_FROM_WIN32(GetLastError());

            //goto done;
            if (FAILED(hr)) {
                printf("Read from pipe %d: %s \nActual data read: %d.\n", *pID, i_puc_Buffer, cbRead);
                //return FALSE;
            }
            else {
                printf("Read from pipe %d: %s \nActual data read: %d.\n", *pID, i_puc_Buffer, cbRead);
                break;
            }

        }
        timeCounter++;
    } while (!bResult && timeCounter < 10);
  

    //done:
        //LocalFree(szBuffer);
    return bResult;

}*/

BOOL GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed)
{
    if (!pDeviceSpeed || hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    ULONG length = sizeof(UCHAR);

    bResult = WinUsb_QueryDeviceInformation(hDeviceHandle, DEVICE_SPEED, &length, pDeviceSpeed);
    if (!bResult)
    {
        printf("Error getting device speed: %d.\n", GetLastError());
        goto done;
    }

    if (*pDeviceSpeed == LowSpeed)
    {
        printf("Device speed: %d (Low speed).\n", *pDeviceSpeed);
        goto done;
    }
    if (*pDeviceSpeed == FullSpeed)
    {
        printf("Device speed: %d (Full speed).\n", *pDeviceSpeed);
        goto done;
    }
    if (*pDeviceSpeed == HighSpeed)
    {
        printf("Device speed: %d (High speed).\n", *pDeviceSpeed);
        goto done;
    }

done:
    return bResult;
}

BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

    WINUSB_PIPE_INFORMATION  Pipe;
    ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


    bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

    if (bResult)
    {
        for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
        {
            bResult = WinUsb_QueryPipe(hDeviceHandle, 0, (UCHAR)index, &Pipe);

            if (bResult)
            {
                if (Pipe.PipeType == UsbdPipeTypeControl)
                {
                    printf("Endpoint index: %d Pipe type: Control Pipe ID: %d.\n", index, Pipe.PipeId);
                }
                if (Pipe.PipeType == UsbdPipeTypeIsochronous)
                {
                    printf("Endpoint index: %d Pipe type: Isochronous Pipe ID: %d.\n", index, Pipe.PipeId);
                }
                if (Pipe.PipeType == UsbdPipeTypeBulk)
                {
                    if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
                    {
                        printf("Endpoint index: %d Pipe type: Bulk Pipe IN ID: %d.\n", index, Pipe.PipeId);
                        pipeid->PipeInId = Pipe.PipeId;
                    }
                    if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
                    {
                        printf("Endpoint index: %d Pipe type: Bulk Pipe OUT ID: %d.\n", index, Pipe.PipeId);
                        pipeid->PipeOutId = Pipe.PipeId;
                    }

                }
                if (Pipe.PipeType == UsbdPipeTypeInterrupt)
                {
                    printf("Endpoint index: %d Pipe type: Interrupt Pipe ID: %d.\n", index, Pipe.PipeId);
                    pipeid->PipeInterruptId = Pipe.PipeId;
                }
            }
            else
            {
                continue;
            }
        }
    }

//done:
    return bResult;
}

static void print_buf(const char *title, const unsigned char *buf, size_t buf_len)
{
    size_t i = 0;
    fprintf(stdout, "%s\n", title);
    for (i = 0; i < buf_len; ++i)
        fprintf(stdout, "%02X%s", buf[i],
        (i + 1) % 16 == 0 ? "\r\n" : " ");

}

BOOL WriteToBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE || !pID || !pcbWritten)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    //UCHAR szBuffer[] = "Hello World";
    //ULONG cbSize = (ULONG)strlen("Hello World");
    
    UCHAR data[] = { 0x08, 0x01, 0x00, 0x02 };
    //ping
    UCHAR szBuffer[18];
    //ULONG cbSize = 18;

    ULONG cbSent = 0;
    //SYNC
    UCHAR headerData[] = "SYNC";
    szBuffer[0] = 0x53;
    szBuffer[1] = 0x59;
    szBuffer[2] = 0x4E;
    szBuffer[3] = 0x43;
    //lenght data
    //int dataSize = (int)std::strlen((char*)data);
    int dataSize = 4;
    UCHAR arr_dataSize[4];
    int2array(dataSize, arr_dataSize);
    szBuffer[4] = 0x04;
    szBuffer[5] = 0x00;
    szBuffer[6] = 0x00;
    szBuffer[7] = 0x00;
    //complement of lenght
    int cDataSize = -(dataSize+1);
    UCHAR arr_cDataSize[4];
    int2array(cDataSize, arr_cDataSize);
    szBuffer[8] = 0xFB;
    szBuffer[9] = 0xFF;
    szBuffer[10] = 0xFF;
    szBuffer[11] = 0xFF;

    //data
    
    szBuffer[12] = 0x08;
    szBuffer[13] = 0x01;
    szBuffer[14] = 0x00;
    szBuffer[15] = 0x02;


    //EN
    UCHAR end[] = "EN";
    szBuffer[16] = 0x45;
    szBuffer[17] = 0x4E;

    //Data Frame Format
    ULONG cbSize2 = (ULONG) (4 + 4 + 4 + dataSize + 2);

    //CHAR * szBuffer2 = new CHAR[cbSize2 + 1];
    UCHAR *szBuffer2 = (UCHAR*)malloc(cbSize2);
    memcpy(szBuffer2, headerData, 4);
    memcpy(szBuffer2 + 4, arr_dataSize, 4);
    memcpy(szBuffer2 + 4 + 4, arr_cDataSize, 4);
    memcpy(szBuffer2 + 4 + 4 + 4, data, dataSize);
    memcpy(szBuffer2 + 4 + 4 + 4 + dataSize, end, 2);
    //strcpy_s(szBuffer2, cbSize2, (char *)headerData);
    //strcat_s(szBuffer2, cbSize2, (char *)arr_dataSize);
    //strcat_s(szBuffer2, cbSize2, (char *)arr_cDataSize);
    //strcat_s(szBuffer2, cbSize2, (char *)data);
    //strcat_s(szBuffer2, cbSize2, (char *)end);
    

    /*I		l_i_Ret;
    PUC		l_puc_Buffer;
    UL		l_ul_ILVSize;
    UC			l_uc_ILV_status;

    l_uc_ILV_status = ILV_OK;
    l_i_Ret = RETURN_NO_ERROR;

    l_puc_Buffer = (PUC)malloc(ILV_BUFFER_SIZE);
    if (!l_puc_Buffer)
    {
        printf("Cannot allocate l_puc_Buffer\r\n");
        return ILV_ERROR_NOT_ENOUGH_MEMORY;
    }
    l_ul_ILVSize = 0;
    // Build request ILV
    l_i_Ret = ILV_Init(l_puc_Buffer, &l_ul_ILVSize, ILV_PING);
    if (l_i_Ret)
    {
        printf("ILV_Init ret 0x%08X\r\n", l_i_Ret);
        free(l_puc_Buffer);
        return l_i_Ret;
    }

    UC		i_uc_DescFormat = ILVSTS_NO_HIT;//x02

    l_i_Ret = ILV_AddValue(l_puc_Buffer, &l_ul_ILVSize, &i_uc_DescFormat, sizeof(UC));
    if (l_i_Ret)
    {
        printf("ILV_AddValue ret 0x%08X\rn", l_i_Ret);
        free(l_puc_Buffer);
        return l_i_Ret;
    }*/

    bResult = WinUsb_WritePipe(hDeviceHandle, *pID, szBuffer2, cbSize2, &cbSent, 0);
    if (!bResult)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        if (FAILED(hr)) {
            goto done;
        }
        
    }

    //const int siz_ar = sizeof(szBuffer2) / sizeof(UCHAR);
    print_buf("Wrote to Pipe OUT", szBuffer2, cbSize2);
    
    free(szBuffer2);
    
    //printf("Wrote to pipe %d: %s \nActual data transferred: %d.\n", *pID, szBuffer, cbSent);
    *pcbWritten = cbSent;


done:
    return bResult;

}

BOOL sendILV(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten, UCHAR *data, int dataSize)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE || !pID || !pcbWritten)
    {
        return FALSE;
    }

    //Size Data Frame
    //ULONG cbSize = (ULONG)(4 + 4 + 4 + dataSize + 2);
    //Data Frame
    //UCHAR *szBuffer = (UCHAR*)malloc(cbSize);
    ULONG cbSize = 4;

    //Send SYNC
    UCHAR headerData[] = "SYNC";
    UCHAR *szBuffer = (UCHAR*)malloc(cbSize);
    memcpy(szBuffer, headerData, 4);

    //Send data size and two's complement
    UCHAR arr_dataSize[4];
    int2array(dataSize, arr_dataSize);
    szBuffer = (UCHAR*)realloc(szBuffer, cbSize += 4);
    memcpy(szBuffer + cbSize - 4, arr_dataSize, 4);

    int cDataSize = -(dataSize + 1);
    UCHAR arr_cDataSize[4];
    int2array(cDataSize, arr_cDataSize);
    szBuffer = (UCHAR*)realloc(szBuffer, cbSize += 4);
    memcpy(szBuffer + cbSize - 4, arr_cDataSize, 4);

    //Send command
    szBuffer = (UCHAR*)realloc(szBuffer, cbSize += dataSize);
    memcpy(szBuffer + cbSize - dataSize, data, dataSize);

    //Send EN
    UCHAR en[] = "EN";
    szBuffer = (UCHAR*)realloc(szBuffer, cbSize += 2);
    memcpy(szBuffer + cbSize - 2, en, 2);
        

    ULONG cbSent = 0;

    BOOL bResult = WinUsb_WritePipe(hDeviceHandle, *pID, szBuffer, cbSize, &cbSent, 0); 

    if (!bResult)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        if (FAILED(hr)) {
            //print_buf("Log %s", hr);
        }

    }

    //const int siz_ar = sizeof(szBuffer2) / sizeof(UCHAR);
    print_buf("Wrote to Pipe OUT", szBuffer, cbSize);

    free(szBuffer);
        
    *pcbWritten = cbSent;
    
    return bResult;

}

BOOL ReadFromBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG cbSize)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;

    UCHAR* szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*cbSize);
    //UCHAR szBuffer[19];


    ULONG cbRead = 0;

    ULONG timeout;
    timeout = 500;
    bResult = WinUsb_SetPipePolicy(hDeviceHandle, *pID, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

    if (FALSE == bResult) {

        //hr = HRESULT_FROM_WIN32(GetLastError());
        //CloseHandle(DeviceData->DeviceHandle);
        //return hr;
        //printf(HRESULT_FROM_WIN32(GetLastError()));
        return FALSE;
    }

    bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, szBuffer, cbSize, &cbRead, 0);
    if (!bResult)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        
        //goto done;
        if (FAILED(hr)) {
            LocalFree(szBuffer);
            return FALSE;
        }
        
    }

    //const int siz_ar = sizeof(szBuffer) / sizeof(UCHAR);
    print_buf("\nRead from Pipe IN", szBuffer, cbSize);

    //printf("Read from pipe %d: %s \nActual data read: %d.\n", *pID, szBuffer, cbRead);


//done:
    LocalFree(szBuffer);
    return bResult;

}


VOID processVerifyReply(UCHAR * buffer, int offset)
{
    UCHAR ILV_OK = 0x00;
    UCHAR ILVSTS_HIT = 0x01;
    UCHAR ILVSTS_NO_HIT = 0x02;
    UCHAR ILVSTS_FFD = 0x22;
    UCHAR ILVSTS_MOIST_FINGER = 0x23;
    UCHAR ILVSTS_MOVED_FINGER = 0x24;
    UCHAR ILVSTS_SATURATED_FINGER = 0x25;
    UCHAR ILVSTS_INVALID_FINGER = 0x26;

    UCHAR ILVERR_ERROR = 0xFF;
    UCHAR ILVERR_BADPARAMETER = 0xFE;
    UCHAR ILVERR_INVALID_MINUTIAE = 0xFD;
    UCHAR ILVERR_TIMEOUT = 0xFA;
    UCHAR ILVERR_CMDE_ABORTED = 0xE5;
    UCHAR ILVERR_BIO_IN_PROGRESS = 0xF5;
    UCHAR ILVERR_CMD_INPROGRESS = 0xF4;
    UCHAR ILVERR_BASE_NOT_FOUND = 0xF7;
    //UCHAR ILVERR_INVALID_USER_ID = 0xFC;
    //UCHAR ILVERR_USER_NOT_FOUND = 0xE6;
    UCHAR ILVERR_BAD_SIGNATURE = 0xF0;
    UCHAR ILVERR_SECU_CERTIF_NOT_EXIST = 0xE2;
    UCHAR ILVERR_SECU_ASN1 = 0xDF;
    UCHAR ILVERR_SECU = 0xDE;
    UCHAR ILVERR_OPERATION_NOT_SUPPORTED = 0xC7;
    UCHAR ILVERR_FFD_FINGER_MISPLACED = 0xB9;


    //I  ILV_VERIFY (0x20)  1 byte
    //L  0x0002 + 7 + 1  2 bytes
    //V  

    ULONG lenght = (buffer[offset + 1] & 0xFF) + ((buffer[offset + 2] << 8) & 0xFF00);    
    offset += 3;

    if (lenght == 5)
    {
        printf("Reply Fail");
        //Request Status 
        if (buffer[offset] == ILVERR_ERROR)
        {
            printf("An unexpected error occurred during the execution of the command");
        }
        if (buffer[offset] == ILVERR_BADPARAMETER)
        {
            printf("At least one parameter format is invalid: see ilv docs");
        }
        if (buffer[offset] == ILVERR_INVALID_MINUTIAE)
        {
            printf("The reference fingerprint template is not valid");
        }
        if (buffer[offset] == ILVERR_TIMEOUT)
        {
            printf("The finger detection timeout has expired");
        }
        if (buffer[offset] == ILVERR_CMDE_ABORTED)
        {
            printf("Command has been cancelled by the Host System (ILV_CANCEL received).");
        }
        if (buffer[offset] == ILVERR_BIO_IN_PROGRESS)
        {
            printf("Command received during biometric processing");
        }
        if (buffer[offset] == ILVERR_CMD_INPROGRESS)
        {
            printf("Command received while another command is running");
        }
        if (buffer[offset] == ILVERR_BASE_NOT_FOUND)
        {
            printf("The searched user is not found.");
        }
        if (buffer[offset] == ILVERR_BAD_SIGNATURE)
        {
            printf("Invalid digital signature.");
        }
        if (buffer[offset] == ILVERR_SECU_CERTIF_NOT_EXIST)
        {
            printf("The required certificate does not exist.");
        }
        if (buffer[offset] == ILVERR_SECU_ASN1)
        {
            printf("Error while decoding an ASN1 object or bad X9.84 template index.");
        }
        if (buffer[offset] == ILVERR_SECU)
        {
            printf("Cryptographic error.");
        }
        if (buffer[offset] == ILVERR_OPERATION_NOT_SUPPORTED)
        {
            printf("Operation not supported by the firmware");
        }
        if (buffer[offset] == ILVERR_FFD_FINGER_MISPLACED)
        {
            printf("Finger was misplaced during acquisition .");
        }
        return;
    }

    //Request Status    
    if (buffer[offset] == ILV_OK)
    {
        printf("Command is Valid\n");
        //Matching Result : Result of the comparison
        offset += 1;
        if (buffer[offset] == ILVSTS_HIT)
        {
            printf("The comparison succeeded (match found).\n");
        }
        if (buffer[offset] == ILVSTS_NO_HIT)
        {
            printf("The comparison fails (no match).\n");
        }
        if (buffer[offset] == ILVSTS_FFD)
        {
            printf("False finger detection.\n");
        }
        if (buffer[offset] == ILVSTS_MOIST_FINGER)
        {
            printf("Finger too moist or the scanner is wet.\n");
        }
        if (buffer[offset] == ILVSTS_MOVED_FINGER)
        {
            printf("Finger moved during acquisition or removed earlier.\n");
        }
        if (buffer[offset] == ILVSTS_SATURATED_FINGER)
        {
            printf("Finger can be too shiny or too much external light.\n");
        }
        if (buffer[offset] == ILVSTS_INVALID_FINGER)
        {
            printf("Finger is invalid.\n");
        }
        //Matching Score value 
        offset += 1;
        if (buffer[offset] == 0x56)//ID_MATCHING_SCORE (0x56)
        {
            offset += 2;
            ULONG score = buffer[offset] + (buffer[offset + 1] & 0xFF) + ((buffer[offset + 2] << 8) & 0xFF00) + ((buffer[offset + 3] << 16) & 0xFF0000);
            printf("score %d\n", score);
        }

    }
    else
    {
        printf("Invalid Command\n");
    }

}


struct BITMAPHEADER
{
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[256];
} m_InfoBitmap;

VOID SaveImageBMP(UCHAR * i_puc_Image, ULONG rowNumber, ULONG colNumber, ULONG horzRes, ULONG vertRes)
{
    BITMAPFILEHEADER	l_FileHeader;
    BITMAPHEADER		l_InfoBitmap;

    int l_i_Size = rowNumber * colNumber;

    PUCHAR l_puc_TempImage = new UCHAR[l_i_Size];
    // in the .bmp  file,
	// biHeight must be > 0. 
	// So we invert the order of the rows in the image buffer.

	//Init RGBQUAD Palette monochrome de 256 niveaux
    for (int i = 0; i < 256; i++)
    {
        l_InfoBitmap.bmiColors[i].rgbBlue = (UCHAR)i;
        l_InfoBitmap.bmiColors[i].rgbGreen = (UCHAR)i;
        l_InfoBitmap.bmiColors[i].rgbRed = (UCHAR)i;
        l_InfoBitmap.bmiColors[i].rgbReserved = 0;
    }

    //Init BITMAPINFOHEADER    
    l_InfoBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    l_InfoBitmap.bmiHeader.biWidth = colNumber;
    l_InfoBitmap.bmiHeader.biHeight = -(LONG)rowNumber;
    // the minus sign imply that the bitmap is a top-down DIB 
    // and its origin is the upper left corner

    l_InfoBitmap.bmiHeader.biPlanes = 1;
    l_InfoBitmap.bmiHeader.biBitCount = 8;
    l_InfoBitmap.bmiHeader.biCompression = BI_RGB;
    l_InfoBitmap.bmiHeader.biSizeImage = 0;

    l_InfoBitmap.bmiHeader.biXPelsPerMeter = (int)(10000 * horzRes / 254);	// conversion inch<->meter
    l_InfoBitmap.bmiHeader.biYPelsPerMeter = (int)(10000 * vertRes / 254);
    l_InfoBitmap.bmiHeader.biClrUsed = 0;
    l_InfoBitmap.bmiHeader.biClrImportant = 0;

    l_InfoBitmap.bmiHeader.biHeight = abs(l_InfoBitmap.bmiHeader.biHeight);
    l_InfoBitmap.bmiHeader.biWidth = abs(l_InfoBitmap.bmiHeader.biWidth);

    for (int l_i_SrcPos = 0, l_i_DstPos = l_i_Size - colNumber; l_i_SrcPos < l_i_Size; l_i_SrcPos += colNumber, l_i_DstPos -= colNumber)
    {
        memcpy(l_puc_TempImage + l_i_DstPos, i_puc_Image + l_i_SrcPos, colNumber);
    }

    l_FileHeader.bfType = 0x4d42;   // 'BM' WINDOWS_BITMAP_SIGNATURE
    l_FileHeader.bfSize = colNumber * rowNumber + sizeof(l_InfoBitmap) + sizeof(l_FileHeader);
    l_FileHeader.bfReserved1 = 0;
    l_FileHeader.bfReserved2 = 0;
    l_FileHeader.bfOffBits = sizeof(l_InfoBitmap) + sizeof(l_FileHeader);


    CHAR * l_cs_File = PATH_BMP_FILE;

    CFile raw(l_cs_File);
    raw.write(&l_FileHeader, sizeof(l_FileHeader));
    raw.write(&l_InfoBitmap, sizeof(l_InfoBitmap));
    raw.write(l_puc_TempImage, colNumber * rowNumber);
    raw.close();

    delete[] l_puc_TempImage;
}

ULONG processPKCOMP(UCHAR * buffer, int offset)
{
    //UCHAR headerSize = buffer[offset + 4]; // Must be 0x0A
    //ULONG rowNumber = (buffer[offset + 5] & 0xFF) + ((buffer[offset + 6] << 8) & 0xFF00); //Vertical Size (Y) (pixels) 
    //ULONG colNumber = (buffer[offset + 7] & 0xFF) + ((buffer[offset + 8] << 8) & 0xFF00);//Horizontal Size (X)(pixels) 
    //ULONG vertRes = (buffer[offset + 9] & 0xFF) + ((buffer[offset + 10] << 8) & 0xFF00);//Vertical Resolution (dpi) 
    //ULONG horzRes = (buffer[offset + 11] & 0xFF) + ((buffer[offset + 12] << 8) & 0xFF00);//Horizontal Resolution (dpi) 
    //UCHAR compression = buffer[offset + 13];
    //UCHAR compressionParam = buffer[offset + 14];
    //ULONG imgSize = rowNumber * colNumber;
    //print "----Image data---- \nrowNumber: %d \ncolNumber: %d \nvertRes: %d \nhorzRes: %d \n%d \n%d" % (rowNumber, colNumber, vertRes, horzRes, compression, compressionParam)
    //offset += 15;


    //I  ID_PKCOMP(0x02)  1 byte
    //L  0x0000 + L  2 bytes
    //V  PK_COMP Minutiae  L bytes(L <= 256)

    ULONG lenght = (buffer[offset + 1] & 0xFF) + ((buffer[offset + 2] << 8) & 0xFF00);
    //ULONG dataSize = 3 + lenght;
    offset += 3;

    UCHAR *huella = (UCHAR*)malloc(lenght);
    memcpy(huella, buffer + offset, lenght);

    //print_buf("huella", huella, imgSize);

    CHAR * l_cs_File = PATH_PKC_FILE;

    CFile raw(l_cs_File);
    raw.write(huella, lenght);
    raw.close();

    //SaveImageBMP(huella, rowNumber, colNumber, horzRes, vertRes);///

    free(huella);

    return 1 + 2 + lenght;
}

VOID processImage(UCHAR * buffer, int offset)
{
    //UCHAR headerSize = buffer[offset + 4]; // Must be 0x0A
    ULONG rowNumber = (buffer[offset + 5] & 0xFF) + ((buffer[offset + 6] << 8) & 0xFF00); //Vertical Size (Y) (pixels) 
    ULONG colNumber = (buffer[offset + 7] & 0xFF) + ((buffer[offset + 8] << 8) & 0xFF00);//Horizontal Size (X)(pixels) 
    ULONG vertRes = (buffer[offset + 9] & 0xFF) + ((buffer[offset + 10] << 8) & 0xFF00);//Vertical Resolution (dpi) 
    ULONG horzRes = (buffer[offset + 11] & 0xFF) + ((buffer[offset + 12] << 8) & 0xFF00);//Horizontal Resolution (dpi) 
    //UCHAR compression = buffer[offset + 13];
    //UCHAR compressionParam = buffer[offset + 14];
    ULONG imgSize = rowNumber * colNumber;
    //print "----Image data---- \nrowNumber: %d \ncolNumber: %d \nvertRes: %d \nhorzRes: %d \n%d \n%d" % (rowNumber, colNumber, vertRes, horzRes, compression, compressionParam)
    offset += 15;

    //Reply (ILV formatted data containing an image included in the reply to ILV_ENROLL command when required.)
    //
    //I  ID_EXPORT_IMAGE(0x3D)  1 byte
    //L  0x000C + L1  2 bytes
    //V Image Header  0x0C bytes (12 bytes)
    //  Image Binary Data  L1 bytes

    UCHAR *huella = (UCHAR*)malloc(imgSize);
    memcpy(huella, buffer + offset, imgSize);

    //print_buf("huella", huella, imgSize);

    CHAR * l_cs_File = PATH_IMAGE_FILE;

    CFile raw(l_cs_File);
    raw.write(huella, imgSize);
    raw.close();

    SaveImageBMP(huella, rowNumber, colNumber, horzRes, vertRes);

    free(huella);
}


VOID processILV(UCHAR * buffer, int size, int offset, BOOL *imageProcessed, BOOL *pckProcessed)
{
    UCHAR ILV_OK = 0x00;
    UCHAR ILVERR_CMDE_ABORTED = 0xE5;

    UCHAR ILVSTS_OK = 0x00;
    UCHAR ILVSTS_HIT = 0x01;
    //UCHAR ILVSTS_NO_HIT = 0x02;
    //UCHAR ILVSTS_DB_FULL = 0x04;
    //UCHAR ILVSTS_DB_EMPTY = 0x05;
    //UCHAR ILVSTS_FFD = 0x22;
    //UCHAR ILVSTS_MOIST_FINGER = 0x23;
    

    UCHAR code = NULL;
    ULONG longitud = 0;
    UCHAR status = NULL;
    
    ULONG dbIdx = NULL;
    //ILVCommand = NULL;

    if (buffer[offset] == 0x50) //ILV_INVALID	
    {
        printf("Invalid ILV command");
    }
    else
    {
        code = buffer[offset];
        //lenght is stored in two bytes
        longitud = (buffer[offset + 1] & 0xFF) + ((buffer[offset + 2] << 8) & 0xFF00);

        //do not comment
        if (longitud == 65535)//Bigger message, use 4 bytes
        {
            //printf("longitud 4 bytes");
            offset += 2;
            longitud = (buffer[offset + 1] & 0xFF) + ((buffer[offset + 2] << 8) & 0xFF00) + ((buffer[offset + 3] << 16) & 0xFF0000) + ((buffer[offset + 4] << 24) & 0xFF000000);
            offset += 2;
        }
        
        //print "ILV command Length = ", str(longitud)
        status = 0xFF;
        if (longitud > 0)
        {
            status = buffer[offset + 3];
        }
        if (code == 0x71)//"Asynchronous Message" Command
        {
            if (status != ILV_OK)
            {
                printf("Erroneous asyncronous message");
                return;
            }
            ILVCommand = code;
            if (longitud > 6)
            {
                processILV(buffer, size, offset + 4, imageProcessed, pckProcessed);
                return;
            }
            
        }
        //print "code:", str(code)
        //Execute when ILVCommand already loaded on previous iteration
        if (ILVCommand == 0x71)//Asynchronous message
        {
            ILVCommand = code;
            if (code == 0x01)//Control message
            {
                //do nothing
            }
            else if(code == 0x02)//Asynchronous image message
            {
                //0x02: Live Low-Resolution Images & Acquired Fingerprint Image (normal resolution)
                //ref ID_ASYNCHRONOUS_EVENT 
                printf("\n live async image\n");                
                processImage(buffer, offset);//uncomment if require preview images               
                return;
            }
            else
            {
                printf("Unknown asynchronous message");
            }
            return;
        }
        if (code == 0x22)//Identification command
        {
            if (status != ILV_OK)
            {
                printf("Error code %d" , status);
                if (status != ILVERR_CMDE_ABORTED) {
                    printf("Biometric device error, please try again");
                }
                return;
            }
            if (buffer[offset + 4] == ILVSTS_HIT)
            {
                dbIdx = buffer[offset + 5] + (buffer[offset + 6] << 8) + (buffer[offset + 7] << 16) + (buffer[offset + 8] << 24);
                printf("Identified user");
                ILVCommand = code;
                if (longitud > 6) {
                    processILV(buffer, size, offset + 9, imageProcessed, pckProcessed);
                }
                
            }
            else
            {
                printf("Unidentified user");
            }
            return;
        }
        if (code == 0x21)// Enrol command
        {
            ILVCommand = code;
            if (status != ILV_OK)
            {
                printf("Error code %d", status);
                if (status != ILVERR_CMDE_ABORTED)
                {
                    printf("Biometric device error, please try again");
                }
                return;
            }
            if (buffer[offset + 4] == ILVSTS_OK)
            {
                //User Database Index 
                dbIdx = buffer[offset + 5] + (buffer[offset + 6] << 8) + (buffer[offset + 7] << 16) + (buffer[offset + 8] << 24);
                //print "User correctly enrolled"
                ILVCommand = code;
                if (longitud > 6)
                {
                    processILV(buffer, size, offset + 9, imageProcessed, pckProcessed);
                    return;
                }
            }
            else
            {
                printf("User not enrolled, please try again");
            }
            return;
        }
        if (code == 0x3d)// Image command
        {
            ILVCommand = code;            
            printf("Final fingerprint received\n");            
            processImage(buffer, offset);
            *imageProcessed = TRUE;//exit Winusb reading loop
            return;
        }

        if (code == 0x02)// PKCOMP command
        {
            ILVCommand = code;
            printf("Final PKCOMP received\n");
            ULONG size_first_ilv = processPKCOMP(buffer, offset);  

            *pckProcessed = TRUE;//exit Winusb reading loop
            
            if (buffer[offset + size_first_ilv] == 0x3d)// Image command
            {
                printf("Image found in second segment\n");
                
                processILV(buffer, size, offset + size_first_ilv, imageProcessed, pckProcessed);
                return;
                
            }            
            
            return;
        }

        if (code == 0x20)// Verify Reply command
        {
            ILVCommand = code;
            printf("Final Verify Reply received\n");
            processVerifyReply(buffer, offset);
            *imageProcessed = TRUE;
            *pckProcessed = TRUE;
            //*itemProcessed = TRUE;//exit Winusb reading loop
            return;
        }

        if (code == 0x23)// Verify Match Reply command
        {
            ILVCommand = code;
            printf("Final Verify Match Reply received\n");
            processVerifyReply(buffer, offset);
            *imageProcessed = TRUE;
            *pckProcessed = TRUE;
            //*itemProcessed = TRUE;//exit Winusb reading loop
            return;
        }

    }
    

}

VOID int2array(int i, UCHAR *arr) 
{
    int n[] = {0, 8, 16, 24};
    for (int k = 0; k < 4; k++) {
        arr[k] = i >> n[k] & 0xff;
    }
}

VOID short2array(short i, UCHAR *arr)
{
    int n[] = { 0, 8 };
    for (int k = 0; k < 2; k++) {
        arr[k] = i >> n[k] & 0xff;
    }
}

////============================================================================
//// Name        : Pas19.cpp
//// Author      : Alexandr
//// Version     :
//// Copyright   : Your copyright notice
//// Description : Hello World in C++, Ansi-style
////============================================================================
//
//#include <iostream>
//using namespace std;
//
//int main() {
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//	return 0;
//}



#include <iostream>

#include "Configuration.h"
#include "MainProductionCycle.h"
#include "DataStore.h"
#include "ModbusRTU.h"

using namespace std;

uint8_t auiTempBlock[]
{
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

static uint8_t m_aucRtuCoilsArray[COILS_WORK_ARRAY_LENGTH];
static uint8_t m_aucRtuDiscreteInputsArray[DISCRETE_INPUTS_ARRAY_LENGTH];
static uint16_t m_aucRtuHoldingRegistersArray[HOLDING_REGISTERS_ARRAY_LENGTH] = {1, 2, 3, 4, 5, 6, 7};
static uint16_t m_aucRtuInputRegistersArray[INPUT_REGISTERS_ARRAY_LENGTH];
//    static uint8_t m_aucRtuDiscreteDataArray[DISCRETE_DATA_ARRAY_LENGTH];
static uint8_t m_auiReceiveMessageBuff[MODBUS_RTU_MAX_ADU_LENGTH];
static uint8_t m_auiTransmitMessageBuff[MODBUS_RTU_MAX_ADU_LENGTH];

//-----------------------------------------------------------------------------------------------------
int main()
{
//    CMainProductionCycle xMainProductionCycle;
//
//    while (1)
//    {
//        xMainProductionCycle.Fsm();
//    }

//    CTimer xTimer;
//    xTimer.Set(1000);
//
//    for (uint8_t i = 0; i < 5; i++)
//    {
////        if (xTimer.IsOverflow())
////        {
////            xTimer.Reset();
////            cout << "Hello world!" << endl;
////        }
//
//        while (1)
//        {
//            if (xTimer.IsOverflow())
//            {
//                xTimer.Reset();
//                cout << "Hello world!" << endl;
//                break;
//            }
//        }
//    }

    CDataStore* pxDataStoreFileSystem = new CDataStore(new CStorageDeviceFileSystem);


    if (!(pxDataStoreFileSystem -> Check()))
    {
        cout << "DataStore check error" << endl;
        cout << "CreateServiceSection" << endl;
        pxDataStoreFileSystem -> CreateServiceSection();

        pxDataStoreFileSystem -> PassingBlockDataAndStartWrite(auiTempBlock, sizeof(auiTempBlock), 0);
        do
        {
            pxDataStoreFileSystem -> Fsm();
        }
        while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);


        pxDataStoreFileSystem -> PassingBlockDataAndStartWrite(auiTempBlock, sizeof(auiTempBlock), 1);
        do
        {
            pxDataStoreFileSystem -> Fsm();
        }
        while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);

        pxDataStoreFileSystem -> PassingBlockDataAndStartWrite(auiTempBlock, sizeof(auiTempBlock), 2);
        do
        {
            pxDataStoreFileSystem -> Fsm();
        }
        while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);

        pxDataStoreFileSystem -> PassingBlockDataAndStartWrite(auiTempBlock, sizeof(auiTempBlock), 3);
        do
        {
            pxDataStoreFileSystem -> Fsm();
        }
        while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);

        pxDataStoreFileSystem -> PassingBlockDataAndStartWrite(auiTempBlock, sizeof(auiTempBlock), 4);
        do
        {
            pxDataStoreFileSystem -> Fsm();
        }
        while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);
    }
    else
    {
        cout << "DataStore check ok" << endl;
    }

//    if (!(pxDataStoreFileSystem -> ReadServiceSection()))
//    {
//        cout << "CreateServiceSection" << endl;
//        pxDataStoreFileSystem -> CreateServiceSection();
//    }



//    pxDataStoreFileSystem -> BlockWritePrepare(auiTempBlock, sizeof(auiTempBlock), 0);

////    pxDataStoreFileSystem -> SetFsmEvent(CDataStore::WRITE_IN_PROGRESS_FSM_EVENT);
//    // �������� ������� ������.
//    pxDataStoreFileSystem -> SetFsmState(CDataStore::START_WRITE_TEMPORARY_SERVICE_SECTION_DATA);

//    pxDataStoreFileSystem -> TemporaryServiceSectionWritePrepare();




//    uint8_t auiTempArray[512];
//
//    if (pxDataStoreFileSystem -> ReadBlock(auiTempArray, 0))
//    {
//        cerr << "ReadBlock ok" << endl;
//    }
//    else
//    {
//        cerr << "ReadBlock error" << endl;
//    }

    CModbusRtu xModbusRtuUpperLevel;
    xModbusRtuUpperLevel.SerialPortInit("/dev/ttyO1",
                                        9600,
                                        8,
                                        'N',
                                        2);
    xModbusRtuUpperLevel.WorkingArraysInit(m_auiReceiveMessageBuff,
                                           m_auiTransmitMessageBuff,
                                           m_aucRtuCoilsArray,
                                           m_aucRtuDiscreteInputsArray,
                                           m_aucRtuHoldingRegistersArray,
                                           m_aucRtuInputRegistersArray,
                                           COILS_WORK_ARRAY_LENGTH,
                                           DISCRETE_INPUTS_ARRAY_LENGTH,
                                           HOLDING_REGISTERS_ARRAY_LENGTH,
                                           INPUT_REGISTERS_ARRAY_LENGTH
                                          );
    xModbusRtuUpperLevel.SlaveSet(1);
    xModbusRtuUpperLevel.SetFsmState(CModbusRtu::START_REQUEST);

    while (1)
    {
        xModbusRtuUpperLevel.Fsm();
        usleep(1000);
    }

    delete pxDataStoreFileSystem;
    return 0;
}



//                cout << hex << uppercase << setw(2) << setfill('0') << (unsigned int)pucSourceTemp[i + j] << " ";
//            }
//            cout << endl;

//cout << "vDeviceControlOnlineDataReadHandler uiAddressHigh" << (int)ucAddressHigh << endl;

//    cout << "CStorageDeviceFileSystem::Write uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Write uiLength" << " " << (int)uiLength << endl;
//    cout << "CStorageDeviceFileSystem::Write sizeof" << " " << (int)(sizeof(struct CDataStore::TServiseSection)) << endl;

//    cout << "CStorageDeviceFileSystem::Write pccFileName" << " " << pccFileName << endl;

//arm-linux-gnueabihf-
//C:\SysGCC\beaglebone\bin
//-pthread -lrt
//sudo /home/debian/Pas19


//#include <stddef.h>
//#include <stdio.h>
//#include <sys/time.h>
//#include <sys/select.h>
//#include <unistd.h>
//#include <fcntl.h>
//
//int main(int argc, char **argv) {
//    int ready;
//    struct timeval *pto;
//    fd_set readfds, writefds;
//    char buf[1024];
//    int bytes;
//    int fd = 0; // stdin
//
//    pto = NULL;
//    FD_ZERO(&readfds);
//    FD_ZERO(&writefds);
//    FD_SET(fd, &readfds);
//
//    int flags = fcntl(fd, F_GETFL, 0);
//    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
//
//    while (1) {
//        ready = select(1, &readfds, &writefds, NULL, pto);
//
//        if (ready == -1) {
//            printf("got -1\n");
//            return 1;
//        }
//
//        printf("ready = %d\n", ready);
//
//        bytes = read(fd, buf, 1024);
//        if (bytes == 0) {
//            printf("all done\n");
//            return 0;
//        } else if (bytes > 0) {
//            buf[bytes] = '\0';
//            printf("read: %s\n", buf);
//        } else {
//            printf("got an error\n");
//        }
//    }
//}



//sudo start-stop-daemon -Kvx /home/debian/Pas19
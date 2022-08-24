#include <iostream>

#include "MainProductionCycle.h"
#include "DataStore.h"

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

//-----------------------------------------------------------------------------------------------------
int main()
{
//    CMainProductionCycle xMainProductionCycle;
//
//    while (1)
//    {
//        xMainProductionCycle.Fsm();
//    }




    cout << "Hello world!" << endl;

    CDataStore* pxDataStoreFileSystem = new CDataStore(new CStorageDeviceFileSystem);


    if (!(pxDataStoreFileSystem -> Check()))
    {
        cerr << "DataStore check error" << endl;
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
        cerr << "DataStore check ok" << endl;
    }

//    if (!(pxDataStoreFileSystem -> ReadServiceSection()))
//    {
//        cout << "CreateServiceSection" << endl;
//        pxDataStoreFileSystem -> CreateServiceSection();
//    }



//    pxDataStoreFileSystem -> BlockWritePrepare(auiTempBlock, sizeof(auiTempBlock), 0);

////    pxDataStoreFileSystem -> SetFsmEvent(CDataStore::WRITE_IN_PROGRESS_FSM_EVENT);
//    // Запустим процесс записи.
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

    delete pxDataStoreFileSystem;
    return 0;
}




//                cout << hex << uppercase << setw(2) << setfill('0') << (unsigned int)pucSourceTemp[i + j] << " ";
//            }
//            cout << endl;

//cout << "vDeviceControlOnlineDataReadHandler ucAddressHigh" << (int)ucAddressHigh << endl;

//    cout << "CStorageDeviceFileSystem::Write uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Write uiLength" << " " << (int)uiLength << endl;
//    cout << "CStorageDeviceFileSystem::Write sizeof" << " " << (int)(sizeof(struct CDataStore::TServiseSection)) << endl;

//    cout << "CStorageDeviceFileSystem::Write pccFileName" << " " << pccFileName << endl;


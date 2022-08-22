#include <iostream>

#include "DataStore.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    CDataStore* pxDataStoreFileSystem = new CDataStore(new CStorageDeviceFileSystem);

    pxDataStoreFileSystem -> CreateServiceSection();

//    pxDataStoreFileSystem -> SetFsmEvent(CDataStore::WRITE_IN_PROGRESS_FSM_EVENT);
    // Запустим процесс записи.
    pxDataStoreFileSystem -> SetFsmState(CDataStore::START_WRITE_TEMPORARY_SERVICE_SECTION_DATA);

//    pxDataStoreFileSystem -> WriteTemporaryServiceSection();

    do
    {
        pxDataStoreFileSystem -> Fsm();
    }
    while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);

//    pxDataStoreFileSystem -> WriteServiceSection();
//
//    do
//    {
//        pxDataStoreFileSystem -> Fsm();
//    }
//    while (pxDataStoreFileSystem -> GetFsmState() != CDataStore::IDDLE);




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


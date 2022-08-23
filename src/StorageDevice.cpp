
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include <iostream>
#include <string.h>

#include "DataStore.h"
#include "StorageDevice.h"

//-----------------------------------------------------------------------------------------------------
CStorageDeviceFileSystem::CStorageDeviceFileSystem()
{

}

//-----------------------------------------------------------------------------------------------------
CStorageDeviceFileSystem::~CStorageDeviceFileSystem()
{

}

//-----------------------------------------------------------------------------------------------------
void CStorageDeviceFileSystem::Init(void)
{
}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Write(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength)
{
    SetOffset(uiOffset);
    SetBufferPointer(puiSource);
    SetLength(uiLength);

    return Write();
}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Write()
//uint8_t CStorageDeviceFileSystem::Write(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength)
{
    uint16_t uiOffset = m_uiOffset;
    uint8_t *puiSource = m_puiBuffer;
    uint16_t uiLength = m_uiLength;

//    cout << "CStorageDeviceFileSystem::Write uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Write uiLength" << " " << (int)uiLength << endl;
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ofstream outdata;
        // Чтобы добавить и не стереть старые данные откроем файл на чтение и запись.
        outdata.open(pccFileName, (ios::binary | ios::in | ios::out));
        // Файл не существует?
        if (!outdata)
        {
            cerr << "CStorageDeviceFileSystem::Write Error: file could not be opened" << endl;
            // чтобы создать файл откроем только на запись.
            outdata.open(pccFileName, (ios::binary | ios::out));
            // Файл не создан?
            if (!outdata)
            {
                cerr << "CStorageDeviceFileSystem::Write Error: file could not be created" << endl;
                return 0;
            }
        }

        outdata.seekp(uiOffset, ios_base::beg);
        outdata.write((char*)puiSource, uiLength);

        // закроем файл.
        outdata.close();

        SetIsDataWrited(true);
        return 1;
    }
    else
    {
        return 0;
    }
}

////-----------------------------------------------------------------------------------------------------
//uint8_t CStorageDeviceFileSystem::Write(void)
//{
//    return Write(GetOffset(),
//                 GetBufferPointer(),
//                 GetLength());
//}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength)
{
//    cout << "CStorageDeviceFileSystem::Read uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Read uiLength" << " " << (int)uiLength << endl;
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ifstream indata;
        // откроем файл.
        indata.open(pccFileName, (ios::in | ios::binary));
        if (!indata)
        {
            cerr << "CStorageDeviceFileSystem::Read Error: file could not be opened" << endl;
            return 0;
        }
        else
        {
            // установим смещение в файле.
            indata.seekg(uiOffset, ios_base::beg);
            // прочитаем файл.
            indata.read(reinterpret_cast<char*>(puiDestination),
                        uiLength);
        }
        // закроем файл.
        indata.close();
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------


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
// ������� ������ ��������� ������ ����� �������� ���������� �������� � ��������� ������� ������.
uint8_t CStorageDeviceFileSystem::PassingDataAndStartWrite(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength)
{
    SetOffset(uiOffset);
    SetBufferPointer(puiSource);
    SetLength(uiLength);

    return Write();
}

//-----------------------------------------------------------------------------------------------------
// ���������� ���� ������ � ���������� ��������.
uint8_t CStorageDeviceFileSystem::Write()
{
    uint16_t uiOffset = m_uiOffset;
    uint8_t *puiSource = m_puiBuffer;
    uint16_t uiLength = m_uiLength;

//    cout << "CStorageDeviceFileSystem::Write uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Write uiLength" << " " << (int)uiLength << endl;
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ofstream outdata;
        // ����� �������� � �� ������� ������ ������ ������� ���� �� ������ � ������.
        outdata.open(pccFileName, (ios::binary | ios::in | ios::out));
        // ���� �� ����������?
        if (!outdata)
        {
            cerr << "CStorageDeviceFileSystem::Write Error: file could not be opened" << endl;
            // ����� ������� ���� ������� ������ �� ������.
            outdata.open(pccFileName, (ios::binary | ios::out));
            // ���� �� ������?
            if (!outdata)
            {
                cerr << "CStorageDeviceFileSystem::Write Error: file could not be created" << endl;
                return 0;
            }
        }

        outdata.seekp(uiOffset, ios_base::beg);
        outdata.write((char*)puiSource, uiLength);

        // ������� ����.
        outdata.close();

        SetIsDataWrited(true);
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// ��������� ���� ������ �� ���������� ��������.
uint8_t CStorageDeviceFileSystem::Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength)
{
//    cout << "CStorageDeviceFileSystem::Read uiOffset" << " " << (int)uiOffset << endl;
//    cout << "CStorageDeviceFileSystem::Read uiLength" << " " << (int)uiLength << endl;
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ifstream indata;
        // ������� ����.
        indata.open(pccFileName, (ios::in | ios::binary));
        if (!indata)
        {
            cerr << "CStorageDeviceFileSystem::Read Error: file could not be opened" << endl;
            return 0;
        }
        else
        {
            // ��������� �������� � �����.
            indata.seekg(uiOffset, ios_base::beg);
            // ��������� ����.
            indata.read(reinterpret_cast<char*>(puiDestination),
                        uiLength);
        }
        // ������� ����.
        indata.close();
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------

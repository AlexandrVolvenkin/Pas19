
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
#include "Crc.h"
#include "HammingCodes.h"



//-----------------------------------------------------------------------------------------------------
CDataStore::CDataStore()
{
    m_puiIntermediateBuff = new uint8_t[MAX_ENCODED_BLOCK_LENGTH];
    SetFsmState(IDDLE);
    SetSavedFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CDataStore::CDataStore(CStorageDevice* pxStorageDevice) :
    m_pxStorageDevice(pxStorageDevice)
{
    m_puiIntermediateBuff = new uint8_t[MAX_ENCODED_BLOCK_LENGTH];
    SetFsmState(IDDLE);
    SetSavedFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CDataStore::~CDataStore()
{
    delete m_puiIntermediateBuff;
    delete m_pxStorageDevice;

}

//-----------------------------------------------------------------------------------------------------
void CDataStore::CreateServiceSection(void)
{
    SetFsmState(IDDLE);

    // ������� ��������� ��������.
    memset(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
           0,
           sizeof(struct TServiseSectionData));

    // ��������� ������ ��������� ������.
    m_xServiseSection.xServiseSectionData.uiLength = sizeof(struct TServiseSectionData);
    // ���������� ���� ���������� ���������
    // ������ ���� �������� �������.
    // ��� ��� �� ���� ����������� ��������� ������.
    // ������������ ����������������������� ��� ��������(8,4).
    // ����������� - 1.5: ���� ���� ������������� � ������� ����� 12 ���,
    // �� ���� ���� �������� ������ ���������� ��� ����� ������������.
    // ��������� ������ �������������� ������.
    m_xServiseSection.xServiseSectionData.uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));
    // ��������� �������� �� ������ ������ ������.
    m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset =
        (SERVICE_SECTION_DATA_BEGIN +
         m_xServiseSection.xServiseSectionData.uiEncodedLength);
    // ��������� �����.
    m_xServiseSection.xServiseSectionData.uiBlocksNumber = 0;
    // ��������� ������� - ��������� �� ���������.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteTemporaryServiceSection(void)
{
    // �������� ����������� �����.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> Write(TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteServiceSection(void)
{
    // �������� ����������� �����.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> Write(SERVICE_SECTION_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::ReadTemporaryServiceSection(void)
{
    uint16_t uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));

    // ��������� �������������� ������.
    // ��� ������ ������ �������� ������?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // ��� ������.
        return 0;
    }

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.uiCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                    sizeof(struct TServiseSectionData)))
    {
        return 1;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::ReadServiceSection(void)
{
    uint16_t uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));

    // ��������� �������������� ������.
    // ��� ������ ������ �������� ������?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    SERVICE_SECTION_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // ��� ������.
        return 0;
    }

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.uiCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                    sizeof(struct TServiseSectionData)))
    {
        return 1;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::CheckTemporaryBlock(void)
{
    uint8_t uiBlock = m_uiBlock;

    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    //    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // ���� ����������?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // ������� ����� ����� � EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // ������� ������ �����.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // ������� ������ ��������������� �����.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }

    // ��������� �������������� ������.
    // ��� ������ ������ �������� ������?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    TEMPORARY_BLOCK_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // ��� ������.
        return 0;
    }

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(m_puiIntermediateBuff, m_puiIntermediateBuff, uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(m_puiIntermediateBuff, uiLength))
    {
        return 1;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::CheckBlock(void)
{
    uint8_t uiBlock = m_uiBlock;

    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // ���� ����������?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // ������� ����� ����� � EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // ������� ������ �����.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // ������� ������ ��������������� �����.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }

    // ��������� �������������� ������.
    // ��� ������ ������ �������� ������?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    uiSourceOffset,
                                    uiEncodedLength)))
    {
        // ��� ������.
        return 0;
    }

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(auiTempArray, m_puiIntermediateBuff, uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(auiTempArray, uiLength))
    {
        return 1;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataStore::ReadBlock(uint8_t *puiDestination, uint8_t uiBlock)
{
    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // ���� ����������?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // ������� ����� ����� � EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // ������� ������ �����.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // ������� ������ ��������������� �����.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }

    // ��������� �������������� ������.
    m_pxStorageDevice -> Read(m_puiIntermediateBuff, uiSourceOffset, uiEncodedLength);

    // ���������� ����������� ������.
    CHammingCodes::HammingCodesToBytes(auiTempArray, m_puiIntermediateBuff, uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(auiTempArray, uiLength))
    {
        memcpy(puiDestination, auiTempArray, uiLength);
        return uiLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteTemporaryBlock(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff, puiSource, uiLength);

    // ������ �������� ����� ������ - uiBlock ����������� �������?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength == 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength == 0))
    {
        // �������� ����� ���� ������.
        // �������� �������� �� ����.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiOffset = m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset;
        // ������� �������� �� ��������� �����.
        m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset += uiEncodedByteCounter;
        // �������� ������ ��������� ������.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiLength = uiLength;
        // �������� ������ �������������� ������.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiEncodedLength = uiEncodedByteCounter;
        // �������� ���������� ������ ����������� � ���������.
        m_xServiseSection.xServiseSectionData.uiBlocksNumber += 1;
    }

    // �������� ����������� ����� ����������� ������.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);

    if (m_pxStorageDevice -> Write(TEMPORARY_BLOCK_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// ����������� ���� � ������ ������� ��������.
// ������� ������ �������� ���������� �������� � ��������� ������� ������.
uint8_t CDataStore::WriteBlock(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff, puiSource, uiLength);

    // ������ �������� ����� ������ - uiBlock ����������� �������?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength == 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength == 0))
    {
        // �������� ����� ���� ������.
        // �������� �������� �� ����.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiOffset = m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset;
        // ������� �������� �� ��������� �����.
        m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset += uiEncodedByteCounter;
        // �������� ������ ��������� ������.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiLength = uiLength;
        // �������� ������ �������������� ������.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiEncodedLength = uiEncodedByteCounter;
//        // �������� ���������� ������ ����������� � ���������.
//        m_xServiseSection.xServiseSectionData.uiBlocksNumber += 1;
    }

    // �������� ����������� ����� ����������� ������.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);
    // �������� ������ ���������� ������������� �����.
    m_xServiseSection.xServiseSectionData.uiLastWritedBlockNumber = uiBlock;

    if (m_pxStorageDevice -> Write(m_xServiseSection.xServiseSectionData.
                                   axBlockPositionData[uiBlock].uiOffset,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// ������� ������ ��������� ������ ����� �������� ��������� ������ � ��������� ������� ������.
uint8_t CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    // ������� �� ����� � ������?
    if (GetFsmState() != IDDLE)
    {
        SetFsmEvent(WRITE_ERROR_FSM_EVENT);
        return 0;
    }

    // ��������� ����� �� ������� ������?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // ��� ������.
        return 0;
    }

    // ������� ������ ��������� ������ �����.
    m_puiBlockSource = puiSource;
    m_uiBlockLength = uiLength;
    m_uiBlock = uiBlock;

    SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
    // �������� ������� ������.
    SetFsmState(START_WRITE_TEMPORARY_BLOCK_DATA);
}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CompareCurrentWithStoredCrc(void)
{
    // ������� ������� Crc ����� � ��� Crc ���������� � ��������� ����� � ���������� ������ ������.
    for (uint16_t i = SERVICE_SECTION_DATA_BLOCK_NUMBER;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        if (m_auiBlocksCurrentCrc[i] !=
                m_xServiseSection.xServiseSectionData.axBlockPositionData[i].uiCrc)
        {
            return false;
        }
    }

    // ��� ����� ����������� ������� ���� ������.
    return true;
}

//-----------------------------------------------------------------------------------------------------
// ���������� ������ ���� ���� ������ ������������ �������������.
void CDataStore::CrcOfBlocksCrcCreate(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER];

    // ������� Crc ���� ������, �� ������� ���������.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i + SERVICE_SECTION_DATA_BLOCK_NUMBER].uiCrc;
    }

    // �������� Crc ���� ������, �� ������� ���������.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc =
        usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                ((MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER) * sizeof(uint16_t)));

}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CrcOfBlocksCrcCheck(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER];

    // ������� Crc ���� ������, �� ������� ���������.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i + SERVICE_SECTION_DATA_BLOCK_NUMBER].uiCrc;
    }

    // Crc �� Crc ���� ������ ��������� � ���������� � ��������� �����?
    if (m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                    ((MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER) * sizeof(uint16_t))))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
// ��������� ����������� � ����������� ���������� ���������� ����� � ������ ��������.
// ����������� � ����������� ������ ������������ ����������� ����������� �
// ���������� �� ��������� ��������� ����� Crc �������� ����� ��������.
uint8_t CDataStore::TemporaryServiceSectionAndBlocksCheck(void)
{
    // ���� ��������?
    if (!(ReadTemporaryServiceSection()))
    {
        return 0;
    }

    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    cout << "TemporaryServiceSectionAndBlocksCheck uiBlocksNumber" << " " << (int)m_xServiseSection.xServiseSectionData.uiBlocksNumber << endl;
    for (uint16_t i = 0;
            i < m_xServiseSection.xServiseSectionData.uiBlocksNumber;
            i++)
    {
        // ���� ��������?
        if (!(ReadBlock(m_puiIntermediateBuff, i)))
        {
            // ��������� ������ ����� � ���������� Crc ��������,
            // ����� ���������� Crc ����� ����������� �� ��������� ������.
            m_uiBlock = i;
            // Crc ����� �� ���������� ������ ��������� � Crc �����
            // ����������� �� ��������� ��������� ������ �� �������� �������?
            if (CheckTemporaryBlock())
            {
                memcpy(auiTempArray,
                       m_puiIntermediateBuff,
                       m_xServiseSection.xServiseSectionData.
                       axBlockPositionData[i].uiLength);
                // ������ ����� ������� �������� �� ��������� ������,
                // �� ��� ������ � ��������� ��������� ����.
                // ��������� ��������� ������ ������ �� ��������� ������� � ���������.
                WriteBlock(auiTempArray,
                           m_xServiseSection.xServiseSectionData.
                           axBlockPositionData[i].uiLength,
                           i);

//                do
//                {
//                    Fsm();
//                }
//                while (GetFsmState() != CDataStore::IDDLE);

                // ���� �� ��������?
                if (!(ReadBlock(m_puiIntermediateBuff, i)))
                {
                    return 0;
                }
            }
        }
        else
        {
            // ���� ������������ ����� ����������� ������?
            if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
            {
                CHammingCodes::SetErrorCode(0);
                cout << "CHammingCodes::GetErrorCode uiBlock" << (int)i << endl;
                memcpy(auiTempArray,
                       m_puiIntermediateBuff,
                       m_xServiseSection.xServiseSectionData.
                       axBlockPositionData[i].uiLength);
                // ��������� ��������� ������ ������ �� ��������� ������� � ���������.
                WriteBlock(auiTempArray,
                           m_xServiseSection.xServiseSectionData.
                           axBlockPositionData[i].uiLength,
                           i);

                do
                {
                    Fsm();
                }
                while (GetFsmState() != CDataStore::IDDLE);

                // ���� �� ��������?
                if (!(ReadBlock(m_puiIntermediateBuff, i)))
                {
                    return 0;
                }
            }
        }

    }

    return 1;
}

//-----------------------------------------------------------------------------------------------------
// ��������� ����������� � ����������� ���������� ����� � ������ ��������.
// ����������� � ����������� ������ ������������ ����������� ����������� �
// ���������� � ��������� ����� Crc �������� ����� ��������.
uint8_t CDataStore::ServiceSectionAndBlocksCheck(void)
{
    // ���� ��������?
    if (!(ReadServiceSection()))
    {
        return 0;
    }

    for (uint16_t i = 0;
            i < m_xServiseSection.xServiseSectionData.uiBlocksNumber;
            i++)
    {
        // ���� ��������?
        if (!(ReadBlock(m_puiIntermediateBuff, i)))
        {
            return 0;
        }
    }

    return 1;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::Check(void)
{
    enum
    {
        IDDLE = 0,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK,

        DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED,
        DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED,

        DATA_STORE_NOT_CORRUPTED,
        DATA_STORE_CORRUPTED,
    };

    uint8_t uiFsmState;
    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
    uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK;


    while (1)
    {
        switch (uiFsmState)
        {
        case IDDLE:
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK:
            // ����� �� ���������� � ������� � ��������� ��������� ������?
            if (TemporaryServiceSectionAndBlocksCheck())
            {
                uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK;
            }
            else
            {
                uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK;
            }
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK:
            // ����� �� ���������� � ������� �� ��������� ������?
            if (ServiceSectionAndBlocksCheck())
            {
                uiFsmState = DATA_STORE_NOT_CORRUPTED;
            }
            else
            {
                uiFsmState = DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED;
            }
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK:
            // ����� �� ���������� � ������� �� ��������� ������?
            if (ServiceSectionAndBlocksCheck())
            {
                uiFsmState = DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED;
            }
            else
            {
                uiFsmState = DATA_STORE_CORRUPTED;
            }
            break;

        case DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED:
            uiFsmState = DATA_STORE_NOT_CORRUPTED;
            break;

        case DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED:
            uiFsmState = DATA_STORE_NOT_CORRUPTED;
            break;

        case DATA_STORE_CORRUPTED:
            return 0;
            break;

        case DATA_STORE_NOT_CORRUPTED:
            // ������ �� ����������.
            return 1;
            break;

        default:
            return 0;
            break;
        }
    }
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Fsm(void)
{
    switch (GetFsmState())
    {
    case IDDLE:
        break;


//-----------------------------------------------------------------------------------------------------
        // ������ ����� �� ��������� �����.
    case START_WRITE_TEMPORARY_BLOCK_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // ��������� ����� �������� ���������� � ������.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_TEMPORARY_BLOCK_DATA);
        break;

    case READY_TO_WRITE_WAITING_TEMPORARY_BLOCK_DATA:
        // ���������� �������� ������ � ������?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (WriteTemporaryBlock())
            {
                SetFsmState(WRITE_END_WAITING_TEMPORARY_BLOCK_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_TEMPORARY_BLOCK_DATA:
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (CheckTemporaryBlock())
            {
                cerr << "CheckTemporaryBlock ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_TEMPORARY_SERVICE_SECTION_DATA);
            }
            else
            {
                cerr << "CheckTemporaryBlock error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // ������ ���������� ���������� �����.
    case START_WRITE_TEMPORARY_SERVICE_SECTION_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // ��������� ����� �������� ���������� � ������.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_TEMPORARY_SERVICE_SECTION_DATA);
        break;

    case READY_TO_WRITE_WAITING_TEMPORARY_SERVICE_SECTION_DATA:
        // ���������� �������� ������ � ������?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (WriteTemporaryServiceSection())
            {
                SetFsmState(WRITE_END_WAITING_TEMPORARY_SERVICE_SECTION_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_TEMPORARY_SERVICE_SECTION_DATA:
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (ReadTemporaryServiceSection())
            {
                cerr << "ReadTemporaryServiceSection ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_BLOCK_DATA);
            }
            else
            {
                cerr << "ReadTemporaryServiceSection error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // ������ �����.
    case START_WRITE_BLOCK_DATA:
        // ��������� ����� �������� ���������� � ������.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_BLOCK_DATA);
        break;

    case READY_TO_WRITE_WAITING_BLOCK_DATA:
        // ���������� �������� ������ � ������?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������ ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (WriteBlock())
            {
                SetFsmState(WRITE_END_WAITING_BLOCK_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_BLOCK_DATA:
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (CheckBlock())
            {
                cerr << "CheckBlock ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
            }
            else
            {
                cerr << "CheckBlock error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // ������ ���������� �����.
    case START_WRITE_SERVICE_SECTION_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // ��������� ����� �������� ���������� � ������.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA);
        break;

    case READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA:
        // ���������� �������� ������ � ������?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (WriteServiceSection())
            {
                SetFsmState(WRITE_END_WAITING_SERVICE_SECTION_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ���������� � ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_SERVICE_SECTION_DATA:
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (ReadServiceSection())
            {
                cerr << "ReadServiceSection ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(IDDLE);
            }
            else
            {
                cerr << "ReadServiceSection error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // ����� �������� ��������� ������ �����������?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


    default:
        SetFsmEvent(WRITE_ERROR_FSM_EVENT);
        SetFsmState(IDDLE);
        break;
    }
}

//-----------------------------------------------------------------------------------------------------














//    case BLOCK_CHECK_START:
//        // ��������� �� ��� �����?
//        if (uiBlockCounter < MAX_BLOCKS_NUMBER)
//        {
//            uiLength = ReadBlock(m_puiIntermediateBuff, uiBlockCounter);
//            // ���� �� ��������?
//            if (uiLength)
//            {
//                // ���� ������������ ����� ����������� ������?
//                if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
//                {
//                    // ������� ��������������� ���� � ���������.
//                    uiFsmState = BLOCK_WRITE_START;
//                }
//                else
//                {
//                    uiFsmState = NEXT_BLOCK;
//                }
//            }
//            else
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//        }
//        else
//        {
//            uiFsmState = ALL_BLOCKS_CHECKED;
//        }
//        break;
//
//    case BLOCK_WRITE_START:
//        // �������� ������ � ���������.
//        // ���� �� ������ � ������?
//        if (WriteBlock(m_puiIntermediateBuff, uiLength, uiBlockCounter))
//        {
//            uiFsmState = BLOCK_WRITE_END_WAITING;
//        }
//        // ��� ������ ����� �� ��������� ������?
//        else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//        {
//            uiFsmState = BLOCK_ERROR;
//        }
//        break;
//
//    case BLOCK_WRITE_END_WAITING:
//        // ���� ������� �������?
//        if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
//        {
//            uiFsmState = NEXT_BLOCK;
//        }
//        // ��� ������ ����� �� ��������� ������?
//        else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//        {
//            uiFsmState = BLOCK_ERROR;
//        }
//        break;

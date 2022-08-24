
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
    m_xServiseSection.xServiseSectionData.uiStoredBlocksNumber = 0;
    // ��������� ������� - ��������� �� ���������.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
}

//-----------------------------------------------------------------------------------------------------
// ������� ������ ���������� ���������� ����� � ������.
// �������� �� ��������� �������� �
// ������� �������� ������ ���������� ��������.
uint8_t CDataStore::TemporaryServiceSectionWritePrepare(void)
{
    // �������� ����������� �����.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    // ���������� ������ ���������� ��������.
    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> PassingDataAndStartWrite(TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
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
// ������� ������ ���������� ����� � ������.
// �������� �� ��������� �������� �
// ������� �������� ������ ���������� ��������.
uint8_t CDataStore::ServiceSectionWritePrepare(void)
{
    // �������� ����������� �����.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    // ���������� ������ ���������� ��������.
    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> PassingDataAndStartWrite(SERVICE_SECTION_DATA_BEGIN,
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
// ��������� � ��������� ����������� ���������� ���������� �����.
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
// ��������� � ��������� ����������� ���������� �����.
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
// ��������� � ��������� ����������� ���������� ������ �����.
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
// ��������� � ��������� ����������� �����.
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
// ��������� � ��������� ����������� �����.
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
    CHammingCodes::HammingCodesToBytes(m_puiIntermediateBuff, m_puiIntermediateBuff, uiEncodedLength);

    // ���� �� ��������?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(m_puiIntermediateBuff, uiLength))
    {
        memcpy(puiDestination, m_puiIntermediateBuff, uiLength);
        return uiLength;
    }
    else
    {
        // ��� ������.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// ������� ������ ����� � ������ �� ��������� �����.
// �������� �� ��������� ��������, ��������� ����� ��� �������� �
// ������� �������� ������ ���������� ��������.
uint8_t CDataStore::TemporaryBlockWritePrepare(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    // ���������� ������ ���������� ��������.
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
        m_xServiseSection.xServiseSectionData.uiStoredBlocksNumber += 1;
    }

    // �������� ����������� ����� ����������� ������.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);

    if (m_pxStorageDevice -> PassingDataAndStartWrite(TEMPORARY_BLOCK_DATA_BEGIN,
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
// ������� ������ ����� � ������.
// �������� �� ��������� ��������, ��������� ����� ��� �������� �
// ������� �������� ������ ���������� ��������.
uint8_t CDataStore::BlockWritePrepare(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    // ���������� ������ ���������� ��������.
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
//        m_xServiseSection.xServiseSectionData.uiStoredBlocksNumber += 1;
    }

    // �������� ����������� ����� ����������� ������.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);
//    // �������� ������ ���������� ������������� �����.
//    m_xServiseSection.xServiseSectionData.uiLastWritedBlockNumber = uiBlock;

    if (m_pxStorageDevice -> PassingDataAndStartWrite(m_xServiseSection.xServiseSectionData.
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
// ������ ����� � ��������� �������� ��������.
// 1 - CDataStore::PassingBlockDataAndStartWrite(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock).
// 2 - CDataStore::BlockWritePrepare(void).
// 3 - CStorageDevice::PassingDataAndStartWrite(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength).

// ������� ������ ��������� ������ ����� �������� ��������� ������ � ��������� ������� ������.
uint8_t CDataStore::PassingBlockDataAndStartWrite(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
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
// ���������� ������ ���� ���� ������ ������������ �������������.
void CDataStore::CrcOfBlocksCrcCreate(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER];

    // ������� Crc ���� ������.
    for (uint16_t i = 0;
            i < MAX_BLOCKS_NUMBER;
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i].uiCrc;
    }

    // �������� Crc ���� ������.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc =
        usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                (MAX_BLOCKS_NUMBER * sizeof(uint16_t)));

}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CrcOfBlocksCrcCheck(void)
{
    // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
    // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
    // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
    // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
    // ������ ������������ ��� ������ ���� ������.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER];

    // ������� Crc ���� ������.
    for (uint16_t i = 0;
            i < MAX_BLOCKS_NUMBER;
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i].uiCrc;
    }

    // Crc �� Crc ���� ������ ��������� � ���������� � ��������� �����?
    if (m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                    (MAX_BLOCKS_NUMBER * sizeof(uint16_t))))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
// ��������� ����������� ������ ���������.
// ��������������� ����������� ������ � ������� ��������� ��������.
// ��������������� ������ ������������� ����� ��� ���� ������� � �.�.
uint8_t CDataStore::Check(void)
{
    enum
    {
        IDDLE = 0,

        TEMPORARY_SERVICE_SECTION_DATA_CHECK,
        TEMPORARY_SERVICE_SECTION_LINKED_BLOCKS_CHECK,

        CORRUPTED_BLOCK_RECOVERY_WRITE_END_WAITING,
        SERVICE_SECTION_DATA_WRITE_START,
        SERVICE_SECTION_DATA_WRITE_END_WAITING,

        SERVICE_SECTION_DATA_CHECK,
        SERVICE_SECTION_LINKED_BLOCKS_CHECK,

        DATA_STORE_NEW_VERSION_ACCEPTED,
        DATA_STORE_OLD_VERSION_ACCEPTED,

        DATA_STORE_CHECK_OK,
        DATA_STORE_CHECK_ERROR,
        DATA_STORE_CHECK_REPEAT,
    };

    enum
    {
        // ���������� ������� �������������� ���������.
        RECOVERY_ATTEMPTS_NUMBER = 3,
    };

    uint8_t uiLocalFsmState;
    uint8_t uiRecoveryAttemptCounter = 0;
    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // ������� ������ ������������� ���������� ��������.
    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
    // �������� ������� �������� � �������������� ���������.
    uiLocalFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK;

    while (1)
    {
        switch (uiLocalFsmState)
        {
        case IDDLE:
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK:
            // ��������� ��������� ���� �� ��������?
            if (ReadTemporaryServiceSection())
            {
                uiLocalFsmState = TEMPORARY_SERVICE_SECTION_LINKED_BLOCKS_CHECK;
            }
            else
            {
                // ��������� ��������� ���� ��������, �������� �� ����� ���������� ������ ������.
                // �������� ����������� ��������� �� ����������� ���������� �����.
                uiLocalFsmState = SERVICE_SECTION_DATA_CHECK;
            }
            break;

        case TEMPORARY_SERVICE_SECTION_LINKED_BLOCKS_CHECK:
            // �������� ����������� ������ ��������� � ��������� ��������� ������, � ������������� �� �����������.
            for (uint16_t i = 0;
                    i < m_xServiseSection.xServiseSectionData.uiStoredBlocksNumber;
                    i++)
            {
                // ���� �� ������ � ��������� ��������� �������(��� ��������)?
                if (!(ReadBlock(auiTempArray, i)))
                {
                    // ��������� ������ �����, � ���������� Crc ��������,
                    // ����� ���������� Crc ����� ���������� �� ��������� ������.
                    m_uiBlock = i;
                    // Crc ����� �� ���������� ������ ��������� � Crc �����
                    // ����������� �� ��������� ��������� ������ �� �������� �������?
                    if (CheckTemporaryBlock())
                    {
                        // ��������� ������ ����� ��������� ��� �������� �� ��������������� �����.
                        memcpy(auiTempArray,
                               m_puiIntermediateBuff,
                               m_xServiseSection.xServiseSectionData.
                               axBlockPositionData[i].uiLength);
                        // ������ ����� ������� �������� �� ��������� ������,
                        // �� ��� ������ � ��������� ��������� ����.
                        // ��������� ��������� ������ ������ ����� �� ��������� ������� � ���������.
                        PassingBlockDataAndStartWrite(auiTempArray,
                                                      m_xServiseSection.xServiseSectionData.
                                                      axBlockPositionData[i].uiLength,
                                                      i);

                        uiLocalFsmState = CORRUPTED_BLOCK_RECOVERY_WRITE_END_WAITING;
                        break;
                    }
                    else
                    {
                        // ���� �� ������ � ��������� ��������� �������.
                        // �������� ��������� ������ �� ����� ������ ���������� ���������� �����.
                        // ��������� ��������.
                        uiLocalFsmState = SERVICE_SECTION_DATA_CHECK;
                        break;
                    }
                }
                else
                {
                    // ���� ������������ ���������� �������� ����� ����������� ������?
                    if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
                    {
                        // ������� ������ ������������� ���������� ��������.
                        CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
                        cout << "CHammingCodes::GetErrorCode 1 uiBlock" << (int)i << endl;

                        // ��������� ��������� ������ ������ � ���������.
                        PassingBlockDataAndStartWrite(auiTempArray,
                                                      m_xServiseSection.xServiseSectionData.
                                                      axBlockPositionData[i].uiLength,
                                                      i);

                        uiLocalFsmState = CORRUPTED_BLOCK_RECOVERY_WRITE_END_WAITING;
                        break;
                    }
                }

                // �������.
                uiLocalFsmState = SERVICE_SECTION_DATA_WRITE_START;
            }
            break;

        case CORRUPTED_BLOCK_RECOVERY_WRITE_END_WAITING:
            // ������� ��������� ������ ��������� ���������.
            // ���������� �� ��������� ������ ���� ������� � ���������?
            if (GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
            {
                uiLocalFsmState = DATA_STORE_CHECK_REPEAT;
            }
            // ��� ������ ����� ��������� ������?
            else if (GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
            {
                uiLocalFsmState = DATA_STORE_CHECK_REPEAT;
            }
            break;

        case SERVICE_SECTION_DATA_WRITE_START:
            // ����� ��������� � ���������� ���������� �����, ������������� ����.
            // ��� �������, ��������� ����� ������ �� ��������� ������ ������ ������.
            // �� ����� ��������� ����������� � ���������� ���������� �����, ������� �����.
            // ������� ��������� ����.
            SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
            // �������� ������� ������ ���������� �����.
            SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
            uiLocalFsmState = SERVICE_SECTION_DATA_WRITE_END_WAITING;
            break;

        case SERVICE_SECTION_DATA_WRITE_END_WAITING:
            // ������� ��������� ������ ��������� ���������.
            // ��������� ���� ������� � ���������?
            if (GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
            {
                // ��������� ���� �� ��������?
                if (ReadServiceSection())
                {
                    uiLocalFsmState = DATA_STORE_NEW_VERSION_ACCEPTED;
                }
                else
                {
                    uiLocalFsmState = DATA_STORE_CHECK_REPEAT;
                }
            }
            // ��� ������ ����� ��������� ������?
            else if (GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
            {
                uiLocalFsmState = DATA_STORE_CHECK_REPEAT;
            }
            break;

        case SERVICE_SECTION_DATA_CHECK:
            // �� ����� ���� ��������� ��������� ���� �� ������ � ������� �������� ������(��� ��������).
            // ��������� ���� �� ����� ������. ���������� ������� ���������� ��������� ���������.
            // ��������� ���� �� ��������?
            if (ReadServiceSection())
            {
                uiLocalFsmState = SERVICE_SECTION_LINKED_BLOCKS_CHECK;
            }
            else
            {
                uiLocalFsmState = DATA_STORE_CHECK_ERROR;
            }
            break;

        case SERVICE_SECTION_LINKED_BLOCKS_CHECK:
            // �������� ����������� ������ ��������� �� ��������� ������, � ������������� �� �����������.
            for (uint16_t i = 0;
                    i < m_xServiseSection.xServiseSectionData.uiStoredBlocksNumber;
                    i++)
            {
                // ���� �� ������ �� ��������� �������(��� ��������)?
                if (!(ReadBlock(auiTempArray, i)))
                {
                    // ���� �� ������ �� ��������� �������.
                    // �������� ��������� ������ �� ����� ������ ���������� �����.
                    // ������������ ������ ������.
                    uiLocalFsmState = DATA_STORE_CHECK_ERROR;
                    break;
                }
                else
                {
                    // ���� ������������ ���������� �������� ����� ����������� ������?
                    if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
                    {
                        // ������� ������ ������������� ���������� ��������.
                        CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
                        cout << "CHammingCodes::GetErrorCode 2 uiBlock" << (int)i << endl;

                        // ��������� ��������� ������ ������ � ���������.
                        PassingBlockDataAndStartWrite(auiTempArray,
                                                      m_xServiseSection.xServiseSectionData.
                                                      axBlockPositionData[i].uiLength,
                                                      i);

                        uiLocalFsmState = CORRUPTED_BLOCK_RECOVERY_WRITE_END_WAITING;
                        break;
                    }
                }

                // �������.
                // ����� ��������� � ���������� �����, ������������� ����.
                // ����������� ���������� ����� ���������.
                uiLocalFsmState = DATA_STORE_OLD_VERSION_ACCEPTED;
            }
            break;

        case DATA_STORE_NEW_VERSION_ACCEPTED:
            // ��������� ���������.
            cerr << "DATA_STORE_NEW_VERSION_ACCEPTED" << endl;
            return 1;
            break;

        case DATA_STORE_OLD_VERSION_ACCEPTED:
            // ��������� �� ���������.
            cerr << "DATA_STORE_OLD_VERSION_ACCEPTED" << endl;
            return 1;
            break;

        case DATA_STORE_CHECK_OK:
            return 1;
            break;

        case DATA_STORE_CHECK_ERROR:
            // ��������� ����������.
            cerr << "DATA_STORE_CHECK_ERROR" << endl;
            return 0;
            break;

        case DATA_STORE_CHECK_REPEAT:
            // ��������� �������� � ������� ��������������.
            // ��� ���� ����������� ��� ��������������?
            if (uiRecoveryAttemptCounter < RECOVERY_ATTEMPTS_NUMBER)
            {
                uiRecoveryAttemptCounter++;
                // �������� ��������� �������� ��������� ����� ��������������.
                uiLocalFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK;
            }
            else
            {
                uiLocalFsmState = DATA_STORE_CHECK_ERROR;
            }
            break;

        default:
            return 0;
            break;
        }

        Fsm();
    }
}

//-----------------------------------------------------------------------------------------------------
// ������� ������� ������ ���������.
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
            // ������� ���� - ������ ���������.
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ �� ��������� ����� ������ �������?
            if (TemporaryBlockWritePrepare())
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
        // ������� ��������� ������ ��������� ���������� ��������.
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (CheckTemporaryBlock())
            {
                cerr << "CheckTemporaryBlock ok" << endl;
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
            // ������� ���� - ������ ���������.
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (TemporaryServiceSectionWritePrepare())
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
        // ������� ��������� ������ ��������� ���������� ��������.
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (ReadTemporaryServiceSection())
            {
                cerr << "ReadTemporaryServiceSection ok" << endl;
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
            // ������� ���� - ������ ���������.
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (BlockWritePrepare())
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
        // ������� ��������� ������ ��������� ���������� ��������.
        // ���������� �������� ��������� ������?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // ������ ������ �������?
            if (CheckBlock())
            {
                cerr << "CheckBlock ok" << endl;
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
            // ������� ���� - ������ ���������.
            m_pxStorageDevice -> SetIsDataWrited(false);
            // ���������� � ������ ������ �������?
            if (ServiceSectionWritePrepare())
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
        // ������� ��������� ������ ��������� ���������� ��������.
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












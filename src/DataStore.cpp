
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
//    delete m_pxStorageDevice;
    delete m_puiIntermediateBuff;
    delete m_pxStorageDevice;

}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Init(void)
{
//    SetFsmState(IDDLE);
//
//    // ������� ��������� ��������.
//    memset(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
//           0,
//           sizeof(m_xServiseSection.xServiseSectionData));
//
//    // ��������� �������� �� ������ ��������� ������.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiOffset = SERVICE_SECTION_DATA_BEGIN;
//    // ��������� ������ ��������� ������.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiLength = sizeof(struct TServiseSectionData);
//    // ���������� ���� ���������� ���������
//    // ������ ���� �������� �������.
//    // ��� ��� �� ���� ����������� ��������� ������.
//    // ������������ ����������������������� ��� ��������(8,4).
//    // ����������� - 1.5: ���� ���� ������������� � ������� ����� 12 ���,
//    // �� ���� ���� �������� ������ ���������� ��� ����� ������������.
//    // ��������� ������ �������������� ������.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiEncodedLength =
//        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSectionData) + CRC_LENGTH));
//    // ��������� �������� ��� ���������� ���������.
//    m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset =
//        (SERVICE_SECTION_DATA_BEGIN +
//         CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSectionData) + CRC_LENGTH));
//    // ��������� ������� - ���� ������ �� ������������ �������������.
//    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
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
//uint16_t CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
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
//uint16_t CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
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
    }

    // �������� ����������� ����� ����������� ������.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);

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
    // ������� ������� Crc ����� � ��� Crc ����������� � ��������� ����� � ���������� ������ ������.
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

    // Crc �� Crc ���� ������ ��������� � ����������� � ��������� �����?
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
uint8_t CDataStore::Check(void)
{
//    CDataStore::Init();
//
//    // ��������� ���� ��������?
//    if (!(ReadBlock(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData), SERVICE_SECTION_DATA)))
//    {
//        return 0;
//    }
//
//    enum
//    {
//        IDDLE = 0,
//        BLOCK_CHECK_START,
//        BLOCK_WRITE_START,
//        BLOCK_WRITE_END_WAITING,
//        NEXT_BLOCK,
//        BLOCK_ERROR,
//        ALL_BLOCKS_CHECKED,
//    };
//
//    uint8_t uiBlockCounter = 0;
//    uint8_t uiFsmState = BLOCK_CHECK_START;
//    uint16_t uiLength;
//
//    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
//
//    while (1)
//    {
//        switch (uiFsmState)
//        {
//        case BLOCK_CHECK_START:
//            // ��������� �� ��� �����?
//            if (uiBlockCounter < MAX_BLOCKS_NUMBER)
//            {
//                uiLength = ReadBlock(m_puiIntermediateBuff, uiBlockCounter);
//                // ���� �� ��������?
//                if (uiLength)
//                {
//                    // ���� ������������ ����� ����������� ������?
//                    if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
//                    {
//                        // ������� ��������������� ���� � ���������.
//                        uiFsmState = BLOCK_WRITE_START;
//                    }
//                    else
//                    {
//                        uiFsmState = NEXT_BLOCK;
//                    }
//                }
//                else
//                {
//                    uiFsmState = BLOCK_ERROR;
//                }
//            }
//            else
//            {
//                uiFsmState = ALL_BLOCKS_CHECKED;
//            }
//            break;
//
//        case BLOCK_WRITE_START:
//            // �������� ������ � ���������.
//            // ���� �� ������ � ������?
//            if (WriteBlock(m_puiIntermediateBuff, uiLength, uiBlockCounter))
//            {
//                uiFsmState = BLOCK_WRITE_END_WAITING;
//            }
//            // ��� ������ ����� �� ��������� ������?
//            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//            break;
//
//        case BLOCK_WRITE_END_WAITING:
//            // ���� ������� �������?
//            if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
//            {
//                uiFsmState = NEXT_BLOCK;
//            }
//            // ��� ������ ����� �� ��������� ������?
//            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//            break;
//
//        case NEXT_BLOCK:
//            uiBlockCounter++;
//            uiFsmState = BLOCK_CHECK_START;
//            break;
//
//        case ALL_BLOCKS_CHECKED:
//            // ������ �� ����������.
//            return 1;
//            break;
//
//        case BLOCK_ERROR:
//            return 0;
//            break;
//
//        default:
//            break;
//        }
//
//        CDataStore::Fsm();
//
//        delay_ms(10);
//
//        CPlatform::WatchdogReset();
//    }
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
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
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
        if (m_pxStorageDevice -> IsDataWrited())
        {
            SetFsmEvent(WRITE_OK_FSM_EVENT);
            SetFsmState(START_WRITE_TEMPORARY_SERVICE_SECTION_DATA);
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
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
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
        if (m_pxStorageDevice -> IsDataWrited())
        {
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
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������ ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
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
        if (m_pxStorageDevice -> IsDataWrited())
        {
            SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
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
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // ��������� ����� �������� ��������� ������.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
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
        if (m_pxStorageDevice -> IsDataWrited())
        {
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
        break;
    }
}

//            // ����������� ��������� ������.
//            ReadServiceSection();
//-----------------------------------------------------------------------------------------------------








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
void CStorageDeviceFileSystem::Fsm(void)
{
//    enum
//    {
//        IDDLE = 0,
//        START_WRITE,
//        READY_TO_WRITE_WAITING,
//        WRITE_END_WAITING,
//
//        START_WRITE_SERVICE_SECTION_DATA,
//        READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA,
//        WRITE_END_WAITING_SERVICE_SECTION_DATA,
//    };
//
//    switch (GetFsmState())
//    {
//    case IDDLE:
//        break;
//
//    case START_WRITE:
//        // ��������� ����� �������� ���������� � ������.
//        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
//        SetFsmState(READY_TO_WRITE_WAITING);
//        break;
//
//    case READY_TO_WRITE_WAITING:
//        if (m_pxStorageDevice -> IsReadyToWrite())
//        {
//            // ��������� ����� �������� ��������� ������ ������.
//            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
    //    m_pxStorageDevice -> SetIsDataWrited(false);
//            m_pxStorageDevice -> Write();
//            SetFsmState(WRITE_END_WAITING);
//        }
//        // ����� �������� ���������� � ������ �����������?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case WRITE_END_WAITING:
//        if (m_pxStorageDevice -> IsDataWrited())
//        {
//            SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
//        }
//        // ����� �������� ��������� ������ �����������?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//
//        // ������ ���������� �����.
//    case START_WRITE_SERVICE_SECTION_DATA:
//        // ������ ���������.
//        // �������� ���������� ��������� ��������.
//        if (Write(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
//                  sizeof(m_xServiseSection.xServiseSectionData),
//                  SERVICE_SECTION_DATA))
//        {
//            // ��������� ����� �������� ���������� � ������.
//            GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
//            SetFsmState(READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA);
//        }
//        else
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA:
//        if (m_pxStorageDevice -> IsReadyToWrite())
//        {
//            // ��������� ����� �������� ��������� ������.
//            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
//    m_pxStorageDevice -> SetIsDataWrited(false);
    //            m_pxStorageDevice -> Write();
//            SetFsmState(WRITE_END_WAITING_SERVICE_SECTION_DATA);
//        }
//        // ����� �������� ���������� � ������ �����������?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case WRITE_END_WAITING_SERVICE_SECTION_DATA:
//        if (m_pxStorageDevice -> IsDataWrited())
//        {
//            SetFsmEvent(WRITE_OK_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        // ����� �������� ��������� ������ �����������?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    default:
//        break;
//    }
}

//-----------------------------------------------------------------------------------------------------




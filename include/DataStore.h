//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDATASTORE_H
#define CDATASTORE_H

#include <stdint.h>
#include <iostream>
#include <fstream>

#include "Dfa.h"
#include "Timer.h"
#include "StorageDevice.h"

using namespace std;

//-----------------------------------------------------------------------------------------------------
// ��������� ������ ������.
// ��������� ����������� ������ ���������.
// ��������������� ����������� ������ � ������� ��������� ��������.
// ��������������� ������ ������������� ����� ��� ���� ������� � �.�.
// ������ ���������:
// 1 - ���� ��������� ������.
// 2 - ����������� ���� ��� ������ ��������� ��������� ������.
// 3 - ��������� ����� ������������� �����.
// 4 - ������������ � ��������������� �������������� ��������� �������.
class CDataStore : public CDfa
{
public:
    CDataStore();
    CDataStore(CStorageDevice* pxStorageDevice);
    virtual ~CDataStore();

    uint8_t Check(void);
    void CreateServiceSection(void);
    uint8_t PassingBlockDataAndStartWrite(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock);
    uint16_t ReadBlock(uint8_t * , uint8_t );
    void CrcOfBlocksCrcCreate(void);
    bool CrcOfBlocksCrcCheck(void);
    void Fsm(void);
    uint8_t GetBlockLength(uint8_t uiBlock)
    {
        return m_xServiseSection.xServiseSectionData.
               axBlockPositionData[uiBlock].uiLength;
    };


private:
    uint8_t TemporaryServiceSectionWritePrepare(void);
    uint8_t ServiceSectionWritePrepare(void);
    uint8_t ReadTemporaryServiceSection(void);
    uint8_t ReadServiceSection(void);
    uint8_t CheckTemporaryBlock(void);
    uint8_t CheckBlock(void);
    uint8_t TemporaryBlockWritePrepare(void);
    uint8_t BlockWritePrepare(void);
    CTimer* GetTimerPointer(void)
    {
        return &m_xTimer;
    };

public:
    enum
    {
        CRC_LENGTH = 2,
        TAIL_LENGTH = 2,
        MAX_BLOCK_LENGTH = 256,
        MAX_ENCODED_BLOCK_LENGTH =
            ((MAX_BLOCK_LENGTH + TAIL_LENGTH) + ((MAX_BLOCK_LENGTH + TAIL_LENGTH) / 2)),
        MAX_BLOCKS_NUMBER = 10,//(TDataBase::BLOCKS_QUANTITY + SERVICE_SECTION_DATA_BLOCK_NUMBER),
    };

    enum
    {
        READY_TO_WRITE_WAITING_TIMEOUT = 200,
        WRITE_END_WAITING_TIMEOUT = 10000,
    };

    enum
    {
        IDDLE = 0,

        START_WRITE_BLOCK_DATA,
        READY_TO_WRITE_WAITING_BLOCK_DATA,
        WRITE_END_WAITING_BLOCK_DATA,

        START_WRITE_TEMPORARY_BLOCK_DATA,
        READY_TO_WRITE_WAITING_TEMPORARY_BLOCK_DATA,
        WRITE_END_WAITING_TEMPORARY_BLOCK_DATA,

        START_WRITE_TEMPORARY_SERVICE_SECTION_DATA,
        READY_TO_WRITE_WAITING_TEMPORARY_SERVICE_SECTION_DATA,
        WRITE_END_WAITING_TEMPORARY_SERVICE_SECTION_DATA,

        START_WRITE_SERVICE_SECTION_DATA,
        READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA,
        WRITE_END_WAITING_SERVICE_SECTION_DATA,
    };

    enum
    {
        NO_EVENT_FSM_EVENT = 0,
        WRITE_IN_PROGRESS_FSM_EVENT,
        STORAGE_DEVICE_BUSY_FSM_EVENT,
        WRITE_OK_FSM_EVENT,
        WRITE_ERROR_FSM_EVENT,
    };

    struct TBlockPositionData
    {
        // �������� �� ������ �����.
        uint16_t uiOffset;
        // ������ ��������� �����.
        uint16_t uiLength;
        // ������ ��������������� ��������� �����.
        uint16_t uiEncodedLength;
        // Crc ����� �������� �������� �� ���� � ��������� �����.
        // �� ��� ���������� ������������ ����������� ����� � ��� �������������� � ���������.
        uint16_t uiCrc;
    };

    struct TServiseSectionData
    {
        // �������� �� ��������� ����� ��� ������ ������ �����.
        uint16_t uiFreeSpaceOffset;
        // ������ ���������� �����.
        uint16_t uiLength;
        // ������ �������������� ���������� �����.
        // ������������ ����������������������� ��� ��������(8,4).
        // ����������� - 1.5: ���� ���� ������������� � ������� ����� 12 ���,
        // �� ���� ���� �������� ������ ���������� ��� ����� ������������.
        uint16_t uiEncodedLength;
//        uint16_t uiLastWritedBlockNumber;
        uint16_t uiStoredBlocksNumber;
        // ����������� ����� ����������� �� ������� ����������� ���� ������, �� ������� ���������.
        // ����������� ��� ������ � ����������� ������� ����� ������ ����� ������������.
        // ���� ��� � ������������ ��������������� � ���, ��� ���� ������ ������� �� ��������,
        // � �� ������������ �������������. � ���� ������ ������ ��������� � ����� ������������ �� ������,
        // ������ ������������ ��� ������ ���� ������.
        uint16_t uiCrcOfBlocksCrc;
        TBlockPositionData axBlockPositionData[MAX_BLOCKS_NUMBER];
    };

    struct TServiseSection
    {
        TServiseSectionData xServiseSectionData;
        // Crc ���������� �����.
        uint16_t uiCrc;
    };

    enum
    {
        // ������� ���� ����� ���� ���� ��� ���� �������.
        TEMPORARY_BLOCK_DATA_BEGIN = 8,
        TEMPORARY_SERVICE_SECTION_DATA_BEGIN =
            (TEMPORARY_BLOCK_DATA_BEGIN + MAX_ENCODED_BLOCK_LENGTH),
        SERVICE_SECTION_DATA_BEGIN =
            TEMPORARY_SERVICE_SECTION_DATA_BEGIN +
            (sizeof(struct TServiseSection) + (sizeof(struct TServiseSection) / 2)),
    };


protected:
private:
    // ������ ��������� ������ �����.
    uint8_t m_uiBlock;
    uint8_t* m_puiBlockSource;
    uint16_t m_uiBlockLength;

    // ��������� �� ������ ������ ���������� ��������.
    CStorageDevice* m_pxStorageDevice;
    // ��������� ������ ������� ��������.
    TServiseSection m_xServiseSection;
    // ������ ����������� ���� ������.
    uint16_t m_auiBlocksCurrentCrc[MAX_BLOCKS_NUMBER];
    // ��������������� �����.
    uint8_t* m_puiIntermediateBuff;
    CTimer m_xTimer;
};

//-----------------------------------------------------------------------------------------------------
#endif // CDATASTORE_H

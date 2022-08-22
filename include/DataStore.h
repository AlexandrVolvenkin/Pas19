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

#include "DataTypes.h"
#include "Dfa.h"
#include "Timer.h"

using namespace std;

//-----------------------------------------------------------------------------------------------------
class CStorageDevice : public CDfa
{
public:
    virtual void WritePrepare(uint16_t uiDestination, uint8_t *puiSource, uint16_t uiLength) = 0;
    virtual uint8_t Write(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength) = 0;
    virtual uint8_t Write(void) = 0;
    virtual uint8_t Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength) = 0;
    virtual void SetIsDataWrited(bool bStatus) = 0;
    virtual bool IsDataWrited(void) = 0;
    virtual bool IsReadyToWrite(void) = 0;

    virtual void SetBufferPointer(uint8_t* puiBuffer)
    {
        m_puiBuffer = puiBuffer;
    };

    virtual uint8_t* GetBufferPointer(void)
    {
        return m_puiBuffer;
    };

    virtual void SetOffset(uint16_t uiOffset)
    {
        m_uiOffset = uiOffset;
    };
    virtual uint16_t GetOffset(void)
    {
        return m_uiOffset;
    };

    virtual void SetLength(uint16_t uiLength)
    {
        m_uiLength = uiLength;
    };
    virtual uint16_t GetLength(void)
    {
        return m_uiLength;
    };

    virtual void SetByteCounter(uint16_t nuiByteCounter)
    {
        m_nuiByteCounter = nuiByteCounter;
    };
    virtual uint16_t GetByteCounter(void)
    {
        return m_nuiByteCounter;
    };

protected:
    uint8_t* m_puiBuffer;
    uint16_t m_uiOffset;
    uint16_t m_nuiByteCounter;
    uint16_t m_uiLength;
};
//-----------------------------------------------------------------------------------------------------







//-----------------------------------------------------------------------------------------------------
class CStorageDeviceFileSystem : public CStorageDevice
{
public:

    enum
    {
        MAX_BUFFER_LENGTH = 25600,
    };

    CStorageDeviceFileSystem();
    virtual ~CStorageDeviceFileSystem();
    void Init(void);
    void WritePrepare(uint16_t uiDestination, uint8_t *puiSource, uint16_t uiLength);
    uint8_t Write(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength);
    uint8_t Write(void);
    uint8_t Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength);
    bool IsReadyToWrite(void)
    {
        return true;
    };

    void SetIsDataWrited(bool bStatus)
    {
        m_bDataIsWrited = bStatus;
    };
    bool IsDataWrited(void)
    {
        return m_bDataIsWrited;
    };

    void Fsm(void);

private:
    const char *pccFileName = "StorageDeviceData.dat";
    //    ifstream indata;
//    ofstream outdata;
    bool m_bDataIsWrited;
};

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
class CDataStore : public CDfa
{
public:
    CDataStore();
    CDataStore(CStorageDevice* pxStorageDevice);
    virtual ~CDataStore();
    void Init(void);
    void CreateServiceSection(void);
    void WriteTemporaryServiceSection(void);
    void WriteServiceSection(void);
    uint8_t ReadTemporaryServiceSection(void);
    uint8_t ReadServiceSection(void);
    uint16_t ReadBlock(uint8_t * , uint8_t );
    bool WriteBlock(uint8_t * , uint16_t , uint8_t );
    uint16_t Write(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock);
    uint8_t Check(void);
    bool CompareCurrentWithStoredCrc(void);
    void CrcOfBlocksCrcCreate(void);
    bool CrcOfBlocksCrcCheck(void);
    void Fsm(void);

    uint8_t GetBlockLength(uint8_t uiBlock)
    {
        return m_xServiseSection.xServiseSectionData.
               axBlockPositionData[uiBlock].uiLength;
    };

    CTimer* GetTimerPointer(void)
    {
        return &m_xTimer;
    };

    enum
    {
        CRC_LENGTH = 2,
        TAIL_LENGTH = 2,
        SERVICE_SECTION_DATA_BLOCK_NUMBER = 1,
        MAX_BLOCK_LENGTH = 256,
        MAX_ENCODED_BLOCK_LENGTH =
            ((MAX_BLOCK_LENGTH + TAIL_LENGTH) + ((MAX_BLOCK_LENGTH + TAIL_LENGTH) / 2)),
        MAX_BLOCKS_NUMBER = (TDataBase::BLOCKS_QUANTITY + SERVICE_SECTION_DATA_BLOCK_NUMBER),
        INTERMEDIATE_BUFFER_LENGTH = 512,
    };

    enum
    {
        SERVICE_SECTION_DATA = 0,
        READY_TO_WRITE_WAITING_TIMEOUT = 200,
        WRITE_END_WAITING_TIMEOUT = 10000,
    };

    enum
    {
        IDDLE = 0,
        START_WRITE,
        READY_TO_WRITE_WAITING,
        WRITE_END_WAITING,

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
        uint16_t uiOffset;
        uint16_t uiLength;
        uint16_t uiEncodedLength;
        uint16_t uiCrc;
    };

    struct TServiseSectionData
    {
        uint16_t uiLength;
        uint16_t uiEncodedLength;
        uint16_t uiFreeSpaceOffset;
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
        uint16_t uiCrc;
    };

    enum
    {
        // ������� ���� ����� ���� ���� ��� ���� �������.
        TEMPORARY_SERVICE_SECTION_DATA_BEGIN = 8,
        SERVICE_SECTION_DATA_BEGIN =
            TEMPORARY_SERVICE_SECTION_DATA_BEGIN +
            (sizeof(struct TServiseSection) + (sizeof(struct TServiseSection) / 2)),
    };


protected:
private:
    CStorageDevice* m_pxStorageDevice;
    // ��������� ������ ������� ��������.
    TServiseSection m_xServiseSection;
//    TServiseSectionData m_xServiseSectionData;
    // ������ ����������� ���� ������.
    uint16_t m_auiBlocksCurrentCrc[MAX_BLOCKS_NUMBER];
    uint8_t* m_puiIntermediateBuff;
    CTimer m_xTimer;
};

//-----------------------------------------------------------------------------------------------------
#endif // CDATASTORE_H
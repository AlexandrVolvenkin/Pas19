
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CSTORAGEDEVICE_H
#define CSTORAGEDEVICE_H

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
    virtual uint8_t PassingDataAndStartWrite(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength) = 0;
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
    uint16_t m_nuiByteCounter;
    uint16_t m_uiOffset;
    uint8_t* m_puiBuffer;
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

    uint8_t PassingDataAndStartWrite(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength);
    uint8_t Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength);
    bool IsReadyToWrite(void)
    {
        // Симулируем готовность к записи.
        // В случае записи средствами ОС проверка не проводится.
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

private:
    uint8_t Write(void);

    const char *pccFileName = "StorageDeviceData.dat";
    // Флаг - данные записаны.
    bool m_bDataIsWrited;
};

//-----------------------------------------------------------------------------------------------------









//-----------------------------------------------------------------------------------------------------
#endif // CSTORAGEDEVICE_H

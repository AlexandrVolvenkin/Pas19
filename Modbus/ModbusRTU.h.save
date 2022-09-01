//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMODBUSRTU_H
#define CMODBUSRTU_H

#include <stdint.h>
#include "Modbus.h"
#include "Configuration.h"
#include "Platform.h"
#include "Timer.h"

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
//#define MODBUS_RTU_MAX_ADU_LENGTH  256

#define _MODBUS_RTU_35_TIMEOUT        (((1000000UL / MODBUS_RTU_BAUD_RATE) * 8UL * 4UL) / 1000) + 1

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

#define MODBUS_RTU_RS232 0
#define MODBUS_RTU_RS485 1

//-----------------------------------------------------------------------------------------------------
class CModbusRtu : public CModbus
{
public:
    CModbusRtu();
    virtual ~CModbusRtu();

    void SerialPortInit(const char* pccPortName,
                        uint32_t uiBaudRate,
                        uint8_t uiDataBits,
                        char cParity,
                        uint8_t uiStopBit);
    void WorkingArraysInit(uint8_t *puiRxBuffer,
                           uint8_t *puiTxBuffer,
                           uint8_t *puiCoils,
                           uint8_t *puiDiscreteInputs,
                           uint16_t *pui16HoldingRegisters,
                           uint16_t *pui16InputRegisters,
                           uint16_t uiCoilsNumber,
                           uint16_t uiDiscreteInputsNumber,
                           uint16_t uiHoldingRegistersNumber,
                           uint16_t uiInputRegistersNumber);
    void Fsm(void);


protected:
private:

//    void Reset(void);
    void ReceiveEnable(void);
    void ReceiveDisable(void);
    void TransmitEnable(void);
    void TransmitDisable(void);
    uint16_t RequestBasis(uint8_t uiSlave,
                          uint8_t uiFunctionCode,
                          uint16_t uiAddress,
                          uint16_t uiBitNumber,
                          uint8_t *puiRequest);
    uint16_t ResponseBasis(uint8_t , uint8_t , uint8_t * );
    uint16_t Tail(uint8_t * , uint16_t );
    uint16_t Send(uint8_t * , uint16_t );
    int16_t Receive(uint8_t * , uint16_t );
    uint16_t GetFrameLength(void);
    int8_t FrameCheck(uint8_t * , uint16_t );

    bool IsDataWrited(void)
    {
        return m_pxSerialPort -> IsDataWrited();
    };

    uint16_t GetGuardTimeout(void)
    {
        return m_uiGuardTimeout;
    };
    void SetGuardTimeout(uint16_t uiData)
    {
        m_uiGuardTimeout = uiData;
    };

    uint16_t HEADER_LENGTH(void)
    {
        return 1;
    };
    uint16_t CRC_LENGTH(void)
    {
        return 2;
    };

    CSerialPort* m_pxSerialPort;
    // таймоут по отсутствию следующего байта 3.5 бода.
    uint16_t m_uiGuardTimeout;
    // таймоут по отсутствию запроса.
    const static uint16_t m_uiReceiveTimeout = 15000;
    // таймоут по отсутствию подтверждения.
    const static uint16_t m_uiConfirmationTimeout = 500;
    const static uint16_t m_uiTransmitDelayTimeout = 5;
};

//-----------------------------------------------------------------------------------------------------
#endif // CMODBUSRTU_H

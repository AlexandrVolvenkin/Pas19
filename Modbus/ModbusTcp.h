//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMODBUSTCP_H
#define CMODBUSTCP_H

#include <stdint.h>
#include "Modbus.h"
#include "Configuration.h"
#include "Platform.h"
#include "Timer.h"

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
//#define MODBUS_RTU_MAX_ADU_LENGTH  256


#define MODBUS_TCP_DEFAULT_PORT   502
#define MODBUS_TCP_SLAVE         0x11

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
#define MODBUS_TCP_MAX_ADU_LENGTH  260

#define _MODBUS_TCP_HEADER_LENGTH      7
#define _MODBUS_TCP_PRESET_REQ_LENGTH 12
#define _MODBUS_TCP_PRESET_RSP_LENGTH  8

#define _MODBUS_TCP_CHECKSUM_LENGTH    0

//-----------------------------------------------------------------------------------------------------
class CModbusTcp : public CModbus
{
public:
    CModbusTcp();
    virtual ~CModbusTcp();

    void CommunicationDeviceInit(const char* pccIpAddress,
                                 uint16_t uiPort);
    void WorkingArraysInit(uint8_t *puiCoils,
                           uint8_t *puiDiscreteInputs,
                           uint16_t *pui16HoldingRegisters,
                           uint16_t *pui16InputRegisters,
                           uint16_t uiCoilsNumber,
                           uint16_t uiDiscreteInputsNumber,
                           uint16_t uiHoldingRegistersNumber,
                           uint16_t uiInputRegistersNumber);
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
        return m_pxCommunicationDevice -> IsDataWrited();
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
        return 7;
    };
    uint16_t CRC_LENGTH(void)
    {
        return 0;
    };

    CTcpCommunicationDevice* m_pxCommunicationDevice;
    uint16_t m_uiRequestTransactionId = 0;
    uint16_t m_uiResponseTransactionId = 0;
    // таймоут по отсутствию следующего байта 3.5 бода.
    uint16_t m_uiGuardTimeout;
    // таймоут по отсутствию запроса.
    const static uint16_t m_uiReceiveTimeout = 15000;
    // таймоут по отсутствию подтверждения.
    const static uint16_t m_uiConfirmationTimeout = 500;
    const static uint16_t m_uiTransmitDelayTimeout = 5;

    uint8_t m_auiReceiveMessageBuff[MODBUS_TCP_MAX_ADU_LENGTH];
    uint8_t m_auiTransmitMessageBuff[MODBUS_TCP_MAX_ADU_LENGTH];
};

#endif // CMODBUSTCP_H

//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------

#include <Crc.h>
#include <Dfa.h>
#include "Platform.h"
#include <ModbusRtu.h>

using namespace std;

//-----------------------------------------------------------------------------------------------------
CModbusRtu::CModbusRtu()
{
    m_pxCommunicationDevice = new CSerialPort();
    m_puiRxBuffer = new uint8_t[MODBUS_RTU_MAX_ADU_LENGTH];
    m_puiTxBuffer = new uint8_t[MODBUS_RTU_MAX_ADU_LENGTH];
    SetFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CModbusRtu::~CModbusRtu()
{
    delete[] m_puiTxBuffer;
    delete[] m_puiRxBuffer;
    delete m_pxCommunicationDevice;
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::CommunicationDeviceInit(const char* pccDeviceName,
                                uint32_t uiBaudRate,
                                uint8_t uiDataBits,
                                char cParity,
                                uint8_t uiStopBit)
{
    m_pxCommunicationDevice -> Init();
    m_pxCommunicationDevice -> SetPortName(pccDeviceName);
    m_pxCommunicationDevice -> SetBaudRate(uiBaudRate);
    m_pxCommunicationDevice -> SetDataBits(uiDataBits);
    m_pxCommunicationDevice -> SetParity(cParity);
    m_pxCommunicationDevice -> SetStopBit(uiStopBit);

    m_uiGuardTimeout = ((((1000000UL / uiBaudRate) * 8UL * 4UL) / 1000UL) + 1);
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::WorkingArraysInit(uint8_t *puiRxBuffer,
                                   uint8_t *puiTxBuffer,
                                   uint8_t *puiCoils,
                                   uint8_t *puiDiscreteInputs,
                                   uint16_t *pui16HoldingRegisters,
                                   uint16_t *pui16InputRegisters,
                                   uint16_t uiCoilsNumber,
                                   uint16_t uiDiscreteInputsNumber,
                                   uint16_t uiHoldingRegistersNumber,
                                   uint16_t uiInputRegistersNumber)
{
//    m_puiRxBuffer = puiRxBuffer;
//    m_puiTxBuffer = puiTxBuffer;
    m_puiCoils = puiCoils;
    m_puiDiscreteInputs = puiDiscreteInputs;
    m_pui16HoldingRegisters = pui16HoldingRegisters;
    m_pui16InputRegisters = pui16InputRegisters;
    m_uiCoilsNumber = uiCoilsNumber;
    m_uiDiscreteInputsNumber = uiDiscreteInputsNumber;
    m_uiHoldingRegistersNumber = uiHoldingRegistersNumber;
    m_uiInputRegistersNumber = uiInputRegistersNumber;
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::WorkingArraysInit(uint8_t *puiCoils,
                                   uint8_t *puiDiscreteInputs,
                                   uint16_t *pui16HoldingRegisters,
                                   uint16_t *pui16InputRegisters,
                                   uint16_t uiCoilsNumber,
                                   uint16_t uiDiscreteInputsNumber,
                                   uint16_t uiHoldingRegistersNumber,
                                   uint16_t uiInputRegistersNumber)
{
    m_puiCoils = puiCoils;
    m_puiDiscreteInputs = puiDiscreteInputs;
    m_pui16HoldingRegisters = pui16HoldingRegisters;
    m_pui16InputRegisters = pui16InputRegisters;
    m_uiCoilsNumber = uiCoilsNumber;
    m_uiDiscreteInputsNumber = uiDiscreteInputsNumber;
    m_uiHoldingRegistersNumber = uiHoldingRegistersNumber;
    m_uiInputRegistersNumber = uiInputRegistersNumber;
}

////-----------------------------------------------------------------------------------------------------
//void CModbusRtu::Reset(void)
//{
//    m_pxCommunicationDevice -> Reset();
//}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::ReceiveEnable(void)
{
    m_pxCommunicationDevice -> Open();
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::ReceiveDisable(void)
{
    m_pxCommunicationDevice -> Close();
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::TransmitEnable(void)
{
    m_pxCommunicationDevice -> Open();
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::TransmitDisable(void)
{
    m_pxCommunicationDevice -> Close();
}

//-----------------------------------------------------------------------------------------------------
uint16_t CModbusRtu::Tail(uint8_t *puiMessage, uint16_t uiLength)
{
    uint16_t uiCrc = usCrc16(puiMessage, uiLength);
    puiMessage[uiLength++] = uiCrc & 0x00FF;
    puiMessage[uiLength++] = uiCrc >> 8;

    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
/* Builds a RTU request header */
uint16_t CModbusRtu::RequestBasis(uint8_t uiSlave,
                                  uint8_t uiFunctionCode,
                                  uint16_t uiAddress,
                                  uint16_t uiBitNumber,
                                  uint8_t *puiRequest)
{
    puiRequest[0] = uiSlave;
    puiRequest[1] = uiFunctionCode;
    puiRequest[2] = (static_cast<uint8_t>(uiAddress >> 8));
    puiRequest[3] = (static_cast<uint8_t>(uiAddress & 0x00ff));
    puiRequest[4] = (static_cast<uint8_t>(uiBitNumber >> 8));
    puiRequest[5] = (static_cast<uint8_t>(uiBitNumber & 0x00ff));

    return _MODBUS_RTU_PRESET_REQ_LENGTH;
}

//-----------------------------------------------------------------------------------------------------
/* Builds a RTU response header */
uint16_t CModbusRtu::ResponseBasis(uint8_t uiSlave, uint8_t uiFunctionCode, uint8_t *puiResponse)
{
    /* In this case, the slave is certainly valid because a check is already
     * done in _modbus_rtu_listen */
    puiResponse[0] = uiSlave;
    puiResponse[1] = uiFunctionCode;

    return _MODBUS_RTU_PRESET_RSP_LENGTH;
}

//-----------------------------------------------------------------------------------------------------
uint16_t CModbusRtu::Send(uint8_t *puiMessage, uint16_t uiLength)
{
    return m_pxCommunicationDevice -> Write(puiMessage, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbusRtu::Receive(uint8_t *puiDestination, uint16_t uiLength)
{
    return m_pxCommunicationDevice -> Read(puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int8_t CModbusRtu::FrameCheck(uint8_t *puiSourse, uint16_t uiLength)
{
    if (uiLength < _MIN_MESSAGE_LENGTH)
    {
        return 0;
    }

    uint16_t uiCrc = ((static_cast<uint16_t>(puiSourse[uiLength - 1]) << 8) |
                      (static_cast<uint16_t>(puiSourse[uiLength - 2])));
    uint16_t uiCrcTemp = usCrc16(puiSourse,
                                 (uiLength - _MODBUS_RTU_CHECKSUM_LENGTH));
    if (usCrc16(puiSourse,
                (uiLength - _MODBUS_RTU_CHECKSUM_LENGTH)) == uiCrc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
void CModbusRtu::Fsm(void)
{
    switch (GetFsmState())
    {
        int16_t iBytesNumber;

    case IDDLE:
        break;

    case START_REQUEST:
        ReceiveDisable();
        GetTimerPointer() -> Set(m_uiReceiveTimeout);
        SetMessageLength(0);
        ReceiveEnable();
        SetFsmState(WAITING_MESSAGE_REQUEST);
        break;

    case WAITING_MESSAGE_REQUEST:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_RTU_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
            SetFsmState(RECEIVE_MESSAGE_REQUEST);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(START_REQUEST);
        }

        // «акончилось врем€ ожидани€ запроса(15 секунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmState(START_REQUEST);
        }

        break;

    case RECEIVE_MESSAGE_REQUEST:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_RTU_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(START_REQUEST);
        }

        // ѕрин€т пакет(закончилось врем€ ожидани€ следующего бода(3.5 бод))?
        if (GetTimerPointer() -> IsOverflow())
        {
            if (FrameCheck(m_puiRxBuffer, GetMessageLength()))
            {
                SetFsmState(REQUEST_PROCESSING_REQUEST);
            }
            else
            {
                SetFsmState(START_REQUEST);
            }
        }

        break;

    case REQUEST_PROCESSING_REQUEST:
        if (RequestProcessing(m_puiRxBuffer, m_puiTxBuffer, GetMessageLength()))
        {
//            CPlatform::TxLedOn();
            SetFsmState(FRAME_TRANSMIT_CONFIRMATION);
        }
        else
        {
            //            CPlatform::TxLedOff();
            SetFsmState(START_REQUEST);
        }
        break;

    case FRAME_TRANSMIT_CONFIRMATION:
        GetTimerPointer() -> Set(m_uiTransmitDelayTimeout);
        ReceiveDisable();
        SetFsmState(WAITING_FRAME_TRANSMIT_CONFIRMATION);
        break;

    case WAITING_FRAME_TRANSMIT_CONFIRMATION:
        // «акончилось врем€ паузы между приЄмом и передачей(5 милисекунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            GetTimerPointer() -> Set(m_uiConfirmationTimeout);
            TransmitEnable();
            SendMessage(m_puiTxBuffer, GetMessageLength());
            SetFsmState(END_WAITING_FRAME_TRANSMIT_CONFIRMATION);
        }
        break;

    case END_WAITING_FRAME_TRANSMIT_CONFIRMATION:
        if (IsDataWrited())
        {
            TransmitDisable();
//            CPlatform::TxLedOff();
            SetFsmState(START_REQUEST);
        }

        // «акончилось врем€ ожидани€ окончани€ передачи?
        if (GetTimerPointer() -> IsOverflow())
        {
//            CPlatform::TxLedOff();
            SetFsmState(START_REQUEST);
        }

        break;

//        case STOP_REQUEST:
//            ReceiveDisable();
//            SetFsmState(IDDLE);
//            break;

//-----------------------------------------------------------------------------------------------------
// ModbusMaster
    case START_CONFIRMATION:
        ReceiveDisable();
        GetTimerPointer() -> Set(m_uiConfirmationTimeout);
        SetMessageLength(0);
        ReceiveEnable();
        SetFsmState(WAITING_MESSAGE_CONFIRMATION);
        break;

    case WAITING_MESSAGE_CONFIRMATION:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_RTU_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
            SetFsmState(RECEIVE_MESSAGE_CONFIRMATION);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(STOP_REQUEST);
        }

        // «акончилось врем€ ожидани€ ответа(500 милисекунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmState(STOP_REQUEST);
        }

        break;

    case RECEIVE_MESSAGE_CONFIRMATION:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_RTU_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(STOP_REQUEST);
        }

        // ѕрин€т пакет(закончилось врем€ ожидани€ следующего бода(3.5 бод))?
        if (GetTimerPointer() -> IsOverflow())
        {
            if (FrameCheck(m_puiRxBuffer, GetMessageLength()))
            {
                SetFsmState(ANSWER_PROCESSING_CONFIRMATION);
            }
            else
            {
                SetFsmState(STOP_REQUEST);
            }
        }

        break;

    case ANSWER_PROCESSING_CONFIRMATION:
        if (AnswerProcessing(m_puiRxBuffer, GetMessageLength()))
        {
            SetFsmState(STOP_REQUEST);
        }
        else
        {
            SetFsmState(STOP_REQUEST);
        }
        break;

    case FRAME_TRANSMIT_REQUEST:
        GetTimerPointer() -> Set(m_uiTransmitDelayTimeout);
//        CPlatform::TxLedOn();
        SetFsmState(WAITING_FRAME_TRANSMIT_REQUEST);
        break;

    case WAITING_FRAME_TRANSMIT_REQUEST:
        // «акончилось врем€ паузы между приЄмом и передачей(5 милисекунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            GetTimerPointer() -> Set(m_uiConfirmationTimeout);
            TransmitEnable();
            SendMessage(m_puiTxBuffer, GetMessageLength());
            SetFsmState(END_WAITING_FRAME_TRANSMIT_REQUEST);
        }
        break;

    case END_WAITING_FRAME_TRANSMIT_REQUEST:
        if (IsDataWrited())
        {
            TransmitDisable();
//            CPlatform::TxLedOff();
            SetFsmState(START_CONFIRMATION);
        }

        // «акончилось врем€ ожидани€ окончани€ передачи?
        if (GetTimerPointer() -> IsOverflow())
        {
//            CPlatform::TxLedOff();
            SetFsmState(STOP_REQUEST);
        }

        break;

    case STOP_REQUEST:
        ReceiveDisable();
        TransmitEnable();
        SetFsmState(IDDLE);
        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------

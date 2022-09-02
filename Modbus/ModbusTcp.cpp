
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
#include <ModbusTcp.h>

using namespace std;

//-----------------------------------------------------------------------------------------------------
CModbusTcp::CModbusTcp()
{
    m_pxCommunicationDevice = new CTcpCommunicationDevice();
    m_puiRxBuffer = new uint8_t[MODBUS_TCP_MAX_ADU_LENGTH];
    m_puiTxBuffer = new uint8_t[MODBUS_TCP_MAX_ADU_LENGTH];
    SetFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CModbusTcp::~CModbusTcp()
{
    delete[] m_puiTxBuffer;
    delete[] m_puiRxBuffer;
    delete m_pxCommunicationDevice;
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::CommunicationDeviceInit(const char* pccIpAddress,
        uint16_t uiPort)
{
    m_pxCommunicationDevice -> Init();
    m_pxCommunicationDevice -> SetIpAddress(pccIpAddress);
    m_pxCommunicationDevice -> SetPort(uiPort);

    m_uiGuardTimeout = 10;
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::WorkingArraysInit(uint8_t *puiCoils,
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

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::WorkingArraysInit(uint8_t *puiRxBuffer,
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

////-----------------------------------------------------------------------------------------------------
//void CModbusTcp::Reset(void)
//{
//    m_pxCommunicationDevice -> Reset();
//}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::ReceiveEnable(void)
{
//    m_pxCommunicationDevice -> Open();
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::ReceiveDisable(void)
{
//    m_pxCommunicationDevice -> Close();
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::TransmitEnable(void)
{
//    m_pxCommunicationDevice -> Open();
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::TransmitDisable(void)
{
//    m_pxCommunicationDevice -> Close();
}

//-----------------------------------------------------------------------------------------------------
uint16_t CModbusTcp::Tail(uint8_t *puiMessage, uint16_t uiLength)
{
    return uiLength;
}

//-----------------------------------------------------------------------------------------------------
/* Builds a TCP request header */
uint16_t CModbusTcp::RequestBasis(uint8_t uiSlave,
                                  uint8_t uiFunctionCode,
                                  uint16_t uiAddress,
                                  uint16_t uiBitNumber,
                                  uint8_t *puiRequest)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
       (page 23/46):
       The transaction identifier is used to associate the future response
       with the request. So, at a time, on a TCP connection, this identifier
       must be unique. */

    /* Transaction ID */
    if (m_uiRequestTransactionId < UINT16_MAX)
    {
        m_uiRequestTransactionId++;
    }
    else
    {
        m_uiRequestTransactionId = 0;
    }
    puiRequest[0] = (m_uiRequestTransactionId >> 8);
    puiRequest[1] = (m_uiRequestTransactionId & 0x00ff);

    /* Protocol Modbus */
    puiRequest[2] = 0;
    puiRequest[3] = 0;

    /* Length will be defined later by set_puiRequest_length_tcp at offsets 4
       and 5 */

    puiRequest[6] = uiSlave;
    puiRequest[7] = uiFunctionCode;
    puiRequest[8] = (static_cast<uint8_t>(uiAddress >> 8));
    puiRequest[9] = (static_cast<uint8_t>(uiAddress & 0x00ff));
    puiRequest[10] = (static_cast<uint8_t>(uiBitNumber >> 8));
    puiRequest[11] = (static_cast<uint8_t>(uiBitNumber & 0x00ff));

    return _MODBUS_TCP_PRESET_REQ_LENGTH;
}

//-----------------------------------------------------------------------------------------------------
/* Builds a RTU response header */
uint16_t CModbusTcp::ResponseBasis(uint8_t uiSlave, uint8_t uiFunctionCode, uint8_t *puiResponse)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation
       Guide V1.0b (page 23/46):
       The transaction identifier is used to associate the future
       response with the puiRequestuest. */
    puiResponse[0] = (m_uiResponseTransactionId >> 8);
    puiResponse[1] = (m_uiResponseTransactionId & 0x00ff);

    /* Protocol Modbus */
    puiResponse[2] = 0;
    puiResponse[3] = 0;

    /* Length will be set later by send_msg (4 and 5) */

    /* The slave ID is copied from the indication */
    puiResponse[6] = uiSlave;
    puiResponse[7] = uiFunctionCode;

    return _MODBUS_TCP_PRESET_RSP_LENGTH;
}

//-----------------------------------------------------------------------------------------------------
uint16_t CModbusTcp::Send(uint8_t *puiMessage, uint16_t uiLength)
{
    return m_pxCommunicationDevice -> Write(puiMessage, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int16_t CModbusTcp::Receive(uint8_t *puiDestination, uint16_t uiLength)
{
    return m_pxCommunicationDevice -> Read(puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int8_t CModbusTcp::FrameCheck(uint8_t *puiSourse, uint16_t uiLength)
{
    if (uiLength < _MIN_MESSAGE_LENGTH)
    {
        return 0;
    }

    return 1;
}

//-----------------------------------------------------------------------------------------------------
void CModbusTcp::Fsm(void)
{
    switch (GetFsmState())
    {
        int16_t iBytesNumber;

    case IDDLE:
        break;

    case REQUEST_ENABLE:
        m_pxCommunicationDevice -> Listen();
        GetTimerPointer() -> Set(m_uiReceiveTimeout);
        SetFsmState(WAITING_ACCEPT);
        break;

    case WAITING_ACCEPT:
        if (m_pxCommunicationDevice -> Accept())
        {
            SetFsmState(START_REQUEST);
        }

        // «акончилось врем€ ожидани€ запроса(15 секунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmState(REQUEST_ERROR);
        }
        break;

    case START_REQUEST:
//        ReceiveDisable();
        GetTimerPointer() -> Set(m_uiReceiveTimeout);
        SetMessageLength(0);
//        ReceiveEnable();
        SetFsmState(WAITING_MESSAGE_REQUEST);
        break;

    case WAITING_MESSAGE_REQUEST:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_TCP_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
            SetFsmState(RECEIVE_MESSAGE_REQUEST);
        }
        else if (iBytesNumber == 0)
        {
            SetFsmState(REQUEST_ERROR);
        }

        // «акончилось врем€ ожидани€ запроса(15 секунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmState(REQUEST_ERROR);
        }

        break;

    case RECEIVE_MESSAGE_REQUEST:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_TCP_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
        }
        else if (iBytesNumber == 0)
        {
            SetFsmState(REQUEST_ERROR);
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
                SetFsmState(STOP_REQUEST);
            }
        }

        break;

    case REQUEST_PROCESSING_REQUEST:
        m_uiResponseTransactionId = (m_puiRxBuffer[0] << 8) + m_puiRxBuffer[1];

        if (RequestProcessing(m_puiRxBuffer, m_puiTxBuffer, GetMessageLength()))
        {
//            CPlatform::TxLedOn();
            SetFsmState(FRAME_TRANSMIT_CONFIRMATION);
        }
        else
        {
            //            CPlatform::TxLedOff();
            SetFsmState(STOP_REQUEST);
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
            SetFsmState(STOP_REQUEST);
        }

        // «акончилось врем€ ожидани€ окончани€ передачи?
        if (GetTimerPointer() -> IsOverflow())
        {
//            CPlatform::TxLedOff();
            SetFsmState(REQUEST_ERROR);
        }

        break;

    case STOP_REQUEST:
//        ReceiveDisable();
        SetFsmState(START_REQUEST);
        break;

    case REQUEST_ERROR:
//        ReceiveDisable();
        m_pxCommunicationDevice -> Close();
        SetFsmState(REQUEST_ENABLE);
        break;

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
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_TCP_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
            SetFsmState(RECEIVE_MESSAGE_CONFIRMATION);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(STOP_CONFIRMATION);
        }

        // «акончилось врем€ ожидани€ ответа(500 милисекунд)?
        if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmState(STOP_CONFIRMATION);
        }

        break;

    case RECEIVE_MESSAGE_CONFIRMATION:
        iBytesNumber = Receive((m_puiRxBuffer + GetMessageLength()), (MODBUS_TCP_MAX_ADU_LENGTH - GetMessageLength()));

        if (iBytesNumber > 0)
        {
            SetMessageLength(GetMessageLength() + iBytesNumber);
            GetTimerPointer() -> Set(m_uiGuardTimeout);
        }
        else if (iBytesNumber == -1)
        {
            SetFsmState(STOP_CONFIRMATION);
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
                SetFsmState(STOP_CONFIRMATION);
            }
        }

        break;

    case ANSWER_PROCESSING_CONFIRMATION:
        if (AnswerProcessing(m_puiRxBuffer, GetMessageLength()))
        {
            SetFsmState(STOP_CONFIRMATION);
        }
        else
        {
            SetFsmState(STOP_CONFIRMATION);
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
            SetFsmState(STOP_CONFIRMATION);
        }

        break;

    case STOP_CONFIRMATION:
        ReceiveDisable();
        TransmitEnable();
        SetFsmState(IDDLE);
        break;

    default:
        break;
    }
}
//-----------------------------------------------------------------------------------------------------

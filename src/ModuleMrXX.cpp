//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "ModuleMrXX.h"
#include "Platform.h"
#include "Crc.h"

using namespace std;

//-----------------------------------------------------------------------------------------------------
CModuleMrXXDriver::CModuleMrXXDriver()
{

}

//-----------------------------------------------------------------------------------------------------
CModuleMrXXDriver::CModuleMrXXDriver(uint8_t uiType)
{
    m_uiType = uiType;
    SetFsmState(MRXX_FSM_STATE_WRITE_DATABASE);
}

//-----------------------------------------------------------------------------------------------------
CModuleMrXXDriver::~CModuleMrXXDriver()
{

}
//-----------------------------------------------------------------------------------------------------
// ���������� ���� ������ �� RAM ������� � EEPROM ������.
int8_t CModuleMrXXDriver::ChangeProtocol(void)
{

//    SPI_MT_PRIAMBLE_LENGTH = 2,
//    SPI_MT_FUNCTION_CODE_OFFSET = 0,
//    SPI_MT_LENGTH_OFFSET = 1,
//    SPI_MT_DATA_OFFSET = 2,

    m_puiTxBuffer[0] = CHANGE_PROTOCOL_COMMAND;

    m_pxCommunicationDevice -> Exchange(m_uiAddress,
                                        m_puiTxBuffer,
                                        m_puiRxBuffer,
                                        (SPI_MT_PRIAMBLE_LENGTH +
                                         GET_MODULE_TYPE_COMMAND_ANSWER_LENGTH +
                                         ONE_BYTE_CRC_LENGTH),
                                        LOW_SPEED_IN_HZ);

//    cout << "CHANGE_PROTOCOL_COMMAND " << endl;
//    unsigned char *pucSourceTemp;
//    pucSourceTemp = (unsigned char*)aucSpiRxBuffer;
//    for(int i=0; i<16; )
//    {
//        for(int j=0; j<8; j++)
//        {
//            cout << hex << uppercase << setw(2) << setfill('0') << (unsigned int)pucSourceTemp[i + j] << " ";
//        }
//        cout << endl;
//        i += 8;
//    }

    // ������ ��������?
    if (m_puiRxBuffer[SPI_COMMAND_BYTE_OFFSET] ==
            CHANGE_PROTOCOL_COMMAND) // if echo answer is ok.
    {
        // ����� ������� ������ ��� ������?
        if ((iCrcSummOneByteCompare(&m_puiRxBuffer[SPI_COMMAND_BYTE_OFFSET],
                                    (GET_MODULE_TYPE_COMMAND_LENGTH +
                                     GET_MODULE_TYPE_COMMAND_ANSWER_LENGTH)) > 0))
        {
            // ���� ������������� ������ ���� ������ � EEPROM ������.
            cout << "iModuleMrXXChangeProtocol ok" << endl;
            return 1;
        }
    }

    cout << "iModuleMrXXChangeProtocol error" << endl;

    return 0;
}

//-----------------------------------------------------------------------------------------------------
void CModuleMrXXDriver::Allocate(TMemoryAllocationConext &xMemoryAllocationConext)
{
//    m_uiAddress = xMemoryAllocationConext.uiAddress;
//    m_puiRxBuffer = xMemoryAllocationConext.puiRxBuffer;
//    m_puiTxBuffer = xMemoryAllocationConext.puiTxBuffer;
//    m_puiErrorCode = xMemoryAllocationConext.puiErrorCode;
//    // ������� ��������� �� ������ ���������� ������.
//    m_puiDiscreteInputs = xMemoryAllocationConext.puiDiscreteInputs;
//
//    // ������� ��������� �� ������ ���� ������.
//    m_pxDiscreteOutputDataBase = xMemoryAllocationConext.pxDiscreteOutputDataBase;//debag//
//
//    m_puiDiscreteOutputState = xMemoryAllocationConext.puiDiscreteOutputState;
//
//    // ������� ��������� �� ����� � ������� ���������� ������� ��� �������� ������.
//    m_pxDiscreteOutputControl =
//        &xMemoryAllocationConext.
//        pxDiscreteOutputControl[xMemoryAllocationConext.uiUsedDiscreteOutputs];
//    // �������� ����� ����� ���������� ������.
//    xMemoryAllocationConext.
//    uiUsedDiscreteOutputs +=
//        MR_DISCRETE_OUTPUT_NUMBER;
//
//    m_pxDiscreteOutputDataBase =
//        &xMemoryAllocationConext.
//        pxDiscreteOutputDataBase[xMemoryAllocationConext.uiUsedDiscreteOutputDataBase];
//    // �������� ����� ����� ���������� ������.
//    xMemoryAllocationConext.
//    uiUsedDiscreteOutputDataBase +=
//        MR_DISCRETE_OUTPUT_NUMBER;
//
//    m_puiErrorAlarmDataArray =
//        &xMemoryAllocationConext.
//        puiErrorAlarmDataArray[DISCRETE_OUTPUT_MODULE_FAILURE];
//
//    m_uiBadAnswerCounter = 0;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModuleMrXXDriver::WriteDataBase(void)
{
    uint16_t usData;
    uint8_t *puiDestination;
    uint8_t uiLength = 0;

    m_puiTxBuffer[uiLength++] = MRXX_SET_DATA_BASE_COMMAND;
    // ��������������� ������ ������� �����.
    m_puiTxBuffer[uiLength++] = uiLength;
    // ��� �������� �� � �� ���������� ������ ���� 0 ��������� ������� ������.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        m_puiTxBuffer[i + uiLength] =
            m_pxDiscreteOutputDataBase -> DiscreteOutputModuleDataBase[i];
        uiLength++;
    }
    // ������ ������� �����.
    m_puiTxBuffer[SPI_MT_LENGTH_OFFSET] = uiLength;
    // ������ Crc.
    m_puiTxBuffer[uiLength] = HammingDistanceCrc(m_puiTxBuffer,
                              (SPI_MT_PRIAMBLE_LENGTH +
                               MR_DISCRETE_OUTPUT_NUMBER));

    m_pxCommunicationDevice -> Exchange(m_uiAddress,
                                        m_puiTxBuffer,
                                        m_puiRxBuffer,
                                        (SPI_MT_PRIAMBLE_LENGTH +
                                         GET_MODULE_TYPE_COMMAND_ANSWER_LENGTH +
                                         ONE_BYTE_CRC_LENGTH),
                                        LOW_SPEED_IN_HZ);

    return 1;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModuleMrXXDriver::DataExchange(void)
{
    uint8_t uiExternalReceipt = 0;
    // ���� ��������� ������� �������� ������������.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        // ��� �������� ����� ������� ������������ �����������������?
        if (m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i] != 0xFF)
        {
            // ��� �������� ����� ���� ������� ������������?
            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i]])
            {
                // ��������� ������������ ��� �������� �����.
                uiExternalReceipt |= (1 << i);
            }
        }
    }

    uint8_t uiExternalReset = 0;
    // ���� ��������� ������� �������� ������.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        // ��� �������� ����� ������� ����� ����������������?
        if (m_pxDiscreteOutputDataBase -> ExternalResetAddress[i] != 0xFF)
        {
            // ��� �������� ����� ���� ������� �����?
            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalResetAddress[i]])
            {
                // ��������� ����� ��� �������� �����.
                uiExternalReset |= (1 << i);
            }
        }
    }

    uint8_t uiLocalBou = 0;
    // ���� ������������ Modbus?
    if (m_uiModbusReceipt)
    {
        // ������� ���� ����������� �������.
        m_uiModbusReceipt = 0;
        // ��������� ���� ���������� - ������������ Modbus.
        uiLocalBou |= (1 << GLOBAL_KVIT_PC_BIT);
    }
    // ���� ����� Modbus?
    if (m_uiModbusReset)
    {
        // ������� ���� ����������� �������.
        m_uiModbusReset = 0;
        // ��������� ���� ���������� - ����� Modbus.
        uiLocalBou |= (1 << GLOBAL_RESET_PC_BIT);
    }
    // ���� ������������ � ������ ����������� ������?
    if (m_uiReceipt)
    {
        // ������� ���� ����������� �������.
        m_uiReceipt = 0;
        // ��������� ���� ���������� - ������������ � ����������� ������.
        uiLocalBou |= (1 << GLOBAL_KVIT_BUTTON_BIT);
    }
    // ���� ����� � ������ ����������� ������?
    if (m_uiReset)
    {
        // ������� ���� ����������� �������.
        m_uiReset = 0;
        // ��������� ���� ���������� - ����� � ����������� ������.
        uiLocalBou |= (1 << GLOBAL_RESET_BUTTON_BIT);
    }

    uint8_t uiLength = 0;

    m_puiTxBuffer[uiLength++] = DATA_EXCHANGE_COMMAND;
    // ��������������� ������ ������� �����.
    m_puiTxBuffer[uiLength++] = uiLength;


    uint8_t uiData = 0;
    // �������� ���������� ��������� ��� ������� ����.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        if (m_pxDiscreteOutputControl[i].uiRelayActivationRequest)
        {
            uiData |= (1 << i);
        }
    }
    m_puiTxBuffer[uiLength++] = uiData;

    uiData = 0;
    // �������� ����� ��������� ��� ������� ����.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        if (m_pxDiscreteOutputControl[i].uiNewActivation)
        {
            uiData |= (1 << i);
        }
    }
    m_puiTxBuffer[uiLength++] = uiData;
    m_puiTxBuffer[uiLength++] = uiExternalReceipt;
    m_puiTxBuffer[uiLength++] = uiExternalReset;
    m_puiTxBuffer[uiLength++] = uiLocalBou;
    // ������ ������� �����.
    m_puiTxBuffer[SPI_MT_LENGTH_OFFSET] = uiLength;
    // ������ Crc.
    m_puiTxBuffer[uiLength] = HammingDistanceCrc(m_puiTxBuffer,
                              (SPI_MT_PRIAMBLE_LENGTH +
                               DATA_EXCHANGE_COMMAND_ANSWER_LENGTH));

    m_pxCommunicationDevice -> Exchange(m_uiAddress,
                                        m_puiTxBuffer,
                                        m_puiRxBuffer,
                                        (SPI_MT_PRIAMBLE_LENGTH +
                                         DATA_EXCHANGE_COMMAND_ANSWER_LENGTH +
                                         ONE_BYTE_CRC_LENGTH),
                                        LOW_SPEED_IN_HZ);

    // ������ �� ����������?
    if (m_puiRxBuffer[m_puiTxBuffer[SPI_MT_RX_OFFSET + SPI_MT_LENGTH_OFFSET]] =
                HammingDistanceCrc(&m_puiRxBuffer[SPI_MT_RX_OFFSET],
                                   (m_puiTxBuffer[SPI_MT_RX_OFFSET + SPI_MT_LENGTH_OFFSET])))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CModuleMrXXDriver::Exchange(void)
{
//    uint8_t uiExternalReceipt = 0;
//    // ���� ��������� ������� �������� ������������.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        // ��� �������� ����� ������� ������������ �����������������?
//        if (m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i] != 0xFF)
//        {
//            // ��� �������� ����� ���� ������� ������������?
//            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i]])
//            {
//                // ��������� ������������ ��� �������� �����.
//                uiExternalReceipt |= (1 << i);
//            }
//        }
//    }
//
//    uint8_t uiExternalReset = 0;
//    // ���� ��������� ������� �������� ������.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        // ��� �������� ����� ������� ����� ����������������?
//        if (m_pxDiscreteOutputDataBase -> ExternalResetAddress[i] != 0xFF)
//        {
//            // ��� �������� ����� ���� ������� �����?
//            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalResetAddress[i]])
//            {
//                // ��������� ����� ��� �������� �����.
//                uiExternalReset |= (1 << i);
//            }
//        }
//    }
//
//    uint8_t uiLocalBou = 0;
//    // ���� ������������ Modbus?
//    if (m_uiModbusReceipt)
//    {
//        // ������� ���� ����������� �������.
//        m_uiModbusReceipt = 0;
//        // ��������� ���� ���������� - ������������ Modbus.
//        uiLocalBou |= (1 << GLOBAL_KVIT_PC_BIT);
//    }
//    // ���� ����� Modbus?
//    if (m_uiModbusReset)
//    {
//        // ������� ���� ����������� �������.
//        m_uiModbusReset = 0;
//        // ��������� ���� ���������� - ����� Modbus.
//        uiLocalBou |= (1 << GLOBAL_RESET_PC_BIT);
//    }
//    // ���� ������������ � ������ ����������� ������?
//    if (m_uiReceipt)
//    {
//        // ������� ���� ����������� �������.
//        m_uiReceipt = 0;
//        // ��������� ���� ���������� - ������������ � ����������� ������.
//        uiLocalBou |= (1 << GLOBAL_KVIT_BUTTON_BIT);
//    }
//    // ���� ����� � ������ ����������� ������?
//    if (m_uiReset)
//    {
//        // ������� ���� ����������� �������.
//        m_uiReset = 0;
//        // ��������� ���� ���������� - ����� � ����������� ������.
//        uiLocalBou |= (1 << GLOBAL_RESET_BUTTON_BIT);
//    }
//
//    m_puiTxBuffer[0] = DATA_EXCHANGE_COMMAND;
//    uint8_t *puiDestination;
//    puiDestination = &m_puiTxBuffer[SPI_MT_DATA_OFFSET];
//    uint8_t uiData = 0;
//    // �������� ���������� ��������� ��� ������� ����.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        if (m_pxDiscreteOutputControl[i].uiRelayActivationRequest)
//        {
//            uiData |= (1 << i);
//        }
//    }
//    *puiDestination++ = uiData;
//
//    uiData = 0;
//    // �������� ����� ��������� ��� ������� ����.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        if (m_pxDiscreteOutputControl[i].uiNewActivation)
//        {
//            uiData |= (1 << i);
//        }
//    }
//    *puiDestination++ = uiData;
//
//    *puiDestination++ = uiExternalReceipt;
//    *puiDestination++ = uiExternalReset;
//    *puiDestination++ = uiLocalBou;
//    uint16_t usData = usCrcSummTwoByteCalculation(&m_puiTxBuffer[SPI_MT_DATA_OFFSET],
//                      DATA_EXCHANGE_COMMAND_ANSWER_LENGTH);
//    *puiDestination++ = (uint8_t)usData;
//    *puiDestination++ = (uint8_t)(usData >> 8);
//    *puiDestination++ = DATA_EXCHANGE_OK;
//
//    CPss21::ConnectDevice(m_uiAddress);
//    delay_us(500);
//    CSpi::Exchange(m_puiRxBuffer,
//                   m_puiTxBuffer,
//                   (PRIAMBLE_LENGTH +
//                    DATA_EXCHANGE_COMMAND_ANSWER_LENGTH +
//                    TWO_BYTE_CRC_LENGTH +
//                    DATA_EXCHANGE_OK_LENGTH));
//    CPss21::DisconnectDevice();
//
//    // ��� ������� ������?
//    switch(m_puiRxBuffer[COMMAND_BYTE_OFFSET])
//    {
//    case DATA_READY:
//        // ������ �� ����������?
//        if (iCrcSummTwoByteCompare(&m_puiRxBuffer[SPI_MT_DATA_OFFSET],
//                                   DATA_EXCHANGE_COMMAND_ANSWER_LENGTH) > 0)
//        {
//            // ���� �������������?
//            if (m_puiRxBuffer[SPI_MT_DATA_OFFSET +
//                              DATA_EXCHANGE_COMMAND_ANSWER_LENGTH +
//                              TWO_BYTE_CRC_LENGTH] == DATA_EXCHANGE_OK)
//            {
//                // ���� �������������.
//                m_uiBadAnswerCounter = 0;
//                SetErrorAlarmData(0);
//
//                uiData = (m_puiRxBuffer[SPI_MT_DATA_OFFSET]);
//                // �������� ������ - ��������� ������� ������� ���� �� �1 � MP �8.
//                for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//                {
//                    if (uiData & (1 << i))
//                    {
//                        m_puiDiscreteOutputState[i] = 1;
//                    }
//                    else
//                    {
//                        m_puiDiscreteOutputState[i] = 0;
//                    }
//                }
//
//                return 1;
//            }
//        }
//        break;
//
//    case DBASE_ERR:
//        // ������ �� ������� �����������?
//        if (m_uiBadAnswerCounter < BAD_MODULE_CYCLE_COUNT_DEFAULT)
//        {
//            // ���� ������ ����������. ���������� �������� � ��������.
//            SetFsmState(MRXX_FSM_STATE_WRITE_DATABASE);
//        }
//        break;
//
//    default:
//        break;
//    };
//
//    // ������ �� ������� �����������?
//    if (m_uiBadAnswerCounter < BAD_MODULE_CYCLE_COUNT_DEFAULT)
//    {
//        // �������� �������� �������� - "���������� ������� ����� � ������� ��� ������".
//        m_uiBadAnswerCounter++;
//        return 0;
//    }
//    else
//    {
//        // ������ ������� �����������.
//        *m_puiErrorCode = OMD_ERROR;
//        SetErrorAlarmData(1);
//        return 0;
//    }
}

////-----------------------------------------------------------------------------------------------------
//uint8_t CModuleMrXXDriver::DataExchange(void)
//{
////    uint8_t uiResult;
////    switch (GetFsmState())
////    {
////        // ��� ������ ���� ������ ������������ �� ������� � ������ ����.
////    case MRXX_FSM_STATE_WRITE_DATABASE:
////        uiResult = WriteDataBase();
////        // �����, ������� ��������� � ����� - ����� ������� � �������(MRXX_FSM_STATE_TRANSFER).
////        SetFsmState(MRXX_FSM_STATE_TRANSFER);
////        break;
////
////    case MRXX_FSM_STATE_TRANSFER:
////        uiResult = Exchange();
////        break;
////
////    default:
////        break;
////    };
////
////    return uiResult;
//}

//    SPI_MT_PRIAMBLE_LENGTH = 2,
//    SPI_MT_FUNCTION_CODE_OFFSET = 0,
//    SPI_MT_LENGTH_OFFSET = 1,
//    SPI_MT_DATA_OFFSET = 2,
//-----------------------------------------------------------------------------------------------------
// ������� ������� ������ ���������.
void CModuleMrXXDriver::Fsm(void)
{
    enum
    {
        IDDLE = 0,
        CHANGE_PROTOCOL,
        WRITE_DATA_BASE,
        WRITE_DATA_BASE_CHECK,
    };

    switch (GetFsmState())
    {
    case IDDLE:
        break;

    case CHANGE_PROTOCOL:
        if (ChangeProtocol())
        {
            SetFsmState(WRITE_DATA_BASE);
        }
        break;

    case WRITE_DATA_BASE:
        WriteDataBase();
        SetFsmState(WRITE_DATA_BASE_CHECK);
        break;

    case WRITE_DATA_BASE_CHECK:
        DataExchange();
        // �������� ����� ������ �� ���������� ������� - ������ ���� ������.
        switch (m_puiRxBuffer[(SPI_MT_RX_OFFSET + SPI_MT_FUNCTION_CODE_OFFSET)])
        {
            // ���� ������ �������� �������?
        case MRXX_SET_DATA_BASE_COMMAND:
            break;
            // �� ����� ������ ���� ������ ��������� ������?
        case MRXX_SET_DATA_BASE_COMMAND_ERROR:
            SetFsmState(WRITE_DATA_BASE);
            break;
            // �� ����� ������ ���� ������ ��������� ������?
        default:
            SetFsmState(WRITE_DATA_BASE);
            break;
        }
        break;

    default:
        break;
    }
}


//-----------------------------------------------------------------------------------------------------
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
// записывает базу данных из RAM прибора в EEPROM модуля.
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

    // модуль отвечает?
    if (m_puiRxBuffer[SPI_COMMAND_BYTE_OFFSET] ==
            CHANGE_PROTOCOL_COMMAND) // if echo answer is ok.
    {
        // обмен данными прошёл без ошибок?
        if ((iCrcSummOneByteCompare(&m_puiRxBuffer[SPI_COMMAND_BYTE_OFFSET],
                                    (GET_MODULE_TYPE_COMMAND_LENGTH +
                                     GET_MODULE_TYPE_COMMAND_ANSWER_LENGTH)) > 0))
        {
            // есть подтверждение записи базы данных в EEPROM модуля.
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
//    // Получим указатель на массив дискретных входов.
//    m_puiDiscreteInputs = xMemoryAllocationConext.puiDiscreteInputs;
//
//    // Получим указатель на массив базы данных.
//    m_pxDiscreteOutputDataBase = xMemoryAllocationConext.pxDiscreteOutputDataBase;//debag//
//
//    m_puiDiscreteOutputState = xMemoryAllocationConext.puiDiscreteOutputState;
//
//    // Получим указатель на место в массиве дискретных выходов для текущего модуля.
//    m_pxDiscreteOutputControl =
//        &xMemoryAllocationConext.
//        pxDiscreteOutputControl[xMemoryAllocationConext.uiUsedDiscreteOutputs];
//    // Увеличим общий объём выделенной памяти.
//    xMemoryAllocationConext.
//    uiUsedDiscreteOutputs +=
//        MR_DISCRETE_OUTPUT_NUMBER;
//
//    m_pxDiscreteOutputDataBase =
//        &xMemoryAllocationConext.
//        pxDiscreteOutputDataBase[xMemoryAllocationConext.uiUsedDiscreteOutputDataBase];
//    // Увеличим общий объём выделенной памяти.
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
    // Предварительная запись размера кадра.
    m_puiTxBuffer[uiLength++] = uiLength;
    // При загрузке БД в МР передается только байт 0 описателя каждого выхода.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        m_puiTxBuffer[i + uiLength] =
            m_pxDiscreteOutputDataBase -> DiscreteOutputModuleDataBase[i];
        uiLength++;
    }
    // Запись размера кадра.
    m_puiTxBuffer[SPI_MT_LENGTH_OFFSET] = uiLength;
    // Запись Crc.
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
    // цикл проверяет наличие внешнего квитирования.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        // для текущего входа внешнее квитирование запрограммировано?
        if (m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i] != 0xFF)
        {
            // для текущего входа есть внешнее квитирование?
            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i]])
            {
                // установим квитирование для текущего входа.
                uiExternalReceipt |= (1 << i);
            }
        }
    }

    uint8_t uiExternalReset = 0;
    // цикл проверяет наличие внешнего сброса.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        // для текущего входа внешний сброс запрограммирован?
        if (m_pxDiscreteOutputDataBase -> ExternalResetAddress[i] != 0xFF)
        {
            // для текущего входа есть внешний сброс?
            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalResetAddress[i]])
            {
                // установим сброс для текущего входа.
                uiExternalReset |= (1 << i);
            }
        }
    }

    uint8_t uiLocalBou = 0;
    // есть квитирование Modbus?
    if (m_uiModbusReceipt)
    {
        // сбросим флаг поступившей команды.
        m_uiModbusReceipt = 0;
        // установим флаг управления - квитирование Modbus.
        uiLocalBou |= (1 << GLOBAL_KVIT_PC_BIT);
    }
    // есть сброс Modbus?
    if (m_uiModbusReset)
    {
        // сбросим флаг поступившей команды.
        m_uiModbusReset = 0;
        // установим флаг управления - сброс Modbus.
        uiLocalBou |= (1 << GLOBAL_RESET_PC_BIT);
    }
    // есть квитирование с кнопки центральной панели?
    if (m_uiReceipt)
    {
        // сбросим флаг поступившей команды.
        m_uiReceipt = 0;
        // установим флаг управления - квитирование с центральной кнопки.
        uiLocalBou |= (1 << GLOBAL_KVIT_BUTTON_BIT);
    }
    // есть сброс с кнопки центральной панели?
    if (m_uiReset)
    {
        // сбросим флаг поступившей команды.
        m_uiReset = 0;
        // установим флаг управления - сброс с центральной кнопки.
        uiLocalBou |= (1 << GLOBAL_RESET_BUTTON_BIT);
    }

    uint8_t uiLength = 0;

    m_puiTxBuffer[uiLength++] = DATA_EXCHANGE_COMMAND;
    // Предварительная запись размера кадра.
    m_puiTxBuffer[uiLength++] = uiLength;


    uint8_t uiData = 0;
    // заполним требования включения для каждого реле.
    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
    {
        if (m_pxDiscreteOutputControl[i].uiRelayActivationRequest)
        {
            uiData |= (1 << i);
        }
    }
    m_puiTxBuffer[uiLength++] = uiData;

    uiData = 0;
    // заполним новые нарушения для каждого реле.
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
    // Запись размера кадра.
    m_puiTxBuffer[SPI_MT_LENGTH_OFFSET] = uiLength;
    // Запись Crc.
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

    // Данные не повреждены?
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
//    // цикл проверяет наличие внешнего квитирования.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        // для текущего входа внешнее квитирование запрограммировано?
//        if (m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i] != 0xFF)
//        {
//            // для текущего входа есть внешнее квитирование?
//            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalReceiptAddress[i]])
//            {
//                // установим квитирование для текущего входа.
//                uiExternalReceipt |= (1 << i);
//            }
//        }
//    }
//
//    uint8_t uiExternalReset = 0;
//    // цикл проверяет наличие внешнего сброса.
//    for (uint8_t i = 0; i < MR_DISCRETE_OUTPUT_NUMBER; i++)
//    {
//        // для текущего входа внешний сброс запрограммирован?
//        if (m_pxDiscreteOutputDataBase -> ExternalResetAddress[i] != 0xFF)
//        {
//            // для текущего входа есть внешний сброс?
//            if (m_puiDiscreteInputs[m_pxDiscreteOutputDataBase -> ExternalResetAddress[i]])
//            {
//                // установим сброс для текущего входа.
//                uiExternalReset |= (1 << i);
//            }
//        }
//    }
//
//    uint8_t uiLocalBou = 0;
//    // есть квитирование Modbus?
//    if (m_uiModbusReceipt)
//    {
//        // сбросим флаг поступившей команды.
//        m_uiModbusReceipt = 0;
//        // установим флаг управления - квитирование Modbus.
//        uiLocalBou |= (1 << GLOBAL_KVIT_PC_BIT);
//    }
//    // есть сброс Modbus?
//    if (m_uiModbusReset)
//    {
//        // сбросим флаг поступившей команды.
//        m_uiModbusReset = 0;
//        // установим флаг управления - сброс Modbus.
//        uiLocalBou |= (1 << GLOBAL_RESET_PC_BIT);
//    }
//    // есть квитирование с кнопки центральной панели?
//    if (m_uiReceipt)
//    {
//        // сбросим флаг поступившей команды.
//        m_uiReceipt = 0;
//        // установим флаг управления - квитирование с центральной кнопки.
//        uiLocalBou |= (1 << GLOBAL_KVIT_BUTTON_BIT);
//    }
//    // есть сброс с кнопки центральной панели?
//    if (m_uiReset)
//    {
//        // сбросим флаг поступившей команды.
//        m_uiReset = 0;
//        // установим флаг управления - сброс с центральной кнопки.
//        uiLocalBou |= (1 << GLOBAL_RESET_BUTTON_BIT);
//    }
//
//    m_puiTxBuffer[0] = DATA_EXCHANGE_COMMAND;
//    uint8_t *puiDestination;
//    puiDestination = &m_puiTxBuffer[SPI_MT_DATA_OFFSET];
//    uint8_t uiData = 0;
//    // заполним требования включения для каждого реле.
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
//    // заполним новые нарушения для каждого реле.
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
//    // что ответил модуль?
//    switch(m_puiRxBuffer[COMMAND_BYTE_OFFSET])
//    {
//    case DATA_READY:
//        // данные не повреждены?
//        if (iCrcSummTwoByteCompare(&m_puiRxBuffer[SPI_MT_DATA_OFFSET],
//                                   DATA_EXCHANGE_COMMAND_ANSWER_LENGTH) > 0)
//        {
//            // есть подтверждение?
//            if (m_puiRxBuffer[SPI_MT_DATA_OFFSET +
//                              DATA_EXCHANGE_COMMAND_ANSWER_LENGTH +
//                              TWO_BYTE_CRC_LENGTH] == DATA_EXCHANGE_OK)
//            {
//                // есть подтверждение.
//                m_uiBadAnswerCounter = 0;
//                SetErrorAlarmData(0);
//
//                uiData = (m_puiRxBuffer[SPI_MT_DATA_OFFSET]);
//                // отправим данные - состояние выходов модулей реле МР №1 – MP №8.
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
//        // модуль не признан неисправным?
//        if (m_uiBadAnswerCounter < BAD_MODULE_CYCLE_COUNT_DEFAULT)
//        {
//            // База данных повреждена. Попытаемся записать её повторно.
//            SetFsmState(MRXX_FSM_STATE_WRITE_DATABASE);
//        }
//        break;
//
//    default:
//        break;
//    };
//
//    // модуль не признан неисправным?
//    if (m_uiBadAnswerCounter < BAD_MODULE_CYCLE_COUNT_DEFAULT)
//    {
//        // увеличим значение счётчика - "количество сеансов связи с модулем без ответа".
//        m_uiBadAnswerCounter++;
//        return 0;
//    }
//    else
//    {
//        // модуль признан неисправным.
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
////        // при старте база данных записывается из прибора в модуль реле.
////    case MRXX_FSM_STATE_WRITE_DATABASE:
////        uiResult = WriteDataBase();
////        // после, автомат переходит в режим - обмен данными с модулем(MRXX_FSM_STATE_TRANSFER).
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
// Главный автомат записи хранилища.
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
        // Проверим ответ модуля на предыдущую команду - запись базы данных.
        switch (m_puiRxBuffer[(SPI_MT_RX_OFFSET + SPI_MT_FUNCTION_CODE_OFFSET)])
        {
            // База данных записана успешно?
        case MRXX_SET_DATA_BASE_COMMAND:
            break;
            // Во время записи базы данных произощла ошибка?
        case MRXX_SET_DATA_BASE_COMMAND_ERROR:
            SetFsmState(WRITE_DATA_BASE);
            break;
            // Во время записи базы данных произощла ошибка?
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

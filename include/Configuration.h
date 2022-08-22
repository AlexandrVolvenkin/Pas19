//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED


//-----------------------------------------------------------------------------------------------------
enum
{
    MODULE_TYPE_MVDI = 0x01,
    MODULE_TYPE_MVSN = 0x01,
    MODULE_TYPE_MVDI5 = 0x01,
    MODULE_TYPE_MVDS9 = 0x01,
    MODULE_TYPE_MVDO5 = 0x02,
    MODULE_TYPE_MR53  = 0x03,
    MODULE_TYPE_MR54  = 0x04,
    MODULE_TYPE_MVI   = 0x05
};

enum
{
    MVSN21_DISCRETE_INPUTS_NUMBER = 24,
    MR_DISCRETE_OUTPUT_NUMBER = 8,
};

enum
{
    // Конфигурация устройства.
    DISCRETE_INPUT_MODULE_MAX_NUMBER = 2,
    DISCRETE_INPUTS_NUMBER = (MVSN21_DISCRETE_INPUTS_NUMBER *
                              DISCRETE_INPUT_MODULE_MAX_NUMBER),
    MAX_DISCRETE_INPUTS_NUMBER = 64,
    DISCRETE_OUTPUT_MODULE_MAX_NUMBER = 1,
    DISCRETE_OUTPUT_NUMBER = (MR_DISCRETE_OUTPUT_NUMBER *
                              DISCRETE_OUTPUT_MODULE_MAX_NUMBER),
    INTERNAL_MODULE_QUANTITY = (DISCRETE_INPUT_MODULE_MAX_NUMBER +
                                DISCRETE_OUTPUT_MODULE_MAX_NUMBER),
    ALARM_WINDOWS_NUMBER = 48,
    MAX_ALARM_WINDOWS_NUMBER = 64,
    DISCRETE_SIGNALS_NUMBER = (DISCRETE_INPUTS_NUMBER +
                               ALARM_WINDOWS_NUMBER),
    // Карта памяти устройства.
    MODBUS_RECEIPT_ADDRESS = 0,
    MODBUS_RECEIPT_ARRAY_LENGTH = 1,
    MODBUS_RECEIPT_BIT_ARRAY_OFFSET = 0,

    MODBUS_RESET_ADDRESS = 1,
    MODBUS_RESET_ARRAY_LENGTH = 1,
    MODBUS_RESET_BIT_ARRAY_OFFSET = (MODBUS_RECEIPT_BIT_ARRAY_OFFSET +
                                     MODBUS_RECEIPT_ARRAY_LENGTH),

    NOT_USED_ARRAY_LENGTH = 14,
    NOT_USED_BIT_ARRAY_OFFSET = (MODBUS_RESET_BIT_ARRAY_OFFSET +
                                 MODBUS_RESET_ARRAY_LENGTH),

    DISCRETE_INPUTS_ARRAY_LENGTH = DISCRETE_INPUTS_NUMBER,
    DISCRETE_INPUTS_BIT_ARRAY_OFFSET = (NOT_USED_BIT_ARRAY_OFFSET +
                                        NOT_USED_ARRAY_LENGTH),
    ALARM_WINDOWS_ARRAY_LENGTH = ALARM_WINDOWS_NUMBER,
    ALARM_WINDOWS_BIT_ARRAY_OFFSET = (DISCRETE_INPUTS_BIT_ARRAY_OFFSET +
                                      DISCRETE_INPUTS_ARRAY_LENGTH),

    DISCRETE_OUTPUT_ARRAY_LENGTH = DISCRETE_OUTPUT_NUMBER,
    DISCRETE_OUTPUT_BIT_ARRAY_OFFSET = (ALARM_WINDOWS_BIT_ARRAY_OFFSET +
                                        ALARM_WINDOWS_ARRAY_LENGTH),
    DISCRETE_DATA_ARRAY_LENGTH = (MODBUS_RECEIPT_ARRAY_LENGTH +
                                  MODBUS_RESET_ARRAY_LENGTH +
                                  NOT_USED_ARRAY_LENGTH +
                                  DISCRETE_INPUTS_ARRAY_LENGTH +
                                  ALARM_WINDOWS_ARRAY_LENGTH +
                                  DISCRETE_OUTPUT_ARRAY_LENGTH),
    HOLDING_REGISTERS_ARRAY_LENGTH = 8,//debag//
    INPUT_REGISTERS_ARRAY_LENGTH = 8,
};

enum
{
    MODBUS_RTU_BAUD_RATE = 9600UL,
    MODBUS_RTU_MAX_ADU_LENGTH = 256,
    MAIN_CYCLE_PERIOD_TIME = 100,
    PROGRAMMING_TIME = 1000,
    PERIPHERY_SCAN_TIME = 10,
    SPI_PRIAMBLE_LENGTH = 2,
    SPI_BUFFER_LENGTH = 64,
    SPI_MAX_BUS_ADDRESS = 16,
// 2 - смещение на данные в массиве RX SPI. 0 - команда запроса, 1 - "эхо" команды запроса.
    SPI_DATA_BYTE_OFFSET = 2,
// 1 - смещение на команду в массиве RX SPI.
    SPI_COMMAND_BYTE_OFFSET = 1,
    SPI_COMMAND_BYTE_LENGTH = 1,
    ONE_BYTE_CRC_LENGTH = 1,
    TWO_BYTE_CRC_LENGTH = 2,
    GET_MODULE_TYPE_COMMAND = 0x91,
    GET_MODULE_TYPE_REQUEST_LENGTH = 4,
    MODULE_REQUEST_QUANTITY = 5,
    MODULE_TYPE_OFFSET = SPI_DATA_BYTE_OFFSET,
// начальное значение счётчика запросов модуля без ответа.
// если равно нулю - модуль считается неисправным. его данные признаются недостоверными.
// устанавливается флаг сигнализации в рабочем массиве aucCoilsArray[BAD_MODULES_BIT_ARRAY_OFFSET + индекс модуля].
    BAD_MODULE_CYCLE_COUNT_DEFAULT = 10,
};

enum TYPE_INDICATION
{
    NORMAL		 = 0,			// Нет отображения
    INDICATION	 = 1,			// Индикация
    PREVENTIVE	 = 2,			// Предупредительная сигнализация
    EMERGENCY	 = 3,			// Аварийная сигнализация

    IND_PREVENTIVE = 4,			// Индикация + предупредительная сигнализация
    IND_EMERGENCY  = 5,			// Индикация + аварийная сигнализация
    ERROR  = 6,			// Звук нажатия кнопок.
    BEEP_SIGNAL  = 7,			// Звук нажатия кнопок.
};

enum Errors
{
    NO_ERROR	= 0,
    IMD_ERROR	= 1,		// Ошибка связи с модулями ввода
    OMD_ERROR	= 2,		// Ошибка связи с модулями вывода
    CFG_ERROR	= 3,		// Ошибка конфигурации
    DB_ERROR	= 4,		// Ошибка БД
    MBS_ERROR	= 5,		// Ошибка канала связи
};

enum EErrorAlarmType
{
    DISCRETE_INPUT_MODULE_FAILURE = 0,
    DISCRETE_OUTPUT_MODULE_FAILURE,
    HANDLED_ERROR_NUMBER = 2,
    ERROR_TYPE_LENGTH = 1,
};

enum EErrorAlarmWindowIndex
{
    DISCRETE_INPUT_MODULE_FAILURE_WINDOW_INDEX = 0,
    DISCRETE_OUTPUT_MODULE_FAILURE_WINDOW_INDEX,
};

//-----------------------------------------------------------------------------------------------------
#endif // CONFIGURATION_H_INCLUDED

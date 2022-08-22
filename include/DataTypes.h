//-----------------------------------------------------------------------------------------------------
//  Sourse      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef DATATYPES_H_INCLUDED
#define DATATYPES_H_INCLUDED

#include "Configuration.h"

//-----------------------------------------------------------------------------------------------------
// Версия программного обеспечения
struct TFirmwareVersion
{
    unsigned char Major;
    unsigned char Minor;
    unsigned char Hardware;
};

// Конфигурация устройства (НОВАЯ v1.60)
#pragma bitfields = default
struct TDevConfig
{
    unsigned char  ICount;			// Число модулей ввода
    unsigned char  OCount;			// Число модулей вывода
    unsigned char  Windows;		// Число окон сигнализации

    unsigned short FixConfig :  1;		// Признак фиксированной конфигурации
    unsigned short MBusMaster:  1;		// Признак режима ModBus-Master
    unsigned short 	  : 14;		// РЕЗЕРВ

    unsigned char MBTimeout;		// Таймаут канала ModBus

    char BYTE[14];				// РЕЗЕРВ
};

// Настройки интерфейса ModBus
struct TMBusSetting
{
    unsigned char NetAddress;		// Сетевой адрес
    unsigned char BaudRate;		// Скорость обмена
    unsigned char LinkParam;		// Число стоп-бит, паритет
    unsigned char Interval;		// Интервал таймера
    unsigned char Prescaler;		// Делитель таймера
    unsigned char Silent[2];		// 2 байта под дополнительную задержку

    enum
    {
        STOPBITS_MASK = 0x08,
        PARITY_MASK   = 0x30,

        ONESTOPBITS	= 0x00,			// Один стоп-бит
        TWOSTOPBITS	= 0x08,			// Два стоп-бита
        NOPARITY	= 0x00,			// Без паритета
        EVENPARITY	= 0x20,			// Четный паритет
        ODDPARITY	= 0x30			// Нечетный паритета
    };
};

// Описатель запроса ModBus Master
struct TMBMRequest
{
    unsigned char  NetAddress;		// Сетевой адрес устройства
    unsigned char  Function;		// Функция
    unsigned short Address;		// Адрес данных
    unsigned char  Value;			// Значение
};

// Описатель списка запросов ModBus Master
struct TMBMRequestList
{
    unsigned char Number;			// Число элементов списка
    TMBMRequest   Request[16];		// Запросы ModBus Master
};

// Описатель настроек каналов индикации
#pragma bitfields = reversed
union TChannelSetting
{
    struct
    {
        unsigned char NoSound:	1;	// Признак отключения звука
        unsigned char :		3;
        unsigned char Type:		4;	// Тип отображения / обработки
    };

    unsigned char Byte;

    enum TYPE_INDICATION
    {
        NORMAL		 = 0,			// Нет отображения
        INDICATION	 = 1,			// Индикация
        PREVENTIVE	 = 2,			// Предупредительная сигнализация
        EMERGENCY	 = 3,			// Аварийная сигнализация

        IND_PREVENTIVE = 4,			// Индикация + предупредительная сигнализация
        IND_EMERGENCY  = 5			// Индикация + аварийная сигнализация
    };
};

// Описатель настроек канала модуля вывода
#pragma bitfields = reversed
union TOutSetting
{
    struct
    {
        unsigned char :		2;
        unsigned char LocalReceipt:	1;	// Квитирование с панели
        unsigned char LocalUnset:	1;	// Сброс с панели
        unsigned char MBusReceipt:	1;	// Квитирование с верхнего уровня
        unsigned char MBusUnset:	1;	// Сброс с верхнего уровня
        unsigned char Mode:		2;	// Режим выхода
    };

    unsigned char Byte;

    enum OUT_MODE
    {
        LIGHT_SIGNALING  = 0x00,		// Световая сигнализация
        SOUND_SIGNALING  = 0x02,		// Звуковая сигнализация
        CONTROL	   = 0x01,		// Управляющий выход
        SELFHOLD_CONTROL = 0x03		// Управляющий выход с самоблокировкой
    };

    operator unsigned char()		// Оператор преобразования к unsigned char
    {
        return Byte;
    };
};

// Описатель канала вывода
struct TOutputData
{
    unsigned char DiscreteOutputModuleDataBase[DISCRETE_OUTPUT_NUMBER];		// Настройки МР-54
    unsigned char ExternalReceiptAddress[DISCRETE_OUTPUT_NUMBER];		// Адреса входов внешнего квитирования
    unsigned char ExternalResetAddress[DISCRETE_OUTPUT_NUMBER];		// Адреса входов внешнего сброса
};

struct TDiscreteOutputControl
{
    // Новое нарушение.
    uint8_t uiNewActivation;
    // Требование включения реле.
    uint8_t uiRelayActivationRequest;
};

struct TAlarmDfaInit
{
    // Уровень дискретного сигнала интерпретируемый как активный.
    uint8_t uiActivityLevel;
    uint8_t uiAlarmType;
    uint8_t uiSavedAlarmType;
};

#pragma bitfields = default
struct TDeviceState
{
    // Признак работы в режиме конроля линии связи ModBus.
    uint8_t ModbusRtuLinkControlInProgress:  1;
    uint8_t 	  : 7;

    uint8_t uiReserved[7];
};

//-----------------------------------------------------------------------------------------------------
// Описатель структур хранения данных в EEPROM
struct TDataBaseBlockPositionData
{
    unsigned char  Field;			// Идентификатор поля данных
    unsigned char  Size;			// Размер блока
    unsigned short Offset;			// Смещение блока данных от начала
};

//-----------------------------------------------------------------------------------------------------
// БД настроек прибора
#pragma pack(push)
#pragma pack(1)
struct TDataBase
{
    // Конфигурация устройства.
    TDevConfig	DevConfig;
    unsigned short CRC0;
    // Параметры интерфейса ModBus.
    TMBusSetting   MBSet;
    unsigned short CRC1;
    // Уровень дискретного сигнала интерпретируемый как активный.
    unsigned char  ActivityLevel[((MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER) / 8)];
    unsigned short CRC2;
    // Индекс окна сигнализации которое привязано к текущему дискретному сигналу.
    unsigned char  AlarmWindowIndex[(MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER)];
    unsigned short CRC3;
    // Тип сигнализации привязанный к окну.
    unsigned char  AlarmType[MAX_ALARM_WINDOWS_NUMBER];
    unsigned short CRC4;
    // БД модулей вывода.
    TOutputData    OutConfig;
    unsigned short CRC5;
    // Выходные реле, сопоставленные физическим входам.
    unsigned char  Relay[(MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER)];
    unsigned short CRC6;
    // Список запросов сбора данных.
    TMBMRequestList InputList;
    unsigned short  CRC7;
    // Список запросов квитирования.
    TMBMRequestList ReceiptList;
    unsigned short  CRC8;
    // Список запросов сброса.
    TMBMRequestList UnsetList;
    unsigned short  CRC9;
    // Состояние устройства хранимое в EEPROM.
    TDeviceState DeviceState;
    unsigned short CRC10;

    enum DB_FIELDS
    {
        DEV_CONFIG	= 0,			// Конфигурация устройства
        MODBUS_SET	= 1,			// Параметры интерфейса ModBus
        ACTIVITY_LEVEL	= 2,			// Тип входа
        ALARM_WINDOW_INDEX	= 3,			// Переадресация
        ALARM_TYPE	= 4,			// Типы сигнализации по окнам
        OUT_BASE	= 5,			// БД модулей вывода
        RELAY		= 6,			// Выходные реле, сопоставленные физическим входам
        INPUT_LIST	= 7,			// Запросы сбора данных
        RECEIPT_LIST	= 8,			// Запросы квитирования
        UNSET_LIST	= 9,			// Запросы сброса
        DEVICE_STATE = 10,			// Состояние устройства хранимое в EEPROM.
        BLOCKS_QUANTITY = 11
    };
};
#pragma pack(pop)

//-----------------------------------------------------------------------------------------------------
#pragma pack(push)
#pragma pack(1)
struct TModuleContext
{
    uint8_t uiType;
    uint8_t uiAddress;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct TConfigDataPackOne
{
    // фактическое количество модулей.
    uint8_t uiModulesQuantity;
    TModuleContext axModulesContext[INTERNAL_MODULE_QUANTITY];
    uint8_t uiDiscreteOutputQuantity;
    uint8_t uiDiscreteInputQuantity;
//    uint8_t uiDisplayType;
//    uint8_t uiErrorCode;
};
#pragma pack(pop)

// для сериализации, разбора данных полученных по интерфейсам передачи данных.
#pragma pack(push)
#pragma pack(1)
struct TConfigDataPackOneOld
{
    // фактическое количество модулей.
    uint8_t uiModulesQuantity;
    uint8_t auiModuleType[INTERNAL_MODULE_QUANTITY];
    uint8_t uiDiscreteOutputQuantity;
    uint8_t uiDiscreteInputQuantity;
    uint8_t uiDisplayType;
    uint8_t uiErrorCode;
};
#pragma pack(pop)

struct TMemoryAllocationConext
{
    uint8_t uiAddress;
    uint8_t *puiRxBuffer;
    uint8_t *puiTxBuffer;
    uint8_t *puiErrorCode;
    uint8_t *puiDiscreteInputs;
    uint8_t uiUsedDiscreteInputs;
    uint8_t *puiDiscreteOutputState;
    uint8_t uiUsedDiscreteOutputState;
    TDiscreteOutputControl *pxDiscreteOutputControl;
    uint8_t uiUsedDiscreteOutputs;
    TOutputData *pxDiscreteOutputDataBase;
//    uint8_t *puiExternalReceiptAddress;
//    uint8_t *puiExternalResetAddress;
    uint8_t uiUsedDiscreteOutputDataBase;
    uint8_t* puiErrorAlarmDataArray;
    uint8_t uiUsedErrorAlarmDataArray;
};

#endif // DATATYPES_H_INCLUDED

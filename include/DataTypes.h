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
// ������ ������������ �����������
struct TFirmwareVersion
{
    unsigned char Major;
    unsigned char Minor;
    unsigned char Hardware;
};

// ������������ ���������� (����� v1.60)
#pragma bitfields = default
struct TDevConfig
{
    unsigned char  ICount;			// ����� ������� �����
    unsigned char  OCount;			// ����� ������� ������
    unsigned char  Windows;		// ����� ���� ������������

    unsigned short FixConfig :  1;		// ������� ������������� ������������
    unsigned short MBusMaster:  1;		// ������� ������ ModBus-Master
    unsigned short 	  : 14;		// ������

    unsigned char MBTimeout;		// ������� ������ ModBus

    char BYTE[14];				// ������
};

// ��������� ���������� ModBus
struct TMBusSetting
{
    unsigned char NetAddress;		// ������� �����
    unsigned char BaudRate;		// �������� ������
    unsigned char LinkParam;		// ����� ����-���, �������
    unsigned char Interval;		// �������� �������
    unsigned char Prescaler;		// �������� �������
    unsigned char Silent[2];		// 2 ����� ��� �������������� ��������

    enum
    {
        STOPBITS_MASK = 0x08,
        PARITY_MASK   = 0x30,

        ONESTOPBITS	= 0x00,			// ���� ����-���
        TWOSTOPBITS	= 0x08,			// ��� ����-����
        NOPARITY	= 0x00,			// ��� ��������
        EVENPARITY	= 0x20,			// ������ �������
        ODDPARITY	= 0x30			// �������� ��������
    };
};

// ��������� ������� ModBus Master
struct TMBMRequest
{
    unsigned char  NetAddress;		// ������� ����� ����������
    unsigned char  Function;		// �������
    unsigned short Address;		// ����� ������
    unsigned char  Value;			// ��������
};

// ��������� ������ �������� ModBus Master
struct TMBMRequestList
{
    unsigned char Number;			// ����� ��������� ������
    TMBMRequest   Request[16];		// ������� ModBus Master
};

// ��������� �������� ������� ���������
#pragma bitfields = reversed
union TChannelSetting
{
    struct
    {
        unsigned char NoSound:	1;	// ������� ���������� �����
        unsigned char :		3;
        unsigned char Type:		4;	// ��� ����������� / ���������
    };

    unsigned char Byte;

    enum TYPE_INDICATION
    {
        NORMAL		 = 0,			// ��� �����������
        INDICATION	 = 1,			// ���������
        PREVENTIVE	 = 2,			// ����������������� ������������
        EMERGENCY	 = 3,			// ��������� ������������

        IND_PREVENTIVE = 4,			// ��������� + ����������������� ������������
        IND_EMERGENCY  = 5			// ��������� + ��������� ������������
    };
};

// ��������� �������� ������ ������ ������
#pragma bitfields = reversed
union TOutSetting
{
    struct
    {
        unsigned char :		2;
        unsigned char LocalReceipt:	1;	// ������������ � ������
        unsigned char LocalUnset:	1;	// ����� � ������
        unsigned char MBusReceipt:	1;	// ������������ � �������� ������
        unsigned char MBusUnset:	1;	// ����� � �������� ������
        unsigned char Mode:		2;	// ����� ������
    };

    unsigned char Byte;

    enum OUT_MODE
    {
        LIGHT_SIGNALING  = 0x00,		// �������� ������������
        SOUND_SIGNALING  = 0x02,		// �������� ������������
        CONTROL	   = 0x01,		// ����������� �����
        SELFHOLD_CONTROL = 0x03		// ����������� ����� � ���������������
    };

    operator unsigned char()		// �������� �������������� � unsigned char
    {
        return Byte;
    };
};

// ��������� ������ ������
struct TOutputData
{
    unsigned char DiscreteOutputModuleDataBase[DISCRETE_OUTPUT_NUMBER];		// ��������� ��-54
    unsigned char ExternalReceiptAddress[DISCRETE_OUTPUT_NUMBER];		// ������ ������ �������� ������������
    unsigned char ExternalResetAddress[DISCRETE_OUTPUT_NUMBER];		// ������ ������ �������� ������
};

struct TDiscreteOutputControl
{
    // ����� ���������.
    uint8_t uiNewActivation;
    // ���������� ��������� ����.
    uint8_t uiRelayActivationRequest;
};

struct TAlarmDfaInit
{
    // ������� ����������� ������� ���������������� ��� ��������.
    uint8_t uiActivityLevel;
    uint8_t uiAlarmType;
    uint8_t uiSavedAlarmType;
};

#pragma bitfields = default
struct TDeviceState
{
    // ������� ������ � ������ ������� ����� ����� ModBus.
    uint8_t ModbusRtuLinkControlInProgress:  1;
    uint8_t 	  : 7;

    uint8_t uiReserved[7];
};

//-----------------------------------------------------------------------------------------------------
// ��������� �������� �������� ������ � EEPROM
struct TDataBaseBlockPositionData
{
    unsigned char  Field;			// ������������� ���� ������
    unsigned char  Size;			// ������ �����
    unsigned short Offset;			// �������� ����� ������ �� ������
};

//-----------------------------------------------------------------------------------------------------
// �� �������� �������
#pragma pack(push)
#pragma pack(1)
struct TDataBase
{
    // ������������ ����������.
    TDevConfig	DevConfig;
    unsigned short CRC0;
    // ��������� ���������� ModBus.
    TMBusSetting   MBSet;
    unsigned short CRC1;
    // ������� ����������� ������� ���������������� ��� ��������.
    unsigned char  ActivityLevel[((MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER) / 8)];
    unsigned short CRC2;
    // ������ ���� ������������ ������� ��������� � �������� ����������� �������.
    unsigned char  AlarmWindowIndex[(MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER)];
    unsigned short CRC3;
    // ��� ������������ ����������� � ����.
    unsigned char  AlarmType[MAX_ALARM_WINDOWS_NUMBER];
    unsigned short CRC4;
    // �� ������� ������.
    TOutputData    OutConfig;
    unsigned short CRC5;
    // �������� ����, �������������� ���������� ������.
    unsigned char  Relay[(MAX_ALARM_WINDOWS_NUMBER + MAX_DISCRETE_INPUTS_NUMBER)];
    unsigned short CRC6;
    // ������ �������� ����� ������.
    TMBMRequestList InputList;
    unsigned short  CRC7;
    // ������ �������� ������������.
    TMBMRequestList ReceiptList;
    unsigned short  CRC8;
    // ������ �������� ������.
    TMBMRequestList UnsetList;
    unsigned short  CRC9;
    // ��������� ���������� �������� � EEPROM.
    TDeviceState DeviceState;
    unsigned short CRC10;

    enum DB_FIELDS
    {
        DEV_CONFIG	= 0,			// ������������ ����������
        MODBUS_SET	= 1,			// ��������� ���������� ModBus
        ACTIVITY_LEVEL	= 2,			// ��� �����
        ALARM_WINDOW_INDEX	= 3,			// �������������
        ALARM_TYPE	= 4,			// ���� ������������ �� �����
        OUT_BASE	= 5,			// �� ������� ������
        RELAY		= 6,			// �������� ����, �������������� ���������� ������
        INPUT_LIST	= 7,			// ������� ����� ������
        RECEIPT_LIST	= 8,			// ������� ������������
        UNSET_LIST	= 9,			// ������� ������
        DEVICE_STATE = 10,			// ��������� ���������� �������� � EEPROM.
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
    // ����������� ���������� �������.
    uint8_t uiModulesQuantity;
    TModuleContext axModulesContext[INTERNAL_MODULE_QUANTITY];
    uint8_t uiDiscreteOutputQuantity;
    uint8_t uiDiscreteInputQuantity;
//    uint8_t uiDisplayType;
//    uint8_t uiErrorCode;
};
#pragma pack(pop)

// ��� ������������, ������� ������ ���������� �� ����������� �������� ������.
#pragma pack(push)
#pragma pack(1)
struct TConfigDataPackOneOld
{
    // ����������� ���������� �������.
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

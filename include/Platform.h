//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef PLATFORM_H
#define PLATFORM_H

//-----------------------------------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

//-----------------------------------------------------------------------------------------------------
class CSerialPort
{

public:
    enum
    {
        UART_MAX_BUFF_LENGTH = 256,
        UART_INTERMEDIATE_BUFF_LENGTH = 16
    };

    CSerialPort();
    virtual ~CSerialPort();

//-----------------------------------------------------------------------------------------------------
    void SetPortName(const char* cPortName);
    const char* GetPortName(void);
    void SetBaudRate(uint32_t uiBaudRate);
    void SetDataBits(uint8_t uiDataBits);
    void SetParity(uint8_t uiParity);
    void SetStopBit(uint8_t uiStopBit);
    uint8_t Open(void);
    uint8_t Close(void);
    void Reset(void);
    int16_t Write(uint8_t * , uint16_t );
    int16_t Read(uint8_t * , uint16_t );
    void SetInterrupts(void);


//-----------------------------------------------------------------------------------------------------
private:
protected:

};
//-----------------------------------------------------------------------------------------------------













////-----------------------------------------------------------------------------------------------------
//class CMainCycle : public QObject
//{
//#define CDevice class CPssProgrammer
//
//    Q_OBJECT
//public:
//    CMainCycle(QObject *parent);
//    virtual ~CMainCycle();
//
//    void SetDevicePointer(CDevice* pxDevice)
//    {
//        m_pxDevice = pxDevice;
//    };
//
//    void SetSerialPortPointer(CSerialPort* pxSerialPort)
//    {
//        m_pxSerialPort = pxSerialPort;
//    };
//
//    CSerialPort* GetSerialPortPointer(void)
//    {
//        return m_pxSerialPort;
//    };
//
//    class CModbusRTU* GetModbusRTUPointer(void)
//    {
//        return m_pxModbusRtuOne;
//    };
//
//public slots:
//    void Process(void);
//    void ProcessSlot(void);
//
//signals:
//    void finished(void);
//    void error(QString err);
//
////public:
//private:
//    CSerialPort* m_pxSerialPort;
//    CDevice* m_pxDevice;
//    class CModbusRTU* m_pxModbusRtuOne;
//};
////-----------------------------------------------------------------------------------------------------






//-----------------------------------------------------------------------------------------------------
class CPlatform
{
public:
    CPlatform();
    virtual ~CPlatform();
    static void Init(void);
    static uint16_t GetCurrentTime(void);

private:

protected:
};

//-----------------------------------------------------------------------------------------------------
#endif // PLATFORM_H

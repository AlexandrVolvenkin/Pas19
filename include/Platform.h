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
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/select.h>

#include <unistd.h>

/* Include definition for RS485 ioctls: TIOCGRS485 and TIOCSRS485 */
#include <sys/ioctl.h>

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
    void Init(void);
    void SetPortName(const char* pccPortName);
    const char* GetPortName(void);
    void SetBaudRate(uint32_t uiBaudRate);
    void SetDataBits(uint8_t uiDataBits);
    void SetParity(char cParity);
    void SetStopBit(uint8_t uiStopBit);
    uint8_t Open(void);
    uint8_t Close(void);
//    void Reset(void);
//    bool IsDataAvailable(void);
    int16_t Write(uint8_t * , uint16_t );
    int16_t Read(uint8_t * , uint16_t );
    bool IsDataWrited(void)
    {
        if (m_bDataIsWrited)
        {
            m_bDataIsWrited = false;
            return true;
        }
        else
        {
            return false;
        }
    };


//-----------------------------------------------------------------------------------------------------
private:
protected:
    const char *m_pccPortName;
    /* Socket or file descriptor */
    int32_t m_iPortDescriptor;
    struct termios m_xTios;
    struct serial_rs485 m_xRs485Conf;
//    struct timeval *pto;
//    fd_set readfds, writefds;
//    uint32_t m_uiBaudRate;
//    /* Parity: 'N', 'O', 'E' */
//    char m_uiParity;
////    uint8_t m_uiParity;
//    uint8_t m_uiDataBits;
//    uint8_t m_uiStopBit;
    // Флаг - данные записаны.
    bool m_bDataIsWrited = false;
    uint8_t* m_puiTxBuffer;
    uint16_t m_nuiTxBuffByteCounter;
    uint8_t* m_puiRxBuffer;
    uint16_t m_nuiRxBuffByteCounter;
//    uint8_t m_auiIntermediateBuff[UART_INTERMEDIATE_BUFF_LENGTH];
};
//-----------------------------------------------------------------------------------------------------













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

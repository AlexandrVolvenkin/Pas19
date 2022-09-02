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
#include <iomanip>
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

# include <sys/socket.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <poll.h>
# include <netdb.h>

#include <unistd.h>

/* Include definition for RS485 ioctls: TIOCGRS485 and TIOCSRS485 */
#include <sys/ioctl.h>

//-----------------------------------------------------------------------------------------------------
class CCommunicationDevice
{
public:
    virtual void Init(void) = 0;

//    virtual void SetPortName(const char* pccPortName) = 0;
//    virtual const char* GetPortName(void) = 0;
//    virtual void SetBaudRate(uint32_t uiBaudRate) = 0;
//    virtual void SetDataBits(uint8_t uiDataBits) = 0;
//    virtual void SetParity(char cParity) = 0;
//    virtual void SetStopBit(uint8_t uiStopBit) = 0;

//    virtual void SetIpAddress(const char* pccIpAddress) = 0;
//    virtual const char* GetIpAddress(void) = 0;
//    virtual void SetPort(uint16_t uiPort) = 0;

    virtual int8_t Open(void) = 0;
    virtual int8_t Close(void) = 0;
    virtual int16_t Write(uint8_t * , uint16_t ) = 0;
    virtual int16_t Read(uint8_t * , uint16_t ) = 0;
    virtual bool IsDataWrited(void) = 0;
//    virtual bool IsDataWrited(void)
//    {
//        if (m_bDataIsWrited)
//        {
//            m_bDataIsWrited = false;
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    };


//-----------------------------------------------------------------------------------------------------
private:
protected:
    // Флаг - данные записаны.
    bool m_bDataIsWrited = false;
    uint8_t* m_puiTxBuffer;
    uint16_t m_nuiTxBuffByteCounter;
    uint8_t* m_puiRxBuffer;
    uint16_t m_nuiRxBuffByteCounter;
};
//-----------------------------------------------------------------------------------------------------














//-----------------------------------------------------------------------------------------------------
class CSerialPort : public CCommunicationDevice
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
    int8_t Open(void);
    int8_t Close(void);
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
    int32_t m_iDeviceDescriptor;
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
//    // Флаг - данные записаны.
//    bool m_bDataIsWrited = false;
//    uint8_t* m_puiTxBuffer;
//    uint16_t m_nuiTxBuffByteCounter;
//    uint8_t* m_puiRxBuffer;
//    uint16_t m_nuiRxBuffByteCounter;
////    uint8_t m_auiIntermediateBuff[UART_INTERMEDIATE_BUFF_LENGTH];
};
//-----------------------------------------------------------------------------------------------------














//-----------------------------------------------------------------------------------------------------
class CTcpCommunicationDevice : public CCommunicationDevice
{
public:
    enum
    {
        UART_MAX_BUFF_LENGTH = 256,
        UART_INTERMEDIATE_BUFF_LENGTH = 16
    };

    CTcpCommunicationDevice();
    virtual ~CTcpCommunicationDevice();

//-----------------------------------------------------------------------------------------------------
    void Init(void);
    void SetIpAddress(const char* pccIpAddress);
    const char* GetIpAddress(void);
    void SetPort(uint16_t uiPort);
    int8_t Listen(void);
    int8_t Accept(void);
    int8_t Connect(void);
    int8_t Open(void);
    int8_t Close(void);
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
    const char *m_pccIpAddress;
    uint32_t m_uiIpAddress;
    uint16_t m_uiPort;
    /* Socket or file descriptor */
    int32_t m_iDeviceDescriptor;
    int32_t m_iDeviceDescriptorAccept;
    struct sockaddr_in m_Address;
//    struct termios m_xTios;
//    struct serial_rs485 m_xRs485Conf;
//    struct timeval *pto;
//    fd_set readfds, writefds;
//    uint32_t m_uiBaudRate;
//    /* Parity: 'N', 'O', 'E' */
//    char m_uiParity;
////    uint8_t m_uiParity;
//    uint8_t m_uiDataBits;
//    uint8_t m_uiStopBit;
//    // Флаг - данные записаны.
//    bool m_bDataIsWrited = false;
//    uint8_t* m_puiTxBuffer;
//    uint16_t m_nuiTxBuffByteCounter;
//    uint8_t* m_puiRxBuffer;
//    uint16_t m_nuiRxBuffByteCounter;
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

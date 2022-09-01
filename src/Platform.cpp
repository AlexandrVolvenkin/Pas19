//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "Platform.h"

using namespace std;

//-----------------------------------------------------------------------------------------------------
CSerialPort::CSerialPort()
{

}

//-----------------------------------------------------------------------------------------------------
CSerialPort::~CSerialPort()
{

}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::Init(void)
{
    memset(&m_xTios, 0, sizeof(struct termios));

    /* C_CFLAG      Control options
       CLOCAL       Local line - do not change "owner" of port
       CREAD        Enable receiver
    */
    m_xTios.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */


    /* Read the man page of termios if you need more information. */

    /* This field isn't used on POSIX systems
       m_xTios.c_line = 0;
    */

    /* C_LFLAG      Line options

       ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
       ICANON       Enable canonical input (else raw)
       XCASE        Map uppercase \lowercase (obsolete)
       ECHO Enable echoing of input characters
       ECHOE        Echo erase character as BS-SP-BS
       ECHOK        Echo NL after kill character
       ECHONL       Echo NL
       NOFLSH       Disable flushing of input buffers after
       interrupt or quit characters
       IEXTEN       Enable extended functions
       ECHOCTL      Echo control characters as ^char and delete as ~?
       ECHOPRT      Echo erased character as character erased
       ECHOKE       BS-SP-BS entire line on line kill
       FLUSHO       Output being flushed
       PENDIN       Retype pending input at next read or input char
       TOSTOP       Send SIGTTOU for background output

       Canonical input is line-oriented. Input characters are put
       into a buffer which can be edited interactively by the user
       until a CR (carriage return) or LF (line feed) character is
       received.

       Raw input is unprocessed. Input characters are passed
       through exactly as they are received, when they are
       received. Generally you'll deselect the ICANON, ECHO,
       ECHOE, and ISIG options when using raw input
    */

    /* Raw input */
    m_xTios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* Software flow control is disabled */
    m_xTios.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* C_OFLAG      Output options
       OPOST        Postprocess output (not set = raw output)
       ONLCR        Map NL to CR-NL

       ONCLR ant others needs OPOST to be enabled
    */

    /* Raw ouput */
    m_xTios.c_oflag &=~ OPOST;

    /* C_CC         Control characters
       VMIN         Minimum number of characters to read
       VTIME        Time to wait for data (tenths of seconds)

       UNIX serial interface drivers provide the ability to
       specify character and packet timeouts. Two elements of the
       c_cc array are used for timeouts: VMIN and VTIME. Timeouts
       are ignored in canonical input mode or when the NDELAY
       option is set on the file via open or fcntl.

       VMIN specifies the minimum number of characters to read. If
       it is set to 0, then the VTIME value specifies the time to
       wait for every character read. Note that this does not mean
       that a read call for N bytes will wait for N characters to
       come in. Rather, the timeout will apply to the first
       character and the read call will return the number of
       characters immediately available (up to the number you
       request).

       If VMIN is non-zero, VTIME specifies the time to wait for
       the first character read. If a character is read within the
       time given, any read will block (wait) until all VMIN
       characters are read. That is, once the first character is
       read, the serial interface driver expects to receive an
       entire packet of characters (VMIN bytes total). If no
       character is read within the time allowed, then the call to
       read returns 0. This method allows you to tell the serial
       driver you need exactly N bytes and any read call will
       return 0 or N bytes. However, the timeout only applies to
       the first character read, so if for some reason the driver
       misses one character inside the N byte packet then the read
       call could block forever waiting for additional input
       characters.

       VTIME specifies the amount of time to wait for incoming
       characters in tenths of seconds. If VTIME is set to 0 (the
       default), reads will block (wait) indefinitely unless the
       NDELAY option is set on the port with open or fcntl.
    */
    /* Unused because we use open with the NDELAY option */
    m_xTios.c_cc[VMIN] = 0;
    m_xTios.c_cc[VTIME] = 0;


    memset(&m_xRs485Conf, 0x0, sizeof(struct serial_rs485));

    m_xRs485Conf.flags |= SER_RS485_ENABLED;
    m_xRs485Conf.flags |= SER_RS485_RTS_ON_SEND;
    //m_xRs485Conf.flags &= ~(SER_RS485_RTS_ON_SEND);
    m_xRs485Conf.flags &= ~SER_RS485_RTS_AFTER_SEND;
    //m_xRs485Conf.flags |= SER_RS485_RTS_AFTER_SEND;
    m_xRs485Conf.delay_rts_before_send = 0;
    m_xRs485Conf.delay_rts_after_send = 0;
}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::SetPortName(const char* pccPortName)
{
    m_pccPortName = pccPortName;
}

//-----------------------------------------------------------------------------------------------------
const char* CSerialPort::GetPortName(void)
{
    return m_pccPortName;
}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::SetBaudRate(uint32_t uiBaudRate)
{
    speed_t speed;

    /* C_ISPEED     Input baud (new interface)
       C_OSPEED     Output baud (new interface)
    */
    switch (uiBaudRate)
    {
    case 110:
        speed = B110;
        break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    default:
        speed = B9600;
        break;
    }

    /* Set the baud rate */
    if ((cfsetispeed(&m_xTios, speed) < 0) ||
            (cfsetospeed(&m_xTios, speed) < 0))
    {
//        close(m_iPortDescriptor);
//        m_iPortDescriptor = -1;
//        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::SetDataBits(uint8_t uiDataBits)
{
    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    m_xTios.c_cflag &= ~CSIZE;
    switch (uiDataBits)
    {
    case 5:
        m_xTios.c_cflag |= CS5;
        break;
    case 6:
        m_xTios.c_cflag |= CS6;
        break;
    case 7:
        m_xTios.c_cflag |= CS7;
        break;
    case 8:
    default:
        m_xTios.c_cflag |= CS8;
        break;
    }
}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::SetParity(char cParity)
{
    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (cParity == 'N')
    {
        /* None */
        m_xTios.c_cflag &=~ PARENB;
    }
    else if (cParity == 'E')
    {
        /* Even */
        m_xTios.c_cflag |= PARENB;
        m_xTios.c_cflag &=~ PARODD;
    }
    else
    {
        /* Odd */
        m_xTios.c_cflag |= PARENB;
        m_xTios.c_cflag |= PARODD;
    }

    /* C_IFLAG      Input options

       Constant     Description
       INPCK        Enable parity check
       IGNPAR       Ignore parity errors
       PARMRK       Mark parity errors
       ISTRIP       Strip parity bits
       IXON Enable software flow control (outgoing)
       IXOFF        Enable software flow control (incoming)
       IXANY        Allow any character to start flow again
       IGNBRK       Ignore break condition
       BRKINT       Send a SIGINT when a break condition is detected
       INLCR        Map NL to CR
       IGNCR        Ignore CR
       ICRNL        Map CR to NL
       IUCLC        Map uppercase to lowercase
       IMAXBEL      Echo BEL on input line too long
    */
    if (cParity == 'N')
    {
        /* None */
        m_xTios.c_iflag &= ~INPCK;
    }
    else
    {
        m_xTios.c_iflag |= INPCK;
    }
}

//-----------------------------------------------------------------------------------------------------
void CSerialPort::SetStopBit(uint8_t uiStopBit)
{
    /* Stop bit (1 or 2) */
    if (uiStopBit == 1)
    {
        m_xTios.c_cflag &=~ CSTOPB;
    }
    else /* 2 */
    {
        m_xTios.c_cflag |= CSTOPB;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CSerialPort::Open(void)
{
    /* The O_NOCTTY flag tells UNIX that this program doesn't want
       to be the "controlling terminal" for that port. If you
       don't specify this then any input (such as keyboard abort
       signals and so forth) will affect your process

       Timeouts are ignored in canonical input mode or when the
       NDELAY option is set on the file via open or fcntl */
    m_iPortDescriptor = open(m_pccPortName, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (m_iPortDescriptor == -1)
    {
        fprintf(stderr, "ERROR Can't open the device %s (%s)\n",
                m_pccPortName, strerror(errno));
        return -1;
    }

    if (tcsetattr(m_iPortDescriptor, TCSANOW, &m_xTios) < 0)
    {
        close(m_iPortDescriptor);
        m_iPortDescriptor = -1;
        return -1;
    }

    if (ioctl(m_iPortDescriptor, TIOCSRS485, &m_xRs485Conf) < 0)
    {
        printf("Error! set rs485 ioctl: %d %s\n", errno, strerror(errno));
        return -1;
    }

    int flags = fcntl(m_iPortDescriptor, F_GETFL, 0);
    fcntl(m_iPortDescriptor, F_SETFL, flags | O_NONBLOCK);

//    pto = NULL;
//    FD_ZERO(&readfds);
//    FD_ZERO(&writefds);
//    FD_SET(m_iPortDescriptor, &readfds);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CSerialPort::Close(void)
{
    close(m_iPortDescriptor);
}

////-----------------------------------------------------------------------------------------------------
//void CSerialPort::Reset(void)
//{
//
//}

////-----------------------------------------------------------------------------------------------------
//bool CSerialPort::IsDataAvailable(void)
//{
//        int ready;
//        ready = select((m_iPortDescriptor + 1), &readfds, &writefds, NULL, pto);
//
//        if (ready == -1)
//        {
//            return false;
//        }
//        else
//        {
//            return true;
//        }
//}

//-----------------------------------------------------------------------------------------------------
int16_t CSerialPort::Read(uint8_t *puiDestination, uint16_t uiLength)
{
    return read(m_iPortDescriptor, puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int16_t CSerialPort::Write(uint8_t *puiSource, uint16_t uiLength)
{
    m_bDataIsWrited = true;
    return write(m_iPortDescriptor, puiSource, uiLength);
}

//-----------------------------------------------------------------------------------------------------












//-----------------------------------------------------------------------------------------------------
CPlatform::CPlatform()
{

}

//-----------------------------------------------------------------------------------------------------
CPlatform::~CPlatform()
{

}

//-----------------------------------------------------------------------------------------------------
void CPlatform::Init(void)
{

}

//-----------------------------------------------------------------------------------------------------
uint16_t CPlatform::GetCurrentTime(void)
{
    struct timeval xCurrentTime;

    gettimeofday( &xCurrentTime, NULL );

    return static_cast<uint16_t>(((xCurrentTime.tv_sec * 1000) +
                                  (xCurrentTime.tv_usec / 1000)));
}

//-----------------------------------------------------------------------------------------------------

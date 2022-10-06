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
#include "Modbus.h"

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
void CSerialPort::SetPortName(const char* pccDeviceName)
{
    m_pccDeviceName = pccDeviceName;
}

//-----------------------------------------------------------------------------------------------------
const char* CSerialPort::GetPortName(void)
{
    return m_pccDeviceName;
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
//        close(m_iDeviceDescriptor);
//        m_iDeviceDescriptor = -1;
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
int8_t CSerialPort::Open(void)
{
    /* The O_NOCTTY flag tells UNIX that this program doesn't want
       to be the "controlling terminal" for that port. If you
       don't specify this then any input (such as keyboard abort
       signals and so forth) will affect your process

       Timeouts are ignored in canonical input mode or when the
       NDELAY option is set on the file via open or fcntl */
    m_iDeviceDescriptor = open(m_pccDeviceName, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (m_iDeviceDescriptor == -1)
    {
        fprintf(stderr, "ERROR Can't open the device %s (%s)\n",
                m_pccDeviceName, strerror(errno));
        return -1;
    }

    if (tcsetattr(m_iDeviceDescriptor, TCSANOW, &m_xTios) < 0)
    {
        close(m_iDeviceDescriptor);
        m_iDeviceDescriptor = -1;
        return -1;
    }

    if (ioctl(m_iDeviceDescriptor, TIOCSRS485, &m_xRs485Conf) < 0)
    {
        printf("Error! set rs485 ioctl: %d %s\n", errno, strerror(errno));
        return -1;
    }

    // Сделаем не блокирующим.
    int flags = fcntl(m_iDeviceDescriptor, F_GETFL, 0);
    fcntl(m_iDeviceDescriptor, F_SETFL, flags | O_NONBLOCK);

//    pto = NULL;
//    FD_ZERO(&readfds);
//    FD_ZERO(&writefds);
//    FD_SET(m_iDeviceDescriptor, &readfds);
}

//-----------------------------------------------------------------------------------------------------
int8_t CSerialPort::Close(void)
{
    close(m_iDeviceDescriptor);
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
//        ready = select((m_iDeviceDescriptor + 1), &readfds, &writefds, NULL, pto);
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
    return read(m_iDeviceDescriptor, puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int16_t CSerialPort::Write(uint8_t *puiSource, uint16_t uiLength)
{
    m_bDataIsWrited = true;
    return write(m_iDeviceDescriptor, puiSource, uiLength);
}

//-----------------------------------------------------------------------------------------------------
// производит обмен данными по SPI.
int CSerialPort::Exchange(uint8_t uiAddress,
                          unsigned char *pucTxBuff,
                          unsigned char *pucRxBuff,
                          int iLength,
                          int iSpeed)
{

}

//-----------------------------------------------------------------------------------------------------













//-----------------------------------------------------------------------------------------------------
CTcpCommunicationDevice::CTcpCommunicationDevice()
{

}

//-----------------------------------------------------------------------------------------------------
CTcpCommunicationDevice::~CTcpCommunicationDevice()
{

}

//-----------------------------------------------------------------------------------------------------
void CTcpCommunicationDevice::Init(void)
{
#if defined(OS_BSD)
    /* MSG_NOSIGNAL is unsupported on *BSD so we install an ignore
       handler for SIGPIPE. */
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) < 0)
    {
        /* The debug flag can't be set here... */
        fprintf(stderr, "Coud not install SIGPIPE handler.\n");
        return NULL;
    }
#endif

    memset(&m_Address, 0, sizeof(m_Address));
    m_Address.sin_family = AF_INET;
}

//-----------------------------------------------------------------------------------------------------
void CTcpCommunicationDevice::SetIpAddress(const char* pccIpAddress)
{
    m_uiIpAddress = inet_addr(pccIpAddress);
}

//-----------------------------------------------------------------------------------------------------
const char* CTcpCommunicationDevice::GetIpAddress(void)
{

}

//-----------------------------------------------------------------------------------------------------
void CTcpCommunicationDevice::SetPort(uint16_t uiPort)
{
    m_uiPort = uiPort;
}

//-----------------------------------------------------------------------------------------------------
int8_t CTcpCommunicationDevice::Open(void)
{

//    int sock, listener;
//    struct sockaddr_in addr;
//    char buf[1024];
//    int bytes_read;
//
//    listener = socket(AF_INET, SOCK_STREAM, 0);
//    if(listener < 0)
//    {
//        perror("socket");
//        exit(1);
//    }
//
//    int yes;
//    yes = 1;
//    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
//                   (char *) &yes, sizeof(yes)) == -1)
//    {
//        cout << "CTcpCommunicationDevice::Open error 8" << endl;
//        close(listener);
//        return -1;
//    }
//
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(5);
//    addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
//    {
//        perror("bind");
//        exit(2);
//    }
//
//        cout << "bind ok" << endl;
//    return 0;


    /* Establishes a modbus TCP connection with a Modbus server. */
    int rc;

    m_iDeviceDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    if (m_iDeviceDescriptor == -1)
    {
        cout << "CTcpCommunicationDevice::Open error" << endl;
        return -1;
    }

    cout << "CTcpCommunicationDevice::Open ok" << endl;

    int option;

    /* Set the TCP no delay flag */
    /* SOL_TCP = IPPROTO_TCP */
    option = 1;
    rc = setsockopt(m_iDeviceDescriptor, IPPROTO_TCP, TCP_NODELAY,
                    (const void *)&option, sizeof(int));
    if (rc == -1)
    {
        cout << "CTcpCommunicationDevice::Open error 2" << endl;
        return -1;
    }

    rc = connect(m_iDeviceDescriptor, (struct sockaddr *)&m_Address,
                 sizeof(struct sockaddr_in));
    if (rc == -1)
    {
        fprintf(stderr, "Connection failed tcp: %s\n",
                CModbus::ModbusStringError(errno));
        cout << "CTcpCommunicationDevice::Open error 3" << endl;
        close(m_iDeviceDescriptor);
//        return -1;
    }



    int new_socket;
    int yes;
    struct sockaddr_in addr;

    new_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (new_socket == -1)
    {
        cout << "CTcpCommunicationDevice::Open error 4" << endl;
        return -1;
    }

    yes = 1;
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &yes, sizeof(yes)) == -1)
    {
        cout << "CTcpCommunicationDevice::Open error 8" << endl;
        close(new_socket);
        return -1;
    }



    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* If the modbus port is < to 1024, we need the setuid root. */
//    addr.sin_port = m_Address.sin_port;
    addr.sin_port = htons(502);
    addr.sin_addr.s_addr = INADDR_ANY;//htonl(INADDR_ANY);//

    cout << "CStorageDeviceFileSystem::Write pccIpAddress" << " " << addr.sin_addr.s_addr << endl;
    cout << "CStorageDeviceFileSystem::Write sin_port" << " " << (int)addr.sin_port << endl;
    cout << "CStorageDeviceFileSystem::Write sin_port" << " " << (int)htons(502) << endl;

    if (bind(new_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "Connection failed tcp bind: %s\n",
                CModbus::ModbusStringError(errno));
        cout << "CTcpCommunicationDevice::Open error 5" << endl;
        close(new_socket);
        return -1;
    }

    int nb_connection = 1;
    if (listen(new_socket, nb_connection) == -1)
    {
        cout << "CTcpCommunicationDevice::Open error 6" << endl;
        close(new_socket);
        return -1;
    }



//    struct sockaddr_in addr;
    socklen_t addrlen;

    addrlen = sizeof(addr);
    m_iDeviceDescriptor = accept(new_socket, (struct sockaddr *)&addr, &addrlen);
    if (m_iDeviceDescriptor == -1)
    {
        cout << "CTcpCommunicationDevice::Open error 7" << endl;
        close(new_socket);
        return -1;
    }

//    if (ctx->debug)
//    {
    printf("The client connection from %s is accepted\n",
           inet_ntoa(m_Address.sin_addr));
//    }



    int flags = fcntl(m_iDeviceDescriptor, F_GETFL, 0);
    fcntl(m_iDeviceDescriptor, F_SETFL, flags | O_NONBLOCK);


    return 0;
}

//-----------------------------------------------------------------------------------------------------
int8_t CTcpCommunicationDevice::Listen(void)
{
    int yes;
    struct sockaddr_in addr;

    m_iDeviceDescriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_iDeviceDescriptor == -1)
    {
        return -1;
    }

    yes = 1;
    if (setsockopt(m_iDeviceDescriptor, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &yes, sizeof(yes)) == -1)
    {
        close(m_iDeviceDescriptor);
        return -1;
    }



    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* If the modbus port is < to 1024, we need the setuid root. */
    addr.sin_port = htons(m_uiPort);
    addr.sin_addr.s_addr = INADDR_ANY;//htonl(INADDR_ANY);//

    if (bind(m_iDeviceDescriptor, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr, "Connection failed tcp bind: %s\n",
                CModbus::ModbusStringError(errno));
        close(m_iDeviceDescriptor);
        return -1;
    }

    int nb_connection = 1;
    if (listen(m_iDeviceDescriptor, nb_connection) == -1)
    {
        close(m_iDeviceDescriptor);
        return -1;
    }

//    // Сделаем не блокирующим.
//    int flags = fcntl(m_iDeviceDescriptor, F_GETFL, 0);
//    fcntl(m_iDeviceDescriptor, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

//-----------------------------------------------------------------------------------------------------
int8_t CTcpCommunicationDevice::Accept(void)
{
//    // Сделаем не блокирующим.
//    int flags = fcntl(m_iDeviceDescriptor, F_GETFL, 0);
//    fcntl(m_iDeviceDescriptor, F_SETFL, flags | O_NONBLOCK);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
//    FD_ZERO(&writefds);
    FD_SET(m_iDeviceDescriptor, &readfds);
//    FD_SET(m_iDeviceDescriptor, &writefds);
    FD_SET(STDIN_FILENO, &readfds);
//    FD_SET(STDIN_FILENO, &writefds);

    int ready = select(m_iDeviceDescriptor + 1, &readfds, NULL, NULL, NULL);

    if (ready == -1)
    {
        return 0;
    }
    else if (FD_ISSET(m_iDeviceDescriptor, &readfds))
    {
        struct sockaddr_in addr;
        socklen_t addrlen;

        addrlen = sizeof(addr);
        m_iDeviceDescriptorAccept = accept(m_iDeviceDescriptor, (struct sockaddr *)&addr, &addrlen);
        if (m_iDeviceDescriptorAccept == -1)
        {
            fprintf(stderr, "Connection failed tcp bind: %s\n",
                    CModbus::ModbusStringError(errno));
            close(m_iDeviceDescriptor);
            return 0;
        }

        printf("The client connection from %s is accepted\n",
               inet_ntoa(addr.sin_addr));

        // Сделаем не блокирующим.
        int flags = fcntl(m_iDeviceDescriptorAccept, F_GETFL, 0);
        fcntl(m_iDeviceDescriptorAccept, F_SETFL, flags | O_NONBLOCK);

        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
int8_t CTcpCommunicationDevice::Connect(void)
{
    /* Establishes a modbus TCP connection with a Modbus server. */
    int rc;

    m_iDeviceDescriptorAccept = socket(PF_INET, SOCK_STREAM, 0);
    if (m_iDeviceDescriptorAccept == -1)
    {
        return -1;
    }

    int option;

    /* Set the TCP no delay flag */
    /* SOL_TCP = IPPROTO_TCP */
    option = 1;
    rc = setsockopt(m_iDeviceDescriptorAccept, IPPROTO_TCP, TCP_NODELAY,
                    (const void *)&option, sizeof(int));
    if (rc == -1)
    {
        return -1;
    }

    rc = connect(m_iDeviceDescriptorAccept, (struct sockaddr *)&m_Address,
                 sizeof(struct sockaddr_in));
    if (rc == -1)
    {
        fprintf(stderr, "Connection failed tcp: %s\n",
                CModbus::ModbusStringError(errno));
        cout << "Connect connect error" << endl;
        close(m_iDeviceDescriptorAccept);
        return -1;
    }

    // Сделаем не блокирующим.
    int flags = fcntl(m_iDeviceDescriptorAccept, F_GETFL, 0);
    fcntl(m_iDeviceDescriptorAccept, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

////-----------------------------------------------------------------------------------------------------
//bool CTcpCommunicationDevice::IsDataAvailable(void)
//{
//    int ready;
//    ready = select((m_iDeviceDescriptor + 1), &readfds, &writefds, NULL, pto);
//
//    if (ready == -1)
//    {
//        return false;
//    }
//    else
//    {
//        return true;
//    }
//}

//-----------------------------------------------------------------------------------------------------
int8_t CTcpCommunicationDevice::Close(void)
{
    close(m_iDeviceDescriptor);
    close(m_iDeviceDescriptorAccept);
}

//-----------------------------------------------------------------------------------------------------
int16_t CTcpCommunicationDevice::Read(uint8_t *puiDestination, uint16_t uiLength)
{
    return read(m_iDeviceDescriptorAccept, puiDestination, uiLength);
}

//-----------------------------------------------------------------------------------------------------
int16_t CTcpCommunicationDevice::Write(uint8_t *puiSource, uint16_t uiLength)
{
    m_bDataIsWrited = true;
    return write(m_iDeviceDescriptorAccept, puiSource, uiLength);
}

//-----------------------------------------------------------------------------------------------------
// производит обмен данными по SPI.
int CTcpCommunicationDevice::Exchange(uint8_t uiAddress,
                                      unsigned char *pucTxBuff,
                                      unsigned char *pucRxBuff,
                                      int iLength,
                                      int iSpeed)
{

}

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CSpi::CSpi()
{

}

//-----------------------------------------------------------------------------------------------------
CSpi::~CSpi()
{

}

//-----------------------------------------------------------------------------------------------------
void CSpi::Init(void)
{
};

//-----------------------------------------------------------------------------------------------------
int8_t CSpi::Open(void)
{
    int iBits = BITS_PER_WORD;
    int  iMode = SPI_MODE;             //8-bits per word, SPI mode 3
//    ULONG  ulMode32 = SPI_MODE32;
    unsigned long ulSpeed;
    unsigned int nuiBusyTimeCounter;

    ulSpeed = SPEED_IN_HZ;               //Speed is 1 MHz

    // The following calls set up the SPI bus properties
    nuiBusyTimeCounter = 20;
    while (1)
    {
        if ((m_iDeviceDescriptor = open(SPI_PATH, O_RDWR | O_SYNC)) < 0)
        {
            perror("SPI: Can't open device.");
            CGpio::SetPin(PRD_EN_PIN_PORT, PRD_EN_PIN);
            usleep(500000);
            CGpio::ClearPin(PRD_EN_PIN_PORT, PRD_EN_PIN);
            usleep(500000);
            if (!nuiBusyTimeCounter--)
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }

    if (ioctl(m_iDeviceDescriptor, SPI_IOC_WR_MODE, &iMode) == -1)
    {
        perror("SPI: Can't set SPI mode.");
        return -1;
    }
    if (ioctl(m_iDeviceDescriptor, SPI_IOC_RD_MODE, &iMode) == -1)
    {
        perror("SPI: Can't get SPI mode.");
        return -1;
    }
//    if (ioctl(m_iDeviceDescriptor, SPI_IOC_WR_MODE32, &ulMode32) == -1)
//    {
//        perror("SPI: Can't set SPI mode32.");
//        return -1;
//    }
//    if (ioctl(m_iDeviceDescriptor, SPI_IOC_RD_MODE32, &ulMode32) == -1)
//    {
//        perror("SPI: Can't get SPI mode32.");
//        return -1;
//    }
    if (ioctl(m_iDeviceDescriptor, SPI_IOC_WR_BITS_PER_WORD, &iBits) == -1)
    {
        perror("SPI: Can't set bits per word.");
        return -1;
    }
    if (ioctl(m_iDeviceDescriptor, SPI_IOC_RD_BITS_PER_WORD, &iBits) == -1)
    {
        perror("SPI: Can't get bits per word.");
        return -1;
    }
    if (ioctl(m_iDeviceDescriptor, SPI_IOC_WR_MAX_SPEED_HZ, &ulSpeed) == -1)
    {
        perror("SPI: Can't set max speed HZ");
        return -1;
    }
    if (ioctl(m_iDeviceDescriptor, SPI_IOC_RD_MAX_SPEED_HZ, &ulSpeed) == -1)
    {
        perror("SPI: Can't get max speed HZ.");
        return -1;
    }

    // Check that the properties have been set
    printf("SPI Mode is: %d\n", iMode);
    printf("SPI Bits is: %d\n", iBits);
    printf("SPI Speed is: %d\n", (int)ulSpeed);
    return 0;
}

//-----------------------------------------------------------------------------------------------------
int8_t CSpi::Close(void)
{
    close(m_iDeviceDescriptor);
}

//-----------------------------------------------------------------------------------------------------
void CSpi::ChipSelectPinSet(void)
{
    CGpio::SetPinOutput(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    CGpio::SetPinOutput(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    CGpio::SetPinOutput(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    CGpio::SetPinOutput(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    CGpio::SetPinOutput(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::SetPinOutput(SPI_CHIP_MIND_ENABLE_PIN_PORT, SPI_CHIP_MIND_ENABLE_PIN);

    CGpio::SetPin(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    CGpio::SetPin(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    CGpio::SetPin(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    CGpio::SetPin(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    CGpio::ClearPin(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::SetPin(SPI_CHIP_MIND_ENABLE_PIN_PORT, SPI_CHIP_MIND_ENABLE_PIN);
}

//-----------------------------------------------------------------------------------------------------
void CSpi::ChipSelectPinDelete(void)
{
    CGpio::SetPinInput(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    CGpio::SetPinInput(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    CGpio::SetPinInput(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    CGpio::SetPinInput(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    CGpio::SetPinInput(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::SetPinInput(SPI_CHIP_MIND_ENABLE_PIN_PORT, SPI_CHIP_MIND_ENABLE_PIN);

    CGpio::ClearPin(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    CGpio::ClearPin(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    CGpio::ClearPin(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    CGpio::ClearPin(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    CGpio::ClearPin(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::ClearPin(SPI_CHIP_MIND_ENABLE_PIN_PORT, SPI_CHIP_MIND_ENABLE_PIN);
}

//-----------------------------------------------------------------------------------------------------
// массив сопоставления номера посадочного слота модуля с адресом на шине SPI.
const uint8_t CSpi::aui8ModuleSlotNumberToSpiAddressMatching[] =
{
    0x0F,
    0x07,
    0x0B,
    0x03,
    0x0D,
    0x05,
    0x09,
    0x01,
    0x0E,
    0x06,
    0x0A,
    0x02,
    0x0C,
    0x04,
    0x08,
    0x00
};

//-----------------------------------------------------------------------------------------------------
// устанавливает адрес на шине SPI. зажигает и гасит светодиод контроля обмена данными.
void CSpi::ChipSelectAddressSet(unsigned char ucAddress)
{
    unsigned char ucAddressLocal;

    CGpio::ClearPin(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::SetPin(PRD_EN_PIN_PORT, PRD_EN_PIN);

    ucAddressLocal = aui8ModuleSlotNumberToSpiAddressMatching[ucAddress];

    if (ucAddressLocal & 0x01)
    {
        CGpio::SetPin(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    }
    else
    {
        CGpio::ClearPin(SPI_CHIP_SELECT_PIN_0_PORT, SPI_CHIP_SELECT_PIN_0);
    }

    ucAddressLocal = (ucAddressLocal >> 1);
    if (ucAddressLocal & 0x01)
    {
        CGpio::SetPin(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    }
    else
    {
        CGpio::ClearPin(SPI_CHIP_SELECT_PIN_1_PORT, SPI_CHIP_SELECT_PIN_1);
    }

    ucAddressLocal = (ucAddressLocal >> 1);
    if (ucAddressLocal & 0x01)
    {
        CGpio::SetPin(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    }
    else
    {
        CGpio::ClearPin(SPI_CHIP_SELECT_PIN_2_PORT, SPI_CHIP_SELECT_PIN_2);
    }

    ucAddressLocal = (ucAddressLocal >> 1);
    if (ucAddressLocal & 0x01)
    {
        CGpio::SetPin(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    }
    else
    {
        CGpio::ClearPin(SPI_CHIP_SELECT_PIN_3_PORT, SPI_CHIP_SELECT_PIN_3);
    }

    usleep(100);
    CGpio::SetPin(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);
    CGpio::ClearPin(PRD_EN_PIN_PORT, PRD_EN_PIN);
    usleep(380);
}

//-----------------------------------------------------------------------------------------------------
// производит обмен данными по SPI.
int CSpi::Exchange(uint8_t uiAddress,
                   unsigned char *pucTxBuff,
                   unsigned char *pucRxBuff,
                   int iLength,
                   int iSpeed)
{
    ChipSelectAddressSet(uiAddress);
//    memset(aucSpiTxBuffer, 0, iLength);
//    memset(aucSpiRxBuffer, 0, iLength);
//    memset(aucSpiRxBuffer, 0, 4);

    struct spi_ioc_transfer xTransfer =
    {
        //the xTransfer structure
        (unsigned long) pucTxBuff,	    // tx_buf;        //the buffer for sending data
        (unsigned long) pucRxBuff,    // rx_buf;         //the buffer for receiving data
        (unsigned long) iLength,      // len;           //the length of buffer
        (unsigned long) iSpeed,       // speed_hz;      //the speed in Hz
        (unsigned int) 0,             // delay_usecs;   //delay in us
        (unsigned char) 8             // bits_per_word; //bits per word
        //(unsigned char)               // cs_change;
        //(unsigned char)               // tx_nbits;
        //(unsigned char)               // rx_nbits;
        //(unsigned int)                // pad;

    };
    // send the SPI message (all of the above fields, inc. buffers)
    int iStatus = ioctl(m_iDeviceDescriptor, SPI_IOC_MESSAGE(1), &xTransfer);

    CGpio::ClearPin(SPI_CHIP_ENABLE_PIN_PORT, SPI_CHIP_ENABLE_PIN);

    if (iStatus < 0)
    {
        perror("SPI: SPI_IOC_MESSAGE Failed");
        return -1;
    }
    return iStatus;
}

//-----------------------------------------------------------------------------------------------------








////-----------------------------------------------------------------------------------------------------
int32_t CGpio::m_iDeviceDescriptor;
TGpioControl CGpio::m_xGpioControl;

////-----------------------------------------------------------------------------------------------------
//CGpio::CGpio()
//{
//
//}
//
////-----------------------------------------------------------------------------------------------------
//CGpio::~CGpio()
//{
//
//}

//-----------------------------------------------------------------------------------------------------
void CGpio::Init(void)
{
};

//-----------------------------------------------------------------------------------------------------
int8_t CGpio::Open(void)
{
    m_iDeviceDescriptor = open("/dev/mem", O_RDWR | O_SYNC);
    if(m_iDeviceDescriptor < 0)
    {
        printf("%s\n", strerror(errno));
        close(m_iDeviceDescriptor);
        return -1;
    }

    m_xGpioControl.aulGpioMmapAddresses[GPIO_PORT_0] = GPIO0_ADDR;
    m_xGpioControl.aulGpioMmapAddresses[GPIO_PORT_1] = GPIO1_ADDR;
    m_xGpioControl.aulGpioMmapAddresses[GPIO_PORT_2] = GPIO2_ADDR;
    m_xGpioControl.aulGpioMmapAddresses[GPIO_PORT_3] = GPIO3_ADDR;

    int i;
    for(i = 0; i < GPIO_PORT_QUANTITY; i++)
    {
        m_xGpioControl.apulGpioMmapPointers[i] =  (ulong*) mmap(NULL,
                0x1000,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                m_iDeviceDescriptor,
                m_xGpioControl.aulGpioMmapAddresses[i]);
    }

// GPIO clock configuration
    m_xGpioControl.pulCmPerMap =  (ulong*) mmap(NULL,
                                  0x400,
                                  PROT_READ | PROT_WRITE,
                                  MAP_SHARED,
                                  m_iDeviceDescriptor,
                                  CM_PER);

//    *(m_xGpioControl.pulCmPerMap + (CM_PER_L4LS_CLKSTCTRL_OFFSET / 4)) |= (1 << CLKACTIVITY_GPIO_1_GDBCLK) |
//            (1 << CLKACTIVITY_GPIO_2_GDBCLK) |
//            (1 << CLKACTIVITY_GPIO_3_GDBCLK);
//
    *(m_xGpioControl.pulCmPerMap + (CM_PER_GPIO1_CLKCTRL_OFFSET / 4)) = (1 << OPTFCLKEN_GPIO_X_GDBCLK) | (1 << MODULEMODE_BIT_1);
    *(m_xGpioControl.pulCmPerMap + (CM_PER_GPIO2_CLKCTRL_OFFSET / 4)) = (1 << OPTFCLKEN_GPIO_X_GDBCLK) | (1 << MODULEMODE_BIT_1);
    *(m_xGpioControl.pulCmPerMap + (CM_PER_GPIO3_CLKCTRL_OFFSET / 4)) = (1 << OPTFCLKEN_GPIO_X_GDBCLK) | (1 << MODULEMODE_BIT_1);

    return 0;
}

//-----------------------------------------------------------------------------------------------------
int8_t CGpio::Close(void)
{
    close(m_iDeviceDescriptor);
}

//-----------------------------------------------------------------------------------------------------
void CGpio::SetPinOutput(unsigned char ucPortN, unsigned char ucPinN)
{
    *(m_xGpioControl.apulGpioMmapPointers[ucPortN] + (GPIO_OE_OFFSET / 4)) &= ~(1 << ucPinN);
}

//-----------------------------------------------------------------------------------------------------
void CGpio::SetPinInput(unsigned char ucPortN, unsigned char ucPinN)
{
    *(m_xGpioControl.apulGpioMmapPointers[ucPortN] + (GPIO_OE_OFFSET / 4)) |= (1 << ucPinN);
}

//-----------------------------------------------------------------------------------------------------
void CGpio::SetPin(unsigned char ucPortN, unsigned char ucPinN)
{
    *(m_xGpioControl.apulGpioMmapPointers[ucPortN] + (GPIO_DATAOUT_OFFSET / 4)) |= (1 << ucPinN);
}

//-----------------------------------------------------------------------------------------------------
void CGpio::ClearPin(unsigned char ucPortN, unsigned char ucPinN)
{
    *(m_xGpioControl.apulGpioMmapPointers[ucPortN] + (GPIO_DATAOUT_OFFSET / 4)) &= ~(1 << ucPinN);
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

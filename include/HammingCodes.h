
//-----------------------------------------------------------------------------------------------------
//  Based on    : https://github.com/Minho-Cha/8bit-Hamming-Code
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CHAMMINGCODES_H
#define CHAMMINGCODES_H

//-----------------------------------------------------------------------------------------------------
#include <stdint.h>

// 11   10   9    8    7    6    5    4    3    2    1    0
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
//------------------------------------------------------------
//       BLOCK2                BLOCK1         BLOCK0
//--------------------    ----------------    ------
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |
//--------------------    ----------------    ------
// P0
//    ------    ------    ------    ------    ------    ======
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 0101 0101 0100 = 0x0554
//    ------    ------    ------    ------    ------    ======
//                        -----     -----------    -----------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b0101 1011 = 0x5B
//                        -----     -----------    -----------
// P1
//    -----------         -----------         -----======
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 0110 0110 0100 = 0x0664
//    -----------         -----------         -----======
//                        -----------    -----------    ------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b0110 1101 = 0x6D
//                        -----------    -----------    ------
// P2
//-----                   ---------------======
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 1000 0111 0000 = 0x0870
//-----                   ---------------======
//                   ------              ----------------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b1000 1110 = 0x8E
//                   ------              ----------------
// P3
//-------------------======
// D7 | D6 | D5 | D4 | P3 | D3 | D2 | D1 | P2 | D0 | P1 | P0 |      0b0000 1111 0000 0000 = 0x0F00
//-------------------======
//                   ---------------------
//                   | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |      0b1111 0000 = 0xF0
//                   ---------------------
#define P0_BIT  0
#define P1_BIT  1
#define P2_BIT  3
#define P3_BIT  7

#define BLOCK0 0x01
#define BLOCK1 0x0E
#define BLOCK2 0xF0

#define BLOCK0_SHIFT 2
#define BLOCK1_SHIFT 3
#define BLOCK2_SHIFT 4

#define P0_POSI_12 0x0554
#define P1_POSI_12 0x0664
#define P2_POSI_12 0x0870
#define P3_POSI_12 0x0F00

#define P0_POSI_8 0x5B
#define P1_POSI_8 0x6D
#define P2_POSI_8 0x8E
#define P3_POSI_8 0xF0


#define PACKET_SIZE 10

// Only a single bit error is possible
#define NOISE_BIT 1
#define NOISE_DATA 0x0003
#if 1
#define NOISE (1 << NOISE_BIT)
#else
#define  NOISE NOISE_DATA
#endif

//-----------------------------------------------------------------------------------------------------
class CHammingCodes
{
public:
    enum
    {
        HAMMING_CODE_8_4_BIT_NUMBER = 12,
    };

    enum
    {
        NONE_ERROR = 0,
    };

//    CHammingCodes();
//    virtual ~CHammingCodes();
    static uint16_t CalculateEncodedDataLength(uint16_t uiLength);
    static uint16_t BytesToHammingCodes(uint8_t* puiDestination, uint8_t* puiSource, uint16_t uiLength);
    static uint16_t HammingCodesToBytes(uint8_t* puiDestination, uint8_t* puiSource, uint16_t uiEncodedLength);

    static void SetErrorCode(uint8_t uiErrorCode)
    {
        m_uiErrorCode = uiErrorCode;
    };
    static uint8_t GetErrorCode(void)
    {
        return m_uiErrorCode;
    };

protected:
private:
    static uint16_t enHamming(uint8_t data);
    static uint8_t getEvenParityforTx(uint8_t data);
    static uint8_t findHammingError(uint16_t hamming);
    static uint8_t getEvenParityforRx(uint16_t data);
    static uint16_t fixError(uint16_t hammingData, uint8_t errorCode);
    static uint8_t deHamming(uint16_t hammingData);
    static uint8_t Recovery(uint16_t uiHammingData);

    static uint8_t m_uiErrorCode;
};

//-----------------------------------------------------------------------------------------------------
#endif // CHAMMINGCODES_H

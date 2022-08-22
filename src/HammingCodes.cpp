
//-----------------------------------------------------------------------------------------------------
//  Based on    : https://github.com/Minho-Cha/8bit-Hamming-Code
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include "HammingCodes.h"
#include "Crc.h"

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::m_uiErrorCode;

////-----------------------------------------------------------------------------------------------------
//CHammingCodes::CHammingCodes()
//{
//    //ctor
//}
//
////-----------------------------------------------------------------------------------------------------
//CHammingCodes::~CHammingCodes()
//{
//    //dtor
//}

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::getEvenParityforTx(uint8_t data) //?? ???
{
    uint8_t parity = 0;
    for (int i = 0; i < 8; i++)
    {
        parity ^= (data >> i) & 0x01;
    }

    return !parity;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::getEvenParityforRx(uint16_t data) //?? ???
{
    uint8_t parity = 0;
    for (int i = 0; i < 12; i++)
    {
        parity ^= (data >> i) & 0x01;
    }

    return !parity;
}

//-----------------------------------------------------------------------------------------------------
// Get Hamming code
uint16_t CHammingCodes::enHamming(uint8_t data)
{
    uint16_t hamming = 0;

    // Set Data
    hamming = (data & BLOCK2) << BLOCK2_SHIFT;  //0b1111 0000 0000
    hamming |= (data & BLOCK1) << BLOCK1_SHIFT;  //0b0000 0111 0000
    hamming |= (data & BLOCK0) << BLOCK0_SHIFT;  //0b0000 0000 0100

    // Set Parity
    hamming |= getEvenParityforTx(data & P3_POSI_8) << P3_BIT; // 0b1111 0000
    hamming |= getEvenParityforTx(data & P2_POSI_8) << P2_BIT; // 0b1000 1110
    hamming |= getEvenParityforTx(data & P1_POSI_8) << P1_BIT; // 0b0110 1101
    hamming |= getEvenParityforTx(data & P0_POSI_8) << P0_BIT; // 0b0101 1011

    return hamming;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::findHammingError(uint16_t hamming)
{
    uint8_t errCode = 0;

    errCode  = ((getEvenParityforRx(hamming & P3_POSI_12)) != ((hamming >> P3_BIT) & 0x0001)) << 3;
    errCode |= ((getEvenParityforRx(hamming & P2_POSI_12)) != ((hamming >> P2_BIT) & 0x0001)) << 2;
    errCode |= ((getEvenParityforRx(hamming & P1_POSI_12)) != ((hamming >> P1_BIT) & 0x0001)) << 1;
    errCode |= ((getEvenParityforRx(hamming & P0_POSI_12)) != ((hamming >> P0_BIT) & 0x0001)) << 0;

    return errCode;
}

//-----------------------------------------------------------------------------------------------------
uint16_t CHammingCodes::fixError(uint16_t hammingData, uint8_t errorCode)
{
    if (errorCode == NONE_ERROR)
    {
        return hammingData;
    }
    else
    {
        m_uiErrorCode = errorCode;
        return hammingData ^ (1 << (errorCode - 1));
    }

}

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::deHamming(uint16_t hammingData)
{
    uint8_t data = 0;

    data = (hammingData >> BLOCK2_SHIFT) & BLOCK2;
    data |= (hammingData >> BLOCK1_SHIFT) & BLOCK1;
    data |= (hammingData >> BLOCK0_SHIFT) & BLOCK0;

    return data;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CHammingCodes::Recovery(uint16_t uiHammingData)
{
    return deHamming(fixError(uiHammingData, findHammingError(uiHammingData)));
}

//-----------------------------------------------------------------------------------------------------
// Вычисляет длину закодированных данных.
uint16_t CHammingCodes::CalculateEncodedDataLength(uint16_t uiLength)
{
    // Длина нечётная?
    if (uiLength & 1)
    {
        // Выровняем длину.
        uiLength += 1;
    }

    // uiLength * 1.5 (HammingCodes 8 + 4 на один байт приходится 12 закодированных бит).
    return (uiLength + (uiLength / 2));
}

//-----------------------------------------------------------------------------------------------------
// Кодирует буфер в код Хемминга.
uint16_t CHammingCodes::BytesToHammingCodes(uint8_t* puiDestination, uint8_t* puiSource, uint16_t uiLength)
{
    uint16_t uiEncodedByteCounter = 0;
    uint16_t uiSourceIndex = 0;
    bool bTailIsPresent = false;

    // Количество байт данных нечётное?
    if ((uiLength > 0) && (uiLength & 1))
    {
        uiLength -= 1;
        bTailIsPresent = true;
    }

    // Закодируем все данные.
    for (uint16_t i = 0; i < uiLength; i += 2)
    {
        uint32_t uiData;
        // Получим кодовое слово 12 бит.
        uiData = static_cast<uint32_t>(enHamming(puiSource[uiSourceIndex]));
        // Следующий байт источника.
        uiSourceIndex++;
        // Получим кодовое слово 12 бит.
        uiData |= (static_cast<uint32_t>(enHamming(puiSource[uiSourceIndex])) << HAMMING_CODE_8_4_BIT_NUMBER);
        // Следующий байт источника.
        uiSourceIndex++;

        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
        uiData >>= 8;
        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
        uiData >>= 8;
        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
    }

    if (bTailIsPresent)
    {
        uint32_t uiData;
        // Получим кодовое слово 12 бит.
        uiData = static_cast<uint32_t>(enHamming(puiSource[uiSourceIndex]));
        // Получим кодовое слово 12 бит.
        uiData |= (static_cast<uint32_t>(enHamming(0)) << HAMMING_CODE_8_4_BIT_NUMBER);

        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
        uiData >>= 8;
        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
        uiData >>= 8;
        puiDestination[uiEncodedByteCounter] = static_cast<uint8_t>(uiData);
        // Адрес следующего байта для сохранения.
        // Увеличим количество закодированных байт.
        uiEncodedByteCounter++;
    }

    return uiEncodedByteCounter;
}

//-----------------------------------------------------------------------------------------------------
// Декодирует буфер с кодом Хемминга.
uint16_t CHammingCodes::HammingCodesToBytes(uint8_t* puiDestination, uint8_t* puiSource, uint16_t uiEncodedLength)
{
    uint16_t uiDecodedByteCounter = 0;
    uint16_t uiSourceIndex = 0;

    // uiEncodedLength = (uiLength * 1.5) (HammingCodes 8 + 4 на один байт приходится 12 закодированных бит - всегда кратно трём).
    // Длина закодированных данных не кратно трём?
    if ((uiEncodedLength > 0) && (uiEncodedLength % 3) != 0)
    {
        // Ошибка - не достаточно данных для восстановления.
        return 0;
    }

    // Декодируем все данные.
    for (uint16_t i = 0; i < uiEncodedLength; i += 3)
    {
        uint32_t uiData;

        uiData = (static_cast<uint32_t>(puiSource[uiSourceIndex]));
        // Следующий байт источника.
        uiSourceIndex++;
        uiData |= (static_cast<uint32_t>(puiSource[uiSourceIndex]) << 8);
        // Следующий байт источника.
        uiSourceIndex++;
        uiData |= (static_cast<uint32_t>(puiSource[uiSourceIndex]) << 16);
        // Следующий байт источника.
        uiSourceIndex++;

        // Восстановим закодированный байт.
        puiDestination[uiDecodedByteCounter] = Recovery((static_cast<uint16_t>(uiData) & 0x0FFF));
        // Увеличим количество декодированных байт.
        uiDecodedByteCounter++;
        // Восстановим закодированный байт.
        puiDestination[uiDecodedByteCounter] = Recovery((static_cast<uint16_t>(uiData >> HAMMING_CODE_8_4_BIT_NUMBER) & 0x0FFF));
        // Увеличим количество декодированных байт.
        uiDecodedByteCounter++;
    }

    return uiDecodedByteCounter;
}

//-----------------------------------------------------------------------------------------------------

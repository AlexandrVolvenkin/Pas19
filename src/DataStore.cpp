
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#include <iostream>
#include <string.h>

#include "DataStore.h"
#include "Crc.h"
#include "HammingCodes.h"



//-----------------------------------------------------------------------------------------------------
CDataStore::CDataStore()
{
    m_puiIntermediateBuff = new uint8_t[MAX_ENCODED_BLOCK_LENGTH];
    SetFsmState(IDDLE);
    SetSavedFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CDataStore::CDataStore(CStorageDevice* pxStorageDevice) :
    m_pxStorageDevice(pxStorageDevice)
{
    m_puiIntermediateBuff = new uint8_t[MAX_ENCODED_BLOCK_LENGTH];
    SetFsmState(IDDLE);
    SetSavedFsmState(IDDLE);
}

//-----------------------------------------------------------------------------------------------------
CDataStore::~CDataStore()
{
    delete m_puiIntermediateBuff;
    delete m_pxStorageDevice;

}

//-----------------------------------------------------------------------------------------------------
void CDataStore::CreateServiceSection(void)
{
    SetFsmState(IDDLE);

    // Очистим служебный контекст.
    memset(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
           0,
           sizeof(struct TServiseSectionData));

    // Установим размер первичных данных.
    m_xServiseSection.xServiseSectionData.uiLength = sizeof(struct TServiseSectionData);
    // Количество байт служебного контекста
    // должно быть известно заранее.
    // Так как по нему извлекаются начальные данные.
    // Используется самовосстанавливающийся код Хемминга(8,4).
    // Коэффициент - 1.5: один байт преобразуется в кодовое слово 12 бит,
    // из двух байт полезных данных получается три байта кодированных.
    // Установим размер закодированных данных.
    m_xServiseSection.xServiseSectionData.uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));
    // Установим смещение на начало данных блоков.
    m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset =
        (SERVICE_SECTION_DATA_BEGIN +
         m_xServiseSection.xServiseSectionData.uiEncodedLength);
    // Хранилище пусто.
    m_xServiseSection.xServiseSectionData.uiBlocksNumber = 0;
    // Установим признак - хранилище не подписано.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteTemporaryServiceSection(void)
{
    // Вычислим контрольную сумму.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> Write(TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteServiceSection(void)
{
    // Вычислим контрольную сумму.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    if (m_pxStorageDevice -> Write(SERVICE_SECTION_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::ReadTemporaryServiceSection(void)
{
    uint16_t uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));

    // Прочитаем закодированные данные.
    // При чтении данных возникла ошибка?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       uiEncodedLength);

    // Блок не повреждён?
    if (m_xServiseSection.uiCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                    sizeof(struct TServiseSectionData)))
    {
        return 1;
    }
    else
    {
        // Нет данных.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::ReadServiceSection(void)
{
    uint16_t uiEncodedLength =
        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSection)));

    // Прочитаем закодированные данные.
    // При чтении данных возникла ошибка?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    SERVICE_SECTION_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       uiEncodedLength);

    // Блок не повреждён?
    if (m_xServiseSection.uiCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                    sizeof(struct TServiseSectionData)))
    {
        return 1;
    }
    else
    {
        // Нет данных.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::CheckTemporaryBlock(void)
{
    uint8_t uiBlock = m_uiBlock;

    // Произошёл выход за границы буфера?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // Нет данных.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    //    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // Блок существует?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // Получим адрес блока в EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // Получим размер блока.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // Получим размер закодированного блока.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // Нет данных.
        return 0;
    }

    // Прочитаем закодированные данные.
    // При чтении данных возникла ошибка?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    TEMPORARY_BLOCK_DATA_BEGIN,
                                    uiEncodedLength)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(m_puiIntermediateBuff, m_puiIntermediateBuff, uiEncodedLength);

    // Блок не повреждён?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(m_puiIntermediateBuff, uiLength))
    {
        return 1;
    }
    else
    {
        // Нет данных.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::CheckBlock(void)
{
    uint8_t uiBlock = m_uiBlock;

    // Произошёл выход за границы буфера?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // Нет данных.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // Блок существует?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // Получим адрес блока в EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // Получим размер блока.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // Получим размер закодированного блока.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // Нет данных.
        return 0;
    }

    // Прочитаем закодированные данные.
    // При чтении данных возникла ошибка?
    if (!(m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                    uiSourceOffset,
                                    uiEncodedLength)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(auiTempArray, m_puiIntermediateBuff, uiEncodedLength);

    // Блок не повреждён?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(auiTempArray, uiLength))
    {
        return 1;
    }
    else
    {
        // Нет данных.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint16_t CDataStore::ReadBlock(uint8_t *puiDestination, uint8_t uiBlock)
{
    // Произошёл выход за границы буфера?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // Нет данных.
        return 0;
    }

    uint16_t uiLength;
    uint16_t uiEncodedLength;
    uint16_t uiSourceOffset;
    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    // Блок существует?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength != 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength != 0))
    {
        // Получим адрес блока в EEPROM.
        uiSourceOffset = m_xServiseSection.xServiseSectionData.
                         axBlockPositionData[uiBlock].uiOffset;
        // Получим размер блока.
        uiLength = m_xServiseSection.xServiseSectionData.
                   axBlockPositionData[uiBlock].uiLength;
        // Получим размер закодированного блока.
        uiEncodedLength = m_xServiseSection.xServiseSectionData.
                          axBlockPositionData[uiBlock].uiEncodedLength;
    }
    else
    {
        // Нет данных.
        return 0;
    }

    // Прочитаем закодированные данные.
    m_pxStorageDevice -> Read(m_puiIntermediateBuff, uiSourceOffset, uiEncodedLength);

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(auiTempArray, m_puiIntermediateBuff, uiEncodedLength);

    // Блок не повреждён?
    if (m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiCrc ==
            usCrc16(auiTempArray, uiLength))
    {
        memcpy(puiDestination, auiTempArray, uiLength);
        return uiLength;
    }
    else
    {
        // Нет данных.
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::WriteTemporaryBlock(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff, puiSource, uiLength);

    // Данные текущего блока данных - uiBlock сохраняются впервые?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength == 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength == 0))
    {
        // Добавили новый блок данных.
        // Сохраним смещение на блок.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiOffset = m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset;
        // Изменим смещение на свободное место.
        m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset += uiEncodedByteCounter;
        // Сохраним размер первичных данных.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiLength = uiLength;
        // Сохраним размер закодированных данных.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiEncodedLength = uiEncodedByteCounter;
        // Увеличим количество блоков находящихся в хранилище.
        m_xServiseSection.xServiseSectionData.uiBlocksNumber += 1;
    }

    // Вычислим контрольную сумму поступивших данных.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);

    if (m_pxStorageDevice -> Write(TEMPORARY_BLOCK_DATA_BEGIN,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// Преобразует блок в формат системы хранения.
// Передаёт данные автомату устройства хранения и запускает процесс записи.
uint8_t CDataStore::WriteBlock(void)
{
    uint8_t uiBlock = m_uiBlock;
    uint8_t* puiSource = m_puiBlockSource;
    uint16_t uiLength = m_uiBlockLength;

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff, puiSource, uiLength);

    // Данные текущего блока данных - uiBlock сохраняются впервые?
    if ((m_xServiseSection.xServiseSectionData.
            axBlockPositionData[uiBlock].uiLength == 0) &&
            (m_xServiseSection.xServiseSectionData.
             axBlockPositionData[uiBlock].uiEncodedLength == 0))
    {
        // Добавили новый блок данных.
        // Сохраним смещение на блок.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiOffset = m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset;
        // Изменим смещение на свободное место.
        m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset += uiEncodedByteCounter;
        // Сохраним размер первичных данных.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiLength = uiLength;
        // Сохраним размер закодированных данных.
        m_xServiseSection.xServiseSectionData.
        axBlockPositionData[uiBlock].uiEncodedLength = uiEncodedByteCounter;
//        // Увеличим количество блоков находящихся в хранилище.
//        m_xServiseSection.xServiseSectionData.uiBlocksNumber += 1;
    }

    // Вычислим контрольную сумму поступивших данных.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);
    // Сохраним индекс последнего записываемого блока.
    m_xServiseSection.xServiseSectionData.uiLastWritedBlockNumber = uiBlock;

    if (m_pxStorageDevice -> Write(m_xServiseSection.xServiseSectionData.
                                   axBlockPositionData[uiBlock].uiOffset,
                                   m_puiIntermediateBuff,
                                   uiEncodedByteCounter))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
// Передаёт данные контекста записи блока автомату хранилища данных и запускает процесс записи.
uint8_t CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    // Автомат не готов к записи?
    if (GetFsmState() != IDDLE)
    {
        SetFsmEvent(WRITE_ERROR_FSM_EVENT);
        return 0;
    }

    // Произошёл выход за границы буфера?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // Нет данных.
        return 0;
    }

    // Получим данные контекста записи блока.
    m_puiBlockSource = puiSource;
    m_uiBlockLength = uiLength;
    m_uiBlock = uiBlock;

    SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
    // Запустим процесс записи.
    SetFsmState(START_WRITE_TEMPORARY_BLOCK_DATA);
}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CompareCurrentWithStoredCrc(void)
{
    // Сравним текущее Crc блока с его Crc сохранённом в служебном блоке в предыдущей сессии записи.
    for (uint16_t i = SERVICE_SECTION_DATA_BLOCK_NUMBER;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        if (m_auiBlocksCurrentCrc[i] !=
                m_xServiseSection.xServiseSectionData.axBlockPositionData[i].uiCrc)
        {
            return false;
        }
    }

    // Все блоки принадлежат текущей базе данных.
    return true;
}

//-----------------------------------------------------------------------------------------------------
// Вызывается только если база данных подтверждена пользователем.
void CDataStore::CrcOfBlocksCrcCreate(void)
{
    // Контрольная сумма вычисленная из массива контрольных сумм блоков, не включая служебный.
    // Сохраняется при первой и последующих записях любых блоков через программатор.
    // Ноль или её несовпадение свидетельствует о том, что база данных создана по умоланию,
    // и не подтверждена пользователем. В этом случае прибор переходит в режим сигнализации об ошибке,
    // ожидая квитирования или записи базы данных.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER];

    // Получим Crc всех блоков, не включая служебный.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i + SERVICE_SECTION_DATA_BLOCK_NUMBER].uiCrc;
    }

    // Сохраним Crc всех блоков, не включая служебный.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc =
        usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                ((MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER) * sizeof(uint16_t)));

}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::CrcOfBlocksCrcCheck(void)
{
    // Контрольная сумма вычисленная из массива контрольных сумм блоков, не включая служебный.
    // Сохраняется при первой и последующих записях любых блоков через программатор.
    // Ноль или её несовпадение свидетельствует о том, что база данных создана по умоланию,
    // и не подтверждена пользователем. В этом случае прибор переходит в режим сигнализации об ошибке,
    // ожидая квитирования или записи базы данных.
    uint16_t auiBlocksCrc[MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER];

    // Получим Crc всех блоков, не включая служебный.
    for (uint16_t i = 0;
            i < (MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER);
            i++)
    {
        auiBlocksCrc[i] =
            m_xServiseSection.xServiseSectionData.axBlockPositionData[i + SERVICE_SECTION_DATA_BLOCK_NUMBER].uiCrc;
    }

    // Crc из Crc всех блоков совпадает с сохранённым в служебном блоке?
    if (m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc ==
            usCrc16(reinterpret_cast<uint8_t*>(auiBlocksCrc),
                    ((MAX_BLOCKS_NUMBER - SERVICE_SECTION_DATA_BLOCK_NUMBER) * sizeof(uint16_t))))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------
// Проверяет целостность и связанность временного служебного блока и блоков хранения.
// Целостность и связанность блоков определяется совпадением вычисленной и
// сохранённой во временном служебном блоке Crc текущего блока хранения.
uint8_t CDataStore::TemporaryServiceSectionAndBlocksCheck(void)
{
    // Блок повреждён?
    if (!(ReadTemporaryServiceSection()))
    {
        return 0;
    }

    uint8_t auiTempArray[MAX_BLOCK_LENGTH];

    cout << "TemporaryServiceSectionAndBlocksCheck uiBlocksNumber" << " " << (int)m_xServiseSection.xServiseSectionData.uiBlocksNumber << endl;
    for (uint16_t i = 0;
            i < m_xServiseSection.xServiseSectionData.uiBlocksNumber;
            i++)
    {
        // Блок повреждён?
        if (!(ReadBlock(m_puiIntermediateBuff, i)))
        {
            // Установим индекс блока с сохранённой Crc которого,
            // будем сравнивать Crc блока сохранённого во временном буфере.
            m_uiBlock = i;
            // Crc блока из временного буфера совпадает с Crc блока
            // сохранённого во временном служебном буфере по текущему индексу?
            if (CheckTemporaryBlock())
            {
                memcpy(auiTempArray,
                       m_puiIntermediateBuff,
                       m_xServiseSection.xServiseSectionData.
                       axBlockPositionData[i].uiLength);
                // Данные блока успешно записаны во временные буферы,
                // но при записи в хранилище произошёл сбой.
                // Требуется повторная запись данных из временных буферов в хранилище.
                WriteBlock(auiTempArray,
                           m_xServiseSection.xServiseSectionData.
                           axBlockPositionData[i].uiLength,
                           i);

//                do
//                {
//                    Fsm();
//                }
//                while (GetFsmState() != CDataStore::IDDLE);

                // Блок не повреждён?
                if (!(ReadBlock(m_puiIntermediateBuff, i)))
                {
                    return 0;
                }
            }
        }
        else
        {
            // Блок восстановлен после обнаружения ошибки?
            if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
            {
                CHammingCodes::SetErrorCode(0);
                cout << "CHammingCodes::GetErrorCode uiBlock" << (int)i << endl;
                memcpy(auiTempArray,
                       m_puiIntermediateBuff,
                       m_xServiseSection.xServiseSectionData.
                       axBlockPositionData[i].uiLength);
                // Требуется повторная запись данных из временных буферов в хранилище.
                WriteBlock(auiTempArray,
                           m_xServiseSection.xServiseSectionData.
                           axBlockPositionData[i].uiLength,
                           i);

                do
                {
                    Fsm();
                }
                while (GetFsmState() != CDataStore::IDDLE);

                // Блок не повреждён?
                if (!(ReadBlock(m_puiIntermediateBuff, i)))
                {
                    return 0;
                }
            }
        }

    }

    return 1;
}

//-----------------------------------------------------------------------------------------------------
// Проверяет целостность и связанность служебного блока и блоков хранения.
// Целостность и связанность блоков определяется совпадением вычисленной и
// сохранённой в служебном блоке Crc текущего блока хранения.
uint8_t CDataStore::ServiceSectionAndBlocksCheck(void)
{
    // Блок повреждён?
    if (!(ReadServiceSection()))
    {
        return 0;
    }

    for (uint16_t i = 0;
            i < m_xServiseSection.xServiseSectionData.uiBlocksNumber;
            i++)
    {
        // Блок повреждён?
        if (!(ReadBlock(m_puiIntermediateBuff, i)))
        {
            return 0;
        }
    }

    return 1;
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::Check(void)
{
    enum
    {
        IDDLE = 0,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK,
        TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK,

        DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED,
        DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED,

        DATA_STORE_NOT_CORRUPTED,
        DATA_STORE_CORRUPTED,
    };

    uint8_t uiFsmState;
    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
    uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK;


    while (1)
    {
        switch (uiFsmState)
        {
        case IDDLE:
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK:
            // Блоки не повреждены и связаны с временным служебным блоком?
            if (TemporaryServiceSectionAndBlocksCheck())
            {
                uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK;
            }
            else
            {
                uiFsmState = TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK;
            }
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK_OK_SERVICE_SECTION_DATA_CHECK:
            // Блоки не повреждены и связаны со служебным блоком?
            if (ServiceSectionAndBlocksCheck())
            {
                uiFsmState = DATA_STORE_NOT_CORRUPTED;
            }
            else
            {
                uiFsmState = DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED;
            }
            break;

        case TEMPORARY_SERVICE_SECTION_DATA_CHECK_ERROR_SERVICE_SECTION_DATA_CHECK:
            // Блоки не повреждены и связаны со служебным блоком?
            if (ServiceSectionAndBlocksCheck())
            {
                uiFsmState = DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED;
            }
            else
            {
                uiFsmState = DATA_STORE_CORRUPTED;
            }
            break;

        case DATA_STORE_CORRUPTED_NEW_VERSION_ACCEPTED:
            uiFsmState = DATA_STORE_NOT_CORRUPTED;
            break;

        case DATA_STORE_CORRUPTED_OLD_VERSION_ACCEPTED:
            uiFsmState = DATA_STORE_NOT_CORRUPTED;
            break;

        case DATA_STORE_CORRUPTED:
            return 0;
            break;

        case DATA_STORE_NOT_CORRUPTED:
            // данные не повреждены.
            return 1;
            break;

        default:
            return 0;
            break;
        }
    }
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Fsm(void)
{
    switch (GetFsmState())
    {
    case IDDLE:
        break;


//-----------------------------------------------------------------------------------------------------
        // Запись блока во временный буфер.
    case START_WRITE_TEMPORARY_BLOCK_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_TEMPORARY_BLOCK_DATA);
        break;

    case READY_TO_WRITE_WAITING_TEMPORARY_BLOCK_DATA:
        // Устройство хранения готово к записи?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // Подготовка к записи прошла успешно?
            if (WriteTemporaryBlock())
            {
                SetFsmState(WRITE_END_WAITING_TEMPORARY_BLOCK_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_TEMPORARY_BLOCK_DATA:
        // Устройство хранения закончило запись?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // Запись прошла успешно?
            if (CheckTemporaryBlock())
            {
                cerr << "CheckTemporaryBlock ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_TEMPORARY_SERVICE_SECTION_DATA);
            }
            else
            {
                cerr << "CheckTemporaryBlock error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // Запись временного служеьного блока.
    case START_WRITE_TEMPORARY_SERVICE_SECTION_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_TEMPORARY_SERVICE_SECTION_DATA);
        break;

    case READY_TO_WRITE_WAITING_TEMPORARY_SERVICE_SECTION_DATA:
        // Устройство хранения готово к записи?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // Подготовка к записи прошла успешно?
            if (WriteTemporaryServiceSection())
            {
                SetFsmState(WRITE_END_WAITING_TEMPORARY_SERVICE_SECTION_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_TEMPORARY_SERVICE_SECTION_DATA:
        // Устройство хранения закончило запись?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // Запись прошла успешно?
            if (ReadTemporaryServiceSection())
            {
                cerr << "ReadTemporaryServiceSection ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_BLOCK_DATA);
            }
            else
            {
                cerr << "ReadTemporaryServiceSection error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // Запись блока.
    case START_WRITE_BLOCK_DATA:
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_BLOCK_DATA);
        break;

    case READY_TO_WRITE_WAITING_BLOCK_DATA:
        // Устройство хранения готово к записи?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // Подготовка к записи прошла успешно?
            if (WriteBlock())
            {
                SetFsmState(WRITE_END_WAITING_BLOCK_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_BLOCK_DATA:
        // Устройство хранения закончило запись?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // Запись прошла успешно?
            if (CheckBlock())
            {
                cerr << "CheckBlock ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
            }
            else
            {
                cerr << "CheckBlock error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


//-----------------------------------------------------------------------------------------------------
        // Запись служеьного блока.
    case START_WRITE_SERVICE_SECTION_DATA:
        SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA);
        break;

    case READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA:
        // Устройство хранения готово к записи?
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            // Подготовка к записи прошла успешно?
            if (WriteServiceSection())
            {
                SetFsmState(WRITE_END_WAITING_SERVICE_SECTION_DATA);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_SERVICE_SECTION_DATA:
        // Устройство хранения закончило запись?
        if (m_pxStorageDevice -> IsDataWrited())
        {
            // Запись прошла успешно?
            if (ReadServiceSection())
            {
                cerr << "ReadServiceSection ok" << endl;
                SetFsmEvent(WRITE_OK_FSM_EVENT);
                SetFsmState(IDDLE);
            }
            else
            {
                cerr << "ReadServiceSection error" << endl;
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        break;


    default:
        SetFsmEvent(WRITE_ERROR_FSM_EVENT);
        SetFsmState(IDDLE);
        break;
    }
}

//-----------------------------------------------------------------------------------------------------














//    case BLOCK_CHECK_START:
//        // Проверены не все блоки?
//        if (uiBlockCounter < MAX_BLOCKS_NUMBER)
//        {
//            uiLength = ReadBlock(m_puiIntermediateBuff, uiBlockCounter);
//            // Блок не повреждён?
//            if (uiLength)
//            {
//                // Блок восстановлен после обнаружения ошибки?
//                if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
//                {
//                    // Обновим восстановленный блок в хранилище.
//                    uiFsmState = BLOCK_WRITE_START;
//                }
//                else
//                {
//                    uiFsmState = NEXT_BLOCK;
//                }
//            }
//            else
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//        }
//        else
//        {
//            uiFsmState = ALL_BLOCKS_CHECKED;
//        }
//        break;
//
//    case BLOCK_WRITE_START:
//        // Поместим данные в хранилище.
//        // Блок БД принят к записи?
//        if (WriteBlock(m_puiIntermediateBuff, uiLength, uiBlockCounter))
//        {
//            uiFsmState = BLOCK_WRITE_END_WAITING;
//        }
//        // При записи блока БД произошла ошибка?
//        else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//        {
//            uiFsmState = BLOCK_ERROR;
//        }
//        break;
//
//    case BLOCK_WRITE_END_WAITING:
//        // Блок записан успешно?
//        if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
//        {
//            uiFsmState = NEXT_BLOCK;
//        }
//        // При записи блока БД произошла ошибка?
//        else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//        {
//            uiFsmState = BLOCK_ERROR;
//        }
//        break;

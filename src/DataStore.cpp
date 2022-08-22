
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
//    delete m_pxStorageDevice;
    delete m_puiIntermediateBuff;
    delete m_pxStorageDevice;

}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Init(void)
{
//    SetFsmState(IDDLE);
//
//    // Очистим служебный контекст.
//    memset(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
//           0,
//           sizeof(m_xServiseSection.xServiseSectionData));
//
//    // Установим смещение на начало служебной секции.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiOffset = SERVICE_SECTION_DATA_BEGIN;
//    // Установим размер первичных данных.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiLength = sizeof(struct TServiseSectionData);
//    // Количество байт служебного контекста
//    // должно быть известно заранее.
//    // Так как по нему извлекаются начальные данные.
//    // Используется самовосстанавливающийся код Хемминга(8,4).
//    // Коэффициент - 1.5: один байт преобразуется в кодовое слово 12 бит,
//    // из двух байт полезных данных получается три байта кодированных.
//    // Установим размер закодированных данных.
//    m_xServiseSection.xServiseSectionData.
//    axBlockPositionData[SERVICE_SECTION_DATA].uiEncodedLength =
//        (CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSectionData) + CRC_LENGTH));
//    // Установим смещение для служебного контекста.
//    m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset =
//        (SERVICE_SECTION_DATA_BEGIN +
//         CHammingCodes::CalculateEncodedDataLength(sizeof(struct TServiseSectionData) + CRC_LENGTH));
//    // Установим признак - база данных не подтверждена пользователем.
//    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
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
    // Установим признак - хранилище не подписано.
    m_xServiseSection.xServiseSectionData.uiCrcOfBlocksCrc = 0;
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::WriteTemporaryServiceSection(void)
{
    // Вычислим контрольную сумму.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    m_pxStorageDevice -> Write(TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                               m_puiIntermediateBuff,
                               uiEncodedByteCounter);
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::WriteServiceSection(void)
{
    // Вычислим контрольную сумму.
    m_xServiseSection.uiCrc =
        usCrc16(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
                sizeof(struct TServiseSectionData));

    uint16_t uiEncodedByteCounter =
        CHammingCodes::BytesToHammingCodes(m_puiIntermediateBuff,
                                           reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                           sizeof(struct TServiseSection));

    m_pxStorageDevice -> Write(SERVICE_SECTION_DATA_BEGIN,
                               m_puiIntermediateBuff,
                               uiEncodedByteCounter);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CDataStore::ReadTemporaryServiceSection(void)
{
    // Прочитаем закодированные данные.
    if (m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                  TEMPORARY_SERVICE_SECTION_DATA_BEGIN,
                                  sizeof(struct TServiseSection)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       sizeof(struct TServiseSection));

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
    // Прочитаем закодированные данные.
    if (m_pxStorageDevice -> Read(m_puiIntermediateBuff,
                                  SERVICE_SECTION_DATA_BEGIN,
                                  sizeof(struct TServiseSection)))
    {
        // Нет данных.
        return 0;
    }

    // Декодируем прочитанные данные.
    CHammingCodes::HammingCodesToBytes(reinterpret_cast<uint8_t*>(&m_xServiseSection),
                                       m_puiIntermediateBuff,
                                       sizeof(struct TServiseSection));

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
            usCrc16(m_puiIntermediateBuff, uiLength))
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
uint16_t CDataStore::Write(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    // Произошёл выход за границы буфера?
    if (uiBlock >= MAX_BLOCKS_NUMBER)
    {
        // Нет данных.
        return 0;
    }

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
    }

    // Вычислим контрольную сумму поступивших данных.
    m_xServiseSection.xServiseSectionData.
    axBlockPositionData[uiBlock].uiCrc =
        usCrc16(puiSource, uiLength);

    m_pxStorageDevice -> WritePrepare(m_xServiseSection.xServiseSectionData.uiFreeSpaceOffset,
                                      m_puiIntermediateBuff,
                                      uiEncodedByteCounter);

    // Out length.
    return uiEncodedByteCounter;
}

//-----------------------------------------------------------------------------------------------------
bool CDataStore::WriteBlock(uint8_t *puiSource, uint16_t uiLength, uint8_t uiBlock)
{
    if (GetFsmState() == IDDLE)
    {
        if (Write(puiSource,
                  uiLength,
                  uiBlock))
        {
            SetFsmEvent(WRITE_IN_PROGRESS_FSM_EVENT);
            SetFsmState(START_WRITE);
            return true;
        }
        else
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            return false;
        }
    }
    else
    {
        SetFsmEvent(STORAGE_DEVICE_BUSY_FSM_EVENT);
        return false;
    }
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
uint8_t CDataStore::Check(void)
{
//    CDataStore::Init();
//
//    // Служебный блок повреждён?
//    if (!(ReadBlock(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData), SERVICE_SECTION_DATA)))
//    {
//        return 0;
//    }
//
//    enum
//    {
//        IDDLE = 0,
//        BLOCK_CHECK_START,
//        BLOCK_WRITE_START,
//        BLOCK_WRITE_END_WAITING,
//        NEXT_BLOCK,
//        BLOCK_ERROR,
//        ALL_BLOCKS_CHECKED,
//    };
//
//    uint8_t uiBlockCounter = 0;
//    uint8_t uiFsmState = BLOCK_CHECK_START;
//    uint16_t uiLength;
//
//    CHammingCodes::SetErrorCode(CHammingCodes::NONE_ERROR);
//
//    while (1)
//    {
//        switch (uiFsmState)
//        {
//        case BLOCK_CHECK_START:
//            // Проверены не все блоки?
//            if (uiBlockCounter < MAX_BLOCKS_NUMBER)
//            {
//                uiLength = ReadBlock(m_puiIntermediateBuff, uiBlockCounter);
//                // Блок не повреждён?
//                if (uiLength)
//                {
//                    // Блок восстановлен после обнаружения ошибки?
//                    if (CHammingCodes::GetErrorCode() != CHammingCodes::NONE_ERROR)
//                    {
//                        // Обновим восстановленный блок в хранилище.
//                        uiFsmState = BLOCK_WRITE_START;
//                    }
//                    else
//                    {
//                        uiFsmState = NEXT_BLOCK;
//                    }
//                }
//                else
//                {
//                    uiFsmState = BLOCK_ERROR;
//                }
//            }
//            else
//            {
//                uiFsmState = ALL_BLOCKS_CHECKED;
//            }
//            break;
//
//        case BLOCK_WRITE_START:
//            // Поместим данные в хранилище.
//            // Блок БД принят к записи?
//            if (WriteBlock(m_puiIntermediateBuff, uiLength, uiBlockCounter))
//            {
//                uiFsmState = BLOCK_WRITE_END_WAITING;
//            }
//            // При записи блока БД произошла ошибка?
//            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//            break;
//
//        case BLOCK_WRITE_END_WAITING:
//            // Блок записан успешно?
//            if (CDataStore::GetFsmEvent() == CDataStore::WRITE_OK_FSM_EVENT)
//            {
//                uiFsmState = NEXT_BLOCK;
//            }
//            // При записи блока БД произошла ошибка?
//            else if (CDataStore::GetFsmEvent() == CDataStore::WRITE_ERROR_FSM_EVENT)
//            {
//                uiFsmState = BLOCK_ERROR;
//            }
//            break;
//
//        case NEXT_BLOCK:
//            uiBlockCounter++;
//            uiFsmState = BLOCK_CHECK_START;
//            break;
//
//        case ALL_BLOCKS_CHECKED:
//            // данные не повреждены.
//            return 1;
//            break;
//
//        case BLOCK_ERROR:
//            return 0;
//            break;
//
//        default:
//            break;
//        }
//
//        CDataStore::Fsm();
//
//        delay_ms(10);
//
//        CPlatform::WatchdogReset();
//    }
}

//-----------------------------------------------------------------------------------------------------
void CDataStore::Fsm(void)
{
    switch (GetFsmState())
    {
    case IDDLE:
        break;

    case START_WRITE:
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING);
        break;

    case READY_TO_WRITE_WAITING:
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            if (m_pxStorageDevice -> Write())
            {
                SetFsmState(WRITE_END_WAITING);
            }
            else
            {
                SetFsmEvent(WRITE_ERROR_FSM_EVENT);
                // Восстановим служебные данные.
                ReadServiceSection();
                SetFsmState(IDDLE);
            }
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            // Восстановим служебные данные.
            ReadServiceSection();
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING:
        if (m_pxStorageDevice -> IsDataWrited())
        {
            SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            // Восстановим служебные данные.
            ReadServiceSection();
            SetFsmState(IDDLE);
        }
        break;


        // Запись служеьного блока.
    case START_WRITE_SERVICE_SECTION_DATA:
        // Данные обновлены.
        // Сохраним изменённый служебный контекст.
        WriteServiceSection();
        // Установим время ожидания готовности к записи.
        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
        SetFsmState(READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA);
        break;

    case READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA:
        if (m_pxStorageDevice -> IsReadyToWrite())
        {
            // Установим время ожидания окончания записи.
            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
            m_pxStorageDevice -> SetIsDataWrited(false);
            m_pxStorageDevice -> Write();
            SetFsmState(WRITE_END_WAITING_SERVICE_SECTION_DATA);
        }
        // Время ожидания готовности к записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            // Восстановим служебные данные.
            ReadServiceSection();
            SetFsmState(IDDLE);
        }
        break;

    case WRITE_END_WAITING_SERVICE_SECTION_DATA:
        if (m_pxStorageDevice -> IsDataWrited())
        {
            SetFsmEvent(WRITE_OK_FSM_EVENT);
            SetFsmState(IDDLE);
        }
        // Время ожидания окончания записи закончилось?
        else if (GetTimerPointer() -> IsOverflow())
        {
            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
            // Восстановим служебные данные.
            ReadServiceSection();
            SetFsmState(IDDLE);
        }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------------------------------








//-----------------------------------------------------------------------------------------------------
CStorageDeviceFileSystem::CStorageDeviceFileSystem()
{

}

//-----------------------------------------------------------------------------------------------------
CStorageDeviceFileSystem::~CStorageDeviceFileSystem()
{

}

//-----------------------------------------------------------------------------------------------------
void CStorageDeviceFileSystem::Init(void)
{
}

//-----------------------------------------------------------------------------------------------------
void CStorageDeviceFileSystem::WritePrepare(uint16_t uiDestination, uint8_t *puiSource, uint16_t uiLength)
{
    SetOffset(uiDestination);
    SetBufferPointer(puiSource);
    SetLength(uiLength);
}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Write(uint16_t uiOffset, uint8_t *puiSource, uint16_t uiLength)
{
    cout << "CStorageDeviceFileSystem::Write uiOffset" << " " << (int)uiOffset << endl;
    cout << "CStorageDeviceFileSystem::Write uiLength" << " " << (int)uiLength << endl;
    cout << "CStorageDeviceFileSystem::Write sizeof" << " " << (int)(sizeof(struct CDataStore::TServiseSection)) << endl;

//    cout << "CStorageDeviceFileSystem::Write pccFileName" << " " << pccFileName << endl;
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ofstream outdata;
        // Чтобы добавить и не стереть старые данные откроем файл на чтение и запись.
        outdata.open(pccFileName, (ios::binary | ios::in | ios::out));
        // Файл не существует?
        if (!outdata)
        {
            cerr << "CStorageDeviceFileSystem::Write Error: file could not be opened" << endl;
            // чтобы создать файл откроем только на запись.
            outdata.open(pccFileName, (ios::binary | ios::out));
            // Файл не создан?
            if (!outdata)
            {
                cerr << "CStorageDeviceFileSystem::Write Error: file could not be created" << endl;
                return 0;
            }
        }

        outdata.seekp(uiOffset, ios_base::beg);
        outdata.write((char*)puiSource, uiLength);

        // закроем файл.
        outdata.close();

        SetIsDataWrited(true);
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Write(void)
{
    return Write(GetOffset(),
                 GetBufferPointer(),
                 GetLength());
}

//-----------------------------------------------------------------------------------------------------
uint8_t CStorageDeviceFileSystem::Read(uint8_t *puiDestination, uint16_t uiOffset, uint16_t uiLength)
{
    if ((uiOffset + uiLength) < MAX_BUFFER_LENGTH)
    {
        ifstream indata;
        // откроем файл.
        indata.open(pccFileName, (ios::in | ios::binary));
        if (!indata)
        {
            cerr << "CStorageDeviceFileSystem::Read Error: file could not be opened" << endl;
            return 0;
        }
        else
        {
            // установим смещение в файле.
            indata.seekg(uiOffset, ios_base::beg);
            // прочитаем файл.
            indata.read(reinterpret_cast<char*>(puiDestination),
                        uiLength);
        }
        // закроем файл.
        indata.close();
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------
void CStorageDeviceFileSystem::Fsm(void)
{
//    enum
//    {
//        IDDLE = 0,
//        START_WRITE,
//        READY_TO_WRITE_WAITING,
//        WRITE_END_WAITING,
//
//        START_WRITE_SERVICE_SECTION_DATA,
//        READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA,
//        WRITE_END_WAITING_SERVICE_SECTION_DATA,
//    };
//
//    switch (GetFsmState())
//    {
//    case IDDLE:
//        break;
//
//    case START_WRITE:
//        // Установим время ожидания готовности к записи.
//        GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
//        SetFsmState(READY_TO_WRITE_WAITING);
//        break;
//
//    case READY_TO_WRITE_WAITING:
//        if (m_pxStorageDevice -> IsReadyToWrite())
//        {
//            // Установим время ожидания окончания записи записи.
//            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
    //    m_pxStorageDevice -> SetIsDataWrited(false);
//            m_pxStorageDevice -> Write();
//            SetFsmState(WRITE_END_WAITING);
//        }
//        // Время ожидания готовности к записи закончилось?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case WRITE_END_WAITING:
//        if (m_pxStorageDevice -> IsDataWrited())
//        {
//            SetFsmState(START_WRITE_SERVICE_SECTION_DATA);
//        }
//        // Время ожидания окончания записи закончилось?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//
//        // Запись служеьного блока.
//    case START_WRITE_SERVICE_SECTION_DATA:
//        // Данные обновлены.
//        // Сохраним изменённый служебный контекст.
//        if (Write(reinterpret_cast<uint8_t*>(&m_xServiseSection.xServiseSectionData),
//                  sizeof(m_xServiseSection.xServiseSectionData),
//                  SERVICE_SECTION_DATA))
//        {
//            // Установим время ожидания готовности к записи.
//            GetTimerPointer() -> Set(READY_TO_WRITE_WAITING_TIMEOUT);
//            SetFsmState(READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA);
//        }
//        else
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case READY_TO_WRITE_WAITING_SERVICE_SECTION_DATA:
//        if (m_pxStorageDevice -> IsReadyToWrite())
//        {
//            // Установим время ожидания окончания записи.
//            GetTimerPointer() -> Set(WRITE_END_WAITING_TIMEOUT);
//    m_pxStorageDevice -> SetIsDataWrited(false);
    //            m_pxStorageDevice -> Write();
//            SetFsmState(WRITE_END_WAITING_SERVICE_SECTION_DATA);
//        }
//        // Время ожидания готовности к записи закончилось?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    case WRITE_END_WAITING_SERVICE_SECTION_DATA:
//        if (m_pxStorageDevice -> IsDataWrited())
//        {
//            SetFsmEvent(WRITE_OK_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        // Время ожидания окончания записи закончилось?
//        else if (GetTimerPointer() -> IsOverflow())
//        {
//            SetFsmEvent(WRITE_ERROR_FSM_EVENT);
//            SetFsmState(IDDLE);
//        }
//        break;
//
//    default:
//        break;
//    }
}

//-----------------------------------------------------------------------------------------------------




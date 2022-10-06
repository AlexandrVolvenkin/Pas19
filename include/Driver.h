//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDRIVER_H
#define CDRIVER_H

#include <stdint.h>
#include "DataTypes.h"

//-----------------------------------------------------------------------------------------------------
class CDriver
{
public:
    virtual void Allocate(TMemoryAllocationConext &xMemoryAllocationConext) = 0;
    virtual uint8_t DataExchange(void) = 0;

protected:
    uint8_t m_uiType;
};

//-----------------------------------------------------------------------------------------------------
#endif // CDRIVER_H

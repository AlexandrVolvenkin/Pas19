
//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CMAINPRODUCTIONCYCLE_H
#define CMAINPRODUCTIONCYCLE_H

#include <iostream>
#include <string.h>

#include "DataStore.h"
#include "Crc.h"

//-----------------------------------------------------------------------------------------------------
class CMainProductionCycle : public CDfa
{
public:
    CMainProductionCycle();
    virtual ~CMainProductionCycle();
    void Fsm(void);

    enum
    {
        IDDLE = 0,
    };

protected:
private:
};

//-----------------------------------------------------------------------------------------------------
#endif // CMAINPRODUCTIONCYCLE_H

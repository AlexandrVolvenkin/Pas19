
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
#include "MainProductionCycle.h"

//-----------------------------------------------------------------------------------------------------
CMainProductionCycle::CMainProductionCycle()
{
    //ctor
}

//-----------------------------------------------------------------------------------------------------
CMainProductionCycle::~CMainProductionCycle()
{
    //dtor
}

//-----------------------------------------------------------------------------------------------------
void CMainProductionCycle::Fsm(void)
{
    switch (GetFsmState())
    {
    case IDDLE:
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------------------------------

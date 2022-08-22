//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef TIMERS_H_INCLUDED
#define TIMERS_H_INCLUDED

#include <stdint.h>

//-----------------------------------------------------------------------------------------------------
class CTimer
{
public:
//-----------------------------------------------------------------------------------------------------
    CTimer();
    CTimer(uint16_t uiTime);
    virtual ~CTimer();
    void Set(uint16_t uiTime);
    void Reset(void);
    bool IsOverflow(void);

//-----------------------------------------------------------------------------------------------------
private:
    uint16_t m_uiTime;
    uint16_t m_uiLastSystemTick;
};
//-----------------------------------------------------------------------------------------------------
#endif // TIMERS_H_INCLUDED

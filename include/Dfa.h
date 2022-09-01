//-----------------------------------------------------------------------------------------------------
//  Source      : FileName.cpp
//  Created     : 01.06.2022
//  Author      : Alexandr Volvenkin
//  email       : aav-36@mail.ru
//  GitHub      : https://github.com/AlexandrVolvenkin
//-----------------------------------------------------------------------------------------------------
#ifndef CDFA_H
#define CDFA_H

#include <stdint.h>
#include "Timer.h"

//-----------------------------------------------------------------------------------------------------
class CDfaLite
{
public:
    CDfaLite();
    virtual ~CDfaLite();
    virtual void Fsm(void);
    uint8_t GetFsmState(void)
    {
        return m_uiFsmState;
    };

//protected:
    void SetFsmState(uint8_t uiData)
    {
        m_uiFsmState = uiData;
    };

    uint8_t m_uiFsmState;
};

//-----------------------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------------------
class CDfa : public CDfaLite
{
public:
    CDfa();
    virtual ~CDfa();
    uint8_t GetSavedFsmState(void)
    {
        return m_uiSavedFsmState;
    };

    uint8_t GetFsmEvent(void)
    {
        return m_uiFsmEvent;
    };

//protected:
    void SetSavedFsmState(uint8_t uiData)
    {
        m_uiSavedFsmState = uiData;
    };
    void SetFsmEvent(uint8_t uiData)
    {
        m_uiFsmEvent = uiData;
    };
    CTimer* GetTimerPointer(void)
    {
        return &m_xTimer;
    };

    uint8_t m_uiSavedFsmState;
    uint8_t m_uiFsmEvent;
    CTimer m_xTimer;
};


//-----------------------------------------------------------------------------------------------------
#endif // CDFA_H

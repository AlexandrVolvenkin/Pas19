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

//-----------------------------------------------------------------------------------------------------
class CDfaLite
{
public:
    CDfaLite();
    virtual ~CDfaLite();
    virtual void Fsm(void);
    void SetFsmState(uint8_t uiData)
    {
        m_uiFsmState = uiData;
    };
    uint8_t GetFsmState(void)
    {
        return m_uiFsmState;
    };

protected:
    uint8_t m_uiFsmState;
};

//-----------------------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------------------
class CDfa : public CDfaLite
{
public:
    CDfa();
    virtual ~CDfa();
    void SetSavedFsmState(uint8_t uiData)
    {
        m_uiSavedFsmState = uiData;
    };
    uint8_t GetSavedFsmState(void)
    {
        return m_uiSavedFsmState;
    };

    void SetFsmEvent(uint8_t uiData)
    {
        m_uiFsmEvent = uiData;
    };
    uint8_t GetFsmEvent(void)
    {
        return m_uiFsmEvent;
    };

protected:
    uint8_t m_uiSavedFsmState;
    uint8_t m_uiFsmEvent;
};


//-----------------------------------------------------------------------------------------------------
#endif // CDFA_H

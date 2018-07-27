/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                                   msOS
*
*                            Ӳ��ƽ̨:msPLC DEMO
*                          ��оƬ:STM32F103R8T6/RBT6
*                           ��������οƼ����޹�˾
*
*                                ����:����ΰ
*                                ����:������
*                                ��ʶ:Wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        ��̳:http://bbs.huayusoft.com
*                        �Ա���:http://52edk.taobao.com
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���     : device.h
*����       : �����豸����غ궨��
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/06/21       ��ʼ�汾
********************************************************************************
*/


#ifndef __DEVICE_H
#define __DEVICE_H



typedef enum 
{
    KeyAdd                  = 0x00,
    KeyLongAdd              = 0x30,
    
    KeySub                  = 0x04,
    KeyLongSub              = 0x34,

    KeyAuxUp                = 0x01,
    KeyLongAuxUp            = 0x31,

    KeyAuxDown              = 0x05,
    KeyLongAuxDown          = 0x35,
    
    KeySave                 = 0x02,
    KeyLongSave             = 0x32,

    KeyTextBoxFocus         = 0x06,
    KeyLongFormFocus        = 0x36,
    
    KeyStart                = 0x03,
    KeyLongStart            = 0x33,
    
    KeyStop                 = 0x07,
    KeyLongStop             = 0x37
}KeyEnum;

typedef enum 
{
    TimerSystick            = 0,
    TimerMessage            = 1	
}TimerModeEnum;	

typedef enum
{
    Systick10000,
    Systick1000,
    Systick100
}SystickEnum;

typedef enum
{
    PwmChannel0             = 0,    // 16bit TIM
    PwmChannel1             = 1,    // 16bit TIM
    PwmChannel2             = 2,    // 16bit TIM
    PwmChannel3             = 3,    // 16bit TIM
    PwmChannel4             = 4,    // 16bit TIM
    PwmChannel5             = 5     // 16bit TIM
}PwmEnum;

typedef struct 
{
    byte Second;
    byte Minute;
    byte Hour;
    byte Day;
    byte Week;
    byte Month;
    ushort Year; 
}RtcStruct;

typedef enum 
{
    DiX                     = 0,    // ������
    DiCapture               = 1	    // PWM
}DiModeEnum;	

typedef struct
{
    bool X0;
    bool X1;
    bool X2;
    bool X3;
    bool * pX0;
    bool * pX1;
    bool * pX2;
    bool * pX3;
}DiStruct;

typedef enum 
{
    DoY                     = 0,
    DoPwm                   = 1	
}DoModeEnum;	

typedef struct 
{
    bool Y0;
    bool Y1;
    bool Y2;
    bool Y3;
    bool Y4;    
    bool Y5;
    bool * pY0;
    bool * pY1;
    bool * pY2;
    bool * pY3;
    bool * pY4;    
    bool * pY5;
} DoStruct;

typedef enum
{
    AdcChannel0             = 0,
    AdcChannel1             = 1,
    AdcChannel2             = 2,
    AdcChannel3             = 3,
    AdcTemperature          = 4         
}AdcChannelEnum;

typedef struct 
{
    ushort A0;
    ushort A1;
    ushort A2;
    ushort A3;
    ushort * pA0;
    ushort * pA1;
    ushort * pA2;
    ushort * pA3;
}AdcStruct;

extern void InitDevice(void);

#endif



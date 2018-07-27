/*
********************************************************************************
*                                嵌入式微系统
*                                   msOS
*
*                            硬件平台:msPLC DEMO
*                          主芯片:STM32F103R8T6/RBT6
*                           深圳市雨滴科技有限公司
*
*                                作者:王绍伟
*                                网名:凤舞天
*                                标识:Wangsw
*
*                                QQ:26033613
*                               QQ群:291235815
*                        论坛:http://bbs.huayusoft.com
*                        淘宝店:http://52edk.taobao.com
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : device_key.c
*作用       : 按键设备
*原理       : 基于SystemTick 100/秒 10mS扫描获取按键值，按键值分为长按与短按两种
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "drive.h"
#include "system.h"

#define PinBeep PaOut->Bit1

#define PinX0	PcIn->Bit10
#define PinX1	PcIn->Bit11
#define PinX2	PcIn->Bit12
#define PinX3	PbIn->Bit3

#define PinY0	PbOut->Bit4
#define PinY1	PbOut->Bit5

#define ShortInterval       30		// 短按间隔
#define LongInterval        150		// 长按间隔

static byte Scan = invalid;
static int ScanCounter = 0;
static bool Enable = false;

static byte RemapKey(byte scan) 
{
    switch(scan)
    {
        case 0xEF:  return(0);           
        case 0xDF:  return(1);         
        case 0xBF:  return(2);            
        case 0x7F:  return(3);       
        case 0xFE:  return(4);         
        case 0xFD:  return(5);       
        case 0xFB:  return(6);   
        case 0xF7:  return(7);
        default:    return(invalid);  
    }
}

static byte RemapLongKey(byte scan) 
{
    switch(scan)
    {
        case 0xEF:  return(0x30);
        case 0xDF:  return(0x31);
        case 0xBF:  return(0x32);
        case 0x7F:  return(0x33);
        case 0xFE:  return(0x34);
        case 0xFD:  return(0x35);
        case 0xFB:  return(0x36);
        case 0xF7:  return(0x37);
        default:    return(invalid);
    }	
}

static void EnableKey(bool status)
{
    Enable = status;
}

void KeySystick1000Routine(void) 
{
    static byte Key;
    static int StateCounter = 0;
    
    if (!Enable) return;
    
    if (++StateCounter == 3) StateCounter = 0;
    switch (StateCounter)
    {
        case 0:
            Key = invalid;
            if(PinX3 == 0)  Key &= 0x7F;
            if(PinX2 == 0)  Key &= 0xBF;
            if(PinX1 == 0)  Key &= 0xDF;
            if(PinX0 == 0)  Key &= 0xEF;
            PinY0 = 0;
            PinY1 = 1;
            break;
            
        case 1:
            if(PinX3 == 0)  Key &= 0xF7;
            if(PinX2 == 0)  Key &= 0xFB;
            if(PinX1 == 0)  Key &= 0xFD;
            if(PinX0 == 0)  Key &= 0xFE;
            PinY1 = 0;
            PinY0 = 1;
            break;

        case 2:
            if (Key != invalid && ScanCounter >= 0)
            {
                if (Key != Scan)
                {
                    Scan = Key;
                    ScanCounter = 0;
                }
                else
                {
                    if (++ScanCounter > 10000) ScanCounter = 10000;
                }
                    
            }
            else
            {
                
                if (ScanCounter > 0) ScanCounter--;
                if (ScanCounter < 0) ScanCounter++;
                if (ScanCounter == -1) PinBeep = 0;

                if (ScanCounter >= LongInterval) 
                { 
                    PostMessage(MessageKey, RemapLongKey(Scan));
                    goto A;
                }
                else if (ScanCounter >= ShortInterval) 
                {
                    PostMessage(MessageKey, RemapKey(Scan));
A:                  ScanCounter = -10;
                    Scan = invalid;
                    PinBeep = 1;
                }
            }
            break;
            
    }
}

void InitKey(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	PinY1 = 0;
	PinY0 = 1;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    PinBeep = 0;  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    System.Device.Key.Enable = EnableKey;
}


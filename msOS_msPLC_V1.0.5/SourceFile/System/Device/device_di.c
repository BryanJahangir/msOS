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
*文件名     : device_di.c
*作用       : PLC开关量输入接口，PLC标识为X端
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/07/17       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"


void DiSystick10000Routine(void)
{
    AppDataPointer->DI.X0 = *AppDataPointer->DI.pX0;
    AppDataPointer->DI.X1 = *AppDataPointer->DI.pX1;
    AppDataPointer->DI.X2 = *AppDataPointer->DI.pX2;
    AppDataPointer->DI.X3 = *AppDataPointer->DI.pX3;
}

void InitDI(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    AppDataPointer->DI.pX0 = (bool *)&PaIn->Bit4; 
    AppDataPointer->DI.pX1 = (bool *)&PaIn->Bit5;
    AppDataPointer->DI.pX2 = (bool *)&PaIn->Bit6;
    AppDataPointer->DI.pX3 = (bool *)&PaIn->Bit7;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


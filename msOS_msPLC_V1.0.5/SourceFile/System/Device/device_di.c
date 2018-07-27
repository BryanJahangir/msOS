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
*�ļ���     : device_di.c
*����       : PLC����������ӿڣ�PLC��ʶΪX��
*ԭ��       : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2015/07/17       ��ʼ�汾
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


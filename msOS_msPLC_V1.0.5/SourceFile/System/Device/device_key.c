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
*�ļ���     : device_key.c
*����       : �����豸
*ԭ��       : ����SystemTick 100/�� 10mSɨ���ȡ����ֵ������ֵ��Ϊ������̰�����
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
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

#define ShortInterval       30		// �̰����
#define LongInterval        150		// �������

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


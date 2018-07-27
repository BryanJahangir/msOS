/*
********************************************************************************
*                                ??????
*                                   msOS
*
*                            ????:msPLC DEMO
*                          ???:STM32F103R8T6/RBT6
*                           ???????????
*
*                                ??:???
*                                ??:???
*                                ??:Wangsw
*
*                                QQ:26033613
*                               QQ?:291235815
*                        ??:http://bbs.huayusoft.com
*                        ???:http://52edk.taobao.com
*                ??:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*???     : device_lcd.c
*??       : 128*64????????
*??       : 128*64?????,????,????????GB2312??,
*           : Keil???????GB2312,????16*16??,??????8???
*           : ????ASCII??,??8*16,??????16???
********************************************************************************
*??     ??            ??            ??
*V0.1    Wangsw        2013/07/21       ????
********************************************************************************
*/



#include "drive.h"
#include "system.h"


#define PinClk	    PcOut->Bit9
#define PinCs	    PcOut->Bit8
#define PinData	    PcOut->Bit7
#define PinReset	PcOut->Bit6

const char Array[4] = 
{
    0x80, 0x90, 0x88, 0x98
};
static void SendInstruct(byte instruct)
{
    short i;
    uint variable;

    variable = 0x00F80000 | ((instruct & 0xF0) << 8) |((instruct & 0x0F) << 4); 

    PinClk = 0;	
    PinCs = 1;
    
    for (i = 23; i > -1; i--) 
    {
        if(GetBit(variable, i)) 
        {
            PinData = 1;
        } 
        else 
        {
            PinData = 0;
        }
        DelayUs(10);     // 7不能正常工作，考虑余量，取10
        PinClk = 1;
        DelayUs(10);
        PinClk = 0;
        DelayUs(10);
    }
    PinCs = 0;
}

static void SendData(byte data)
{
    short i;
    uint variable;
    variable = 0x00FA0000 | ((data & 0xF0) << 8) |((data & 0x0F) << 4); 

    PinClk = 0;
    PinCs = 1;

    for (i = 23; i > -1; i--) 
    {
        if (GetBit(variable, i)) 
            PinData = 1;
        else 
            PinData = 0;
        DelayUs(10);
        PinClk = 1;
        DelayUs(10);
        PinClk = 0;
        DelayUs(10);
    }
    PinCs = 0;
}


/*******************************************************************************
* 描述	    : Lcd屏显示函数，显示一整行16个字母或者8个汉字，汉字必须要在偶数位置开始
* 输入参数  : y: 行号0、1、2、3
*           : string 字符串，必须要16个char，一个汉字等于两个char
*******************************************************************************/
static void LcdDisplayString(byte y, string string)
{ 
    byte i;
    	
    SendInstruct(Array[y]);

    for (i = 0; i < 16; i++)
        SendData(*string++);
}

#if 0
static void LcdDisplayGraphy(void)
{
    int i, j, k;
    
    SendInstruct(0x34);
    SendInstruct(0x36);

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 32; j++)
        {
            SendInstruct(0x80 + j);
            if (i == 0)
                SendInstruct(0x80);
            else
                SendInstruct(0x88);
        
            for (k = 0; k < 16; k++)
                SendData(0xFF);
        }
    }
    SendInstruct(0x34);
    SendInstruct(0x30);
}
#endif

void InitLcd(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    PinClk = 0;	PinCs = 0; PinData = 0; PinReset = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
    DelayMs(100);
    PinReset = 1;
    
    SendInstruct(0x30);
    SendInstruct(0x0C);
    SendInstruct(0x01);
    SendInstruct(0x02);
    SendInstruct(0x80);
    SendInstruct(0x01);
    System.Device.Lcd.DisplayString = LcdDisplayString;
}






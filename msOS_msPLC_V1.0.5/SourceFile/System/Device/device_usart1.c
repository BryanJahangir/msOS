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
*�ļ���     : device_usart1.c
*����       : �����豸
*ԭ��       : ���Ͳ���Buffer���ͣ����ղ���Appע�ắ����ʽ����
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
*V0.2    Wangsw        2013/09/13       ��һ�θ���
********************************************************************************
*/

#include "drive.h"
#include "system.h"

typedef struct 
{
    byte    * Start;            //ָ����п�ʼ 
    byte    * End;              //ָ����н��� 
    byte    * In;               //����һ����Ϣ  
    byte    * Out;              //ȡ��һ����Ϣ  
    volatile ushort    Entries;     //��Ϣ����      
}QueueStruct;

#define TxdBufferSum 1000
static QueueStruct TxdQueue;
static byte TxdBuffer[TxdBufferSum];

static void InitQueue(void)
{
    TxdQueue.Start = TxdBuffer;
    TxdQueue.End = TxdBuffer + TxdBufferSum - 1;
    TxdQueue.In = TxdQueue.Start;
    TxdQueue.Out = TxdQueue.Start;
    TxdQueue.Entries = 0;
}

/*******************************************************************************
* ����	    : ����1����һ���ֽ�
* �������  : data �ֽ����ͣ����Ͳ���
* ���ز���  : bool ����
*******************************************************************************/
static bool WriteByte(byte data) 
{
    if (TxdQueue.Entries >= TxdBufferSum) return (false);
    
    *TxdQueue.In++ = data;	
    
    if (TxdQueue.In > TxdQueue.End) 
        TxdQueue.In = TxdQueue.Start;

    EnterCritical();
    TxdQueue.Entries++;
    ExitCritical();
    return (true);
}

/*******************************************************************************
* ����	    : ����1����һ������
* �������  : dataPointer ����ָ�룬sum ��������
*******************************************************************************/
static void Write(byte * dataPointer, int sum)
{
    if (sum)
    {
        while (sum--) 
            WriteByte(*dataPointer++);
    }
    else
    {
        while (*dataPointer) 
            WriteByte(*dataPointer++);
    }
}

/*******************************************************************************
* ����	    : Keil������֧��printf��������Ҫ���ô˺�����ע�������ҪMicorLib֧��
* �������  : ch ʵΪbyte���ͣ�ʵ�ʷ��͵�����
*           : f �豸������Ҫ����
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    if (ch == '\n')                         //���з�
        WriteByte('\r');            //���ӻس���

    WriteByte((byte)ch);

    return ch;
}

/*******************************************************************************
* ����	    : ϵͳ����1000/S,��1mSһ�ε��ô˷�������ͻ���������
*******************************************************************************/
void Usart1Systick1000Routine(void) 
{
    if (TxdQueue.Entries == 0) return;

    if ((USART1->SR&0x80)==0) return; 

    USART1->DR = *TxdQueue.Out++;
    
    if (TxdQueue.Out > TxdQueue.End) 
        TxdQueue.Out = TxdQueue.Start;

    EnterCritical();
    TxdQueue.Entries--;
    ExitCritical();
}

/*******************************************************************************
* ����	    : �����жϴ�����
*******************************************************************************/
void USART1_IRQHandler(void) 
{
    byte data;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        data = (byte)USART_ReceiveData(USART1);
        
        PostMessage(MessageKey, data);
    } 
    
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
		USART_ReceiveData(USART1);
} 

static void Open(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static void Close(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART1, DISABLE);
}
     
void InitUsart1(void) 
{
//GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
//USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	
  
//USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //�����ж�
   
    USART_Cmd(USART1, ENABLE);                              //ʹ�ܴ��� 

    InitQueue();

    System.Device.Usart1.Open = Open;
    System.Device.Usart1.Close = Close;
    System.Device.Usart1.WriteByte = WriteByte;
    System.Device.Usart1.Write = Write;
}



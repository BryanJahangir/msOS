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
*�ļ���     : device_usart2.c
*����       : �����豸,����HMI��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2015/11/20       ��ʼ�汾
********************************************************************************
*/
#include "drive.h"
#include "system.h"

static void UsartDummy(byte data){;}
typedef void (*RxdFunction)(byte data);
static RxdFunction Rxd = UsartDummy;

static void Register(uint rxdFucntion)
{
    Rxd = (RxdFunction)rxdFucntion;
}
   
void USART2_IRQHandler(void)
{ 
    byte data;  
#if 0
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {    
        data = USART_ReceiveData(USART2);
        Rxd(data);
    }
    
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
		USART_ReceiveData(USART2);
#else  
    uint sr;
    sr = USART2->SR;


    if (sr & USART_FLAG_RXNE)
    {
        data = USART_ReceiveData(USART2);
        Rxd(data);
    }
    
    if (sr & USART_FLAG_ORE)
    {
        USART_ReceiveData(USART2);
    }
#endif 
}



static void Write(byte * dataPointer, int sum)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CMAR = (uint)dataPointer;
    DMA1_Channel7->CNDTR = sum;
    DMA_Cmd(DMA1_Channel7, ENABLE);  
}


void InitUsart2(void)
{
 
// GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;   
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
    
// USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
// USART2_RX   PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

// Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2

// DMA ��ʼ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    DMA_DeInit(DMA1_Channel7);  

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART2->DR));  
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;  

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStructure.DMA_BufferSize = 8;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel7,&DMA_InitStructure);  

// USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   //�����ж�
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
    USART_Cmd(USART2, ENABLE);                       //ʹ�ܴ���
  
    //DMA_Cmd(DMA1_Channel7, ENABLE); 

    System.Device.Usart2.Register = Register;
    System.Device.Usart2.Write = Write; 
}


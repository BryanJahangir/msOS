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
*�ļ���     : device_adc.c
*����       : ADC�豸
*ԭ��       : Adc�豸���ܿ飬����DMAʵ��ʵʱ���ݲ���
*           : ����ע�ắ���������������ݸ��ϲ�Ӧ�ñ���                  
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/09/01       ��ʼ�汾
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define Sum         5       // 4ͨ��
#define Depth       8       // ÿͨ����8��


static ushort DmaBuffer[Depth][Sum];
static ushort Adc[Sum - 1];

static ushort NullValue;
static ushort * RegisterPointerBlock[Sum];


static void InitRegisterBlock(void)
{
    int i;
    for (i = 0; i < Sum; i++)
        RegisterPointerBlock[i] = &NullValue;
}

/*******************************************************************************
* ����	    : ��Ӧ�ò����ָ��ע�ᵽ��Ӧ��ͨ�������У�ʵ�ֵײ������򶥲㴫��
* �������  : adcChannel: ADCͨ����
*           : dataPoint: Ӧ�ò����ָ��
*******************************************************************************/
static void Register(AdcChannelEnum channel, ushort * dataPointer)
{
    RegisterPointerBlock[channel] = dataPointer;
}

static void PortRegister(void)
{
    AppDataPointer->Adc.pA0 = &Adc[0];
    AppDataPointer->Adc.pA1 = &Adc[1];
    AppDataPointer->Adc.pA2 = &Adc[2];
    AppDataPointer->Adc.pA3 = &Adc[3];
}


/*******************************************************************************
* ����	    : ϵͳʱ��10000/S����0.1mS�������һ�����У���ȡADCֵ��App�����
*******************************************************************************/
void AdcSystick10000Routine(void)
{
    int i, j;
    uint acc;
    float temp;
    for (i = 0; i < Sum; i++)
    {
        acc = 0;
        for (j = 0; j < Depth; j++)
            acc = acc + DmaBuffer[j][i];
        
        acc= acc / Depth;
        if (i < 4)
        {   
            Adc[i] = acc;
            *RegisterPointerBlock[i] = acc;
        }
        else
        {
            temp = acc;
            temp = (1.43 - (temp / 4096 * 3.3)) / 0.0043 + 25;              // оƬ�¶ȹ�ʽ
            *RegisterPointerBlock[i] = temp;                                // �����Դﵽ45��
        }
    }
    AppDataPointer->Adc.A0 = Adc[0];
    AppDataPointer->Adc.A1 = Adc[1];
    AppDataPointer->Adc.A2 = Adc[2];
    AppDataPointer->Adc.A3 = Adc[3];
}

void InitAdc(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // IO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // ADC
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                       //12MHz
    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;            
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = Sum;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 5 , ADC_SampleTime_28Cycles5);   // оƬ�¶�
    ADC_TempSensorVrefintCmd(ENABLE);                                               // �¶�ʹ��

    ADC_DMACmd(ADC1, ENABLE);	                                            //ADC DMA Enable

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);

    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);

    while(ADC_GetCalibrationStatus(ADC1));
    
    // DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 
	DMA_DeInit(DMA1_Channel1);		  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint)(&(ADC1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint)DmaBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = Depth * Sum; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);              

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    InitRegisterBlock();
    PortRegister();
    System.Device.Adc.Register = Register;
}


	

	
	 


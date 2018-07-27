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
*�ļ���     : modbus_slave.c
*����       : ��չHMI��
*ԭ��       : ͨ����׼ModbusЭ����չ��ҵ��HMI��
*           : ͨ��Modbus����PLC����������ַ
*           : ͨ���ĸ����飬�������ţ�Ҳ��HMI�����ݵ�ַ��������ָ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/11/8       ��ʼ�汾
********************************************************************************
*/

#include "system.h"

#define DeviceID                    1
#define RxdTimeoutSum               2

#define TxdBufferSum                256
#define RxdBufferSum                256
#define DataPointer                 ((DataStruct *)0)

//************************************************************************************
//��HMI�������ݣ�����������Ӧ�ĵ�ַ
static const bool * DoBlock[] =                // DataOutPort
{
    &DataPointer->DO.Y0, &DataPointer->DO.Y1, &DataPointer->DO.Y2, &DataPointer->DO.Y3, &DataPointer->DO.Y4, &DataPointer->DO.Y5
};

static const bool * DiBlock[] =                // DataInPort
{
    &DataPointer->DI.X0, &DataPointer->DI.X1, &DataPointer->DI.X2, &DataPointer->DI.X3
};
static const ushort * AdcBlock[] =              // AdcInPort
{
    &DataPointer->Adc.A0, &DataPointer->Adc.A1, &DataPointer->Adc.A2, &DataPointer->Adc.A3
};

static const ushort * RegBlock[] =              // Register
{
    (ushort *)(&DataPointer->Frequency), (ushort *)(&(DataPointer->Frequency)) + 1,
    (ushort *)(&(DataPointer->Voltage)), (ushort *)(&(DataPointer->Current))
};
//************************************************************************************

static byte TxdBuffer[TxdBufferSum];    // ����֡Buffer
static byte RxdBuffer[RxdBufferSum];    // ����֡Buffer

static bool RxdState;                   // ����״̬
static ushort RxdCounter;               // ���ռ���
static ushort RxdTimeout;               // ���ճ�ʱ��֡
#define GetBool(nodePointer) *(bool *)((uint)AppDataPointer + (uint)nodePointer)
#define GetUshort(nodePointer) *(ushort *)((uint)AppDataPointer + (uint)nodePointer)
#define UsartWrite(pointer, sum) System.Device.Usart2.Write(pointer, sum)
/*************************************************************************************
* MODBUS������Ӧ����
* ���룺���ݿ�ʼ��ַ�����ȣ���1���ǣ�0����֡�Ŀ�ʼ��
* ���û����ã�Ҫȷ��USART�ķ�������ж���ʹ�ܵģ�
*************************************************************************************/
static void Response(byte *bufferPointer, int sum)
{
    ushort crc;
    byte * pointer;

    crc = Crc16(bufferPointer, sum);

    pointer = bufferPointer + sum;
    
    *pointer++ = Byte1(crc);
    *pointer = Byte0(crc);
    sum = sum + 2;
    UsartWrite(bufferPointer, sum);
}

/*************************************************************************************
* ��������˿ڶ�ȡ
*************************************************************************************/
static void ReadDo(ushort address)
{
    int i;
    byte byteSum, packetSum;
    ushort bitSum;
    byte * pointer;

    TxdBuffer[0] = RxdBuffer[0];      //֡��ַ
    TxdBuffer[1] = RxdBuffer[1];      //������
    
    Byte1(bitSum) = RxdBuffer[4];  
    Byte0(bitSum) = RxdBuffer[5];  //����˿���

    
    if (bitSum & 0x07)                  //���˿����Ƿ���8�ı���
        byteSum = (bitSum >> 3) + 1;    //ȷ���ֽ���
    else
        byteSum = bitSum >> 3;
    
    TxdBuffer[2] = byteSum;            //�ֽ���

    pointer = &TxdBuffer[3];           //����շ�������
    for (i = 0; i < byteSum; i++)       
        pointer[i] = false;
    
    for (i = 0; i < bitSum; i++)        //д��˿���Ϣ
    {
        if (GetBool(DoBlock[address + i]) == true) //��������ָ������
            pointer[i >> 3] |= 1 << (i & 7);
    }

    packetSum = TxdBuffer[2] + 3;

    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* ��������˿ڶ�ȡ
*************************************************************************************/
static void ReadDi(ushort address)
{
    int i, packetSum;
    ushort bitSum, byteSum;
    byte * pointer;

    TxdBuffer[0] = RxdBuffer[0];      //֡��ַ
    TxdBuffer[1] = RxdBuffer[1];      //������
    
    Byte1(bitSum) = RxdBuffer[4];
    Byte0(bitSum) = RxdBuffer[5];

    
    if (bitSum & 0x07)
        byteSum = (bitSum >> 3) + 1;
    else
        byteSum = bitSum >> 3;
    
    TxdBuffer[2] = byteSum;            //�ֽ���

    pointer = &TxdBuffer[3];           //������������
    for (i = 0; i < byteSum; i++)       
        pointer[i] = false;

    for (i = 0; i < bitSum; i++)        //д��bit��Ϣ,msOS�д洢bit��Ϣ����Byte�������洢
    {
        if (GetBool(DiBlock[address + i]) == true)
            pointer[i >> 3] |= 1 << (i & 7);
    }

    packetSum = TxdBuffer[2] + 3;

    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* Adc���봦��
*************************************************************************************/
static void ReadAdc(ushort address)
{
    int i, packetSum;
    ushort registerSum, data;
    byte * pointer;
    
    TxdBuffer[0] = RxdBuffer[0];      //֡��ַ
    TxdBuffer[1] = RxdBuffer[1];      //������
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];
    
    TxdBuffer[2] = registerSum << 1;   //�ֽ��� registerSum <= 0x7D��register��ushort����

    pointer = &TxdBuffer[3];
    for (i = 0; i < registerSum; i++)
    {
        data = GetUshort(AdcBlock[address++]);
        *pointer++ = Byte1(data);
        *pointer++ = Byte0(data);
    }

    packetSum = TxdBuffer[2] + 3;
    
    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* �Ĵ�����������Ӧ
*************************************************************************************/
static void ReadReg(ushort address)
{
    int i, packetSum;
    ushort registerSum, data;
    byte * pointer;
    
    TxdBuffer[0] = RxdBuffer[0];      //֡��ַ
    TxdBuffer[1] = RxdBuffer[1];      //������
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];
    
    TxdBuffer[2] = registerSum << 1;   //�ֽ��� registerSum <= 0x7D��register��ushort����

    pointer = &TxdBuffer[3];
    for (i = 0; i < registerSum; i++)
    {
        data = GetUshort(RegBlock[address++]);
        *pointer++ = Byte1(data);
        *pointer++ = Byte0(data);
    }

    packetSum = TxdBuffer[2] + 3;
    
    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* ��Ȧ����д����
*************************************************************************************/
static void WriteOneDo(ushort address)
{
    ushort bitFlag;

    Byte1(bitFlag) = RxdBuffer[4];
    Byte0(bitFlag) = RxdBuffer[5];
        
    if (bitFlag == 0xFF00)                                  //true:0xFF00
        GetBool(DoBlock[address]) = true;
    else                                                    //false:0x0000
        GetBool(DoBlock[address]) = false;
    
    UsartWrite(RxdBuffer, RxdCounter);    //Ӧ��֡��������֡��ͬ
}

/*************************************************************************************
* ��Ȧ���д����������Ӧ
* Э�����������������ֻ��д8bit���ݵ�ȱ�ݣ����޸���Э������������⡣
*************************************************************************************/
static void WriteDo(ushort address)
{
    int i;
    ushort bitSum;
    byte * pointer;
    
    Byte1(bitSum) = RxdBuffer[4];
    Byte0(bitSum) = RxdBuffer[5];

    pointer = &RxdBuffer[7];
    for (i = 0; i < bitSum; i++)
        GetBool(DoBlock[address + i]) = GetBit(pointer[i >> 3], i & 0x07);

    memcpy(TxdBuffer, RxdBuffer, 7);

    Response(TxdBuffer, 6);
}

/*************************************************************************************
* �Ĵ�������д����������Ӧ
*************************************************************************************/
static void WriteOneReg(ushort address)
{
    ushort data;

    Byte1(data) = RxdBuffer[4];
    Byte0(data) = RxdBuffer[5];

    GetUshort(RegBlock[address]) = data;
    
    UsartWrite(RxdBuffer, RxdCounter);
}

/*************************************************************************************
*
* ��Ĵ���д����������Ӧ
*
*************************************************************************************/
static void WriteReg(ushort address)
{
    int i;
    ushort registerSum, data;
    byte * pointer;
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];

    pointer = &RxdBuffer[7];                   //ǰ�滹��һ���ֽ�������6λ��
    for (i = 0; i < registerSum; i++)
    {
        Byte1(data) = *pointer++;
        Byte0(data) = *pointer++;

        GetUshort(RegBlock[address++]) = data;
    }
    
    memcpy(TxdBuffer, RxdBuffer, 6);

    Response(TxdBuffer, 6);
}



/************************************************************************************
* ���ݽ��ո�λ
*************************************************************************************/
static void Reset()
{
    RxdCounter = 0;
    RxdTimeout = 0;
    RxdState = no;
}

/*************************************************************************************
* ���ջ���
*************************************************************************************/
static void Receive(byte data)
{
    if(RxdCounter >= RxdBufferSum)
        RxdCounter = 0;
		
    RxdBuffer[RxdCounter++] = data;

    RxdTimeout = 0;//1 
    RxdState = yes;
}

/*************************************************************************************
* MODBUS����֡����:�������
* ���룺ָʾ�Ѿ����յ�һ֡���ݹ��Ӻ���ָ�룬��Ӧָʾ���Ӻ���ָ��
* ������ѭ������
*************************************************************************************/
static void ParseRxdFrame(void)
{
    ushort address, crcCalculate , crcReceive;

    if (RxdBuffer[0] != DeviceID)
    {
        Reset(); return;
    }
        
    if (RxdCounter <= 4)
    {
        Reset(); return;
    }
        
    crcCalculate = Crc16(RxdBuffer, RxdCounter - 2);
    Byte1(crcReceive) = RxdBuffer[RxdCounter - 2];
    Byte0(crcReceive) = RxdBuffer[RxdCounter - 1];
    
    if (crcCalculate != crcReceive) {Reset(); return;}//У��
    
    Byte1(address) = RxdBuffer[2];  //��ȡ��λ��ַ
    Byte0(address) = RxdBuffer[3];  //��ȡ��λ��ַ
    
    switch(RxdBuffer[1])            //ʶ��֧�ֵĹ�����
    {
        case ReadDo0x:
            ReadDo(address);
            break;
        case ReadDi1x:
            ReadDi(address);
            break;
        case ReadAdc3x:
            ReadAdc(address);
            break;
        case ReadReg4x:
            ReadReg(address);
            break;
        case WriteOneDo0x:
            WriteOneDo(address);
            break;
        case WriteOneReg4x:
            WriteOneReg(address);
            break;
        case WriteDo0x:
            WriteDo(address);
            break;
        case WriteReg4x:
            WriteReg(address);
            break;
        default:
            break;
    }

    Reset();
}



/*************************************************************************************
* MODBUS���ջ�������ʱ����
* ÿ0.1MS������һ��
* ����ȷ��MODBUS������һ֡����
*************************************************************************************/
static void SystickRoutine(void)
{
    if (RxdState == yes)
    {
        RxdTimeout++;
        if (RxdTimeout > RxdTimeoutSum)
        {
            RxdState = no;
            ParseRxdFrame();
        }
    }
}


/************************************************************************************
* MODBUS��ʼ��
*************************************************************************************/
void InitSlaveModbus(void)
{
    Reset();
    
    System.Device.Systick.Register(Systick10000, SystickRoutine);    

    System.Device.Usart2.Register((uint)Receive);
}
 




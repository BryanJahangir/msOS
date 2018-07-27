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
*�ļ���     : device_storage.c
*����       : �洢�豸(�����洢����־�洢)
*ԭ��       : STM32F103R8ϵ��flashΪ64*1KByte,���ٲ�����ԪΪ1KByte
*           : STM32F103RBϵ��flashΪ128*1KByte,���ٲ�����ԪΪ1KByte
*           : storage�����洢����׷��ʽ�洢��ʽ���洢��ʽΪ[Address][Data]��
*           : �������ݵĵ�ַ��������Ϊһ������洢��ȥ���������б仯��ʱ��
*           : �ں���׷�Ӵ洢���ɡ�����ʱ���������Ч�����ݡ�
*           : ��־�洢ֱ�Ӵ洢д�����ݼ��ɣ�����׷�ӷ�ʽ��ע�⣬�����洢��
*           : ��־�洢�ռ�λ�ò�ͬ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/08/23       ��ʼ�汾      
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define PageSize                1024 //Stm32F103R8T6 1Page=1024Byte

#define ParameterAddress        (FLASH_BASE + (63 * 1024)) 
#define ParameterSpace          PageSize / 4 / 2

#define LogAddress              (FLASH_BASE + (60 * 1024)) 
#define LogSpace                PageSize * 3

typedef struct
{
    uint Address;               //�����洢��Ԫ��ַ
    uint Data;                  //�����洢��Ԫ����
}CellStruct;

typedef struct
{
    ushort Entries;             //�����������
    ushort Size;                //���ò����ռ�
    ushort Space;               //�ܲ����ռ�
    ushort WritePonit;          //д���
}ParameterStruct;

typedef struct
{
    char * ReadPointer;         //��ȡ��
    char * WritePointer;        //д���
    ushort Size;                //������־�ռ�
    ushort Space;               //����־�ռ�
}LogStruct;

static CellStruct * CellBlock = (CellStruct *)ParameterAddress;

static ParameterStruct Parameter; 

static LogStruct Log;

/*******************************************************************************
* ����	    : Flash���򿪣�����д�룬��������
*******************************************************************************/
static void Open(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

/*******************************************************************************
* ����	    : Flash���رգ�������д�룬��������
*******************************************************************************/
static void Close(void)
{
    FLASH_Lock();
}

/*******************************************************************************
* ����	    : ������ȡ�����洢��Ϣ�����ݿ��У�������ݿ⣬�����������ݣ���ʡ�洢�ռ�
*******************************************************************************/
static void ReadAllParameter(void)
{
    bool CleanFlag;
    int i, j;
    CellStruct cell;
    uint AddressBlock[ParameterSpace];
    
    for (i = 0; i < ParameterSpace; i++)
    {
        if (CellBlock[i].Address != 0xFFFFFFFF)
        {
            AddressBlock[i] = CellBlock[i].Address;             // ����ҳ�浽��ʱ������
            pUint(AddressBlock[i]) = CellBlock[i].Data;   // ��ʼ�����ݿ�����
        }
        else
            break;
    }
    
    Parameter.Size = i;
    Parameter.Space = ParameterSpace;                           
    if (Parameter.Size < 2)                                     // ����Ƿ�����Ч��Ԫ���Ƿ���Ҫ����
        Parameter.Entries = Parameter.Size;
    else
    {
        CleanFlag = false;
    
        for (i = 0; i < Parameter.Size - 1; i++)
        {
            for (j = i + 1; j < Parameter.Size; j++)            // ����Ч��Ԫ��ַ��գ���ԪʧЧ
            {
                if (AddressBlock[i] == AddressBlock[j])
                {
                    AddressBlock[i] = 0;            
                    CleanFlag = true;
                    break;
                }   
            }     
        }
        
        if (CleanFlag == false)                                 // ������Ч��Ԫ
            Parameter.Entries = Parameter.Size;
        else
        {
            Open();
            FLASH_ErasePage(ParameterAddress);                  // ����ҳ��

            j = 0;
            for (i = 0; i < Parameter.Size; i++)                // ��д��Ԫ
            {
                if (AddressBlock[i] == 0) continue;
                
                cell.Address = AddressBlock[i];
                cell.Data = pUint(AddressBlock[i]);
                
                FLASH_ProgramWord((uint)(&CellBlock[j].Address), cell.Address);
                FLASH_ProgramWord((uint)(&CellBlock[j].Data), cell.Data);
                j++;
            }
            Close();
            Parameter.Entries = j;
        }
    }

    Parameter.WritePonit = Parameter.Entries;
    
    printf("\n\n\n");
    printf("\n�����ܴ洢�ռ� = %d", Parameter.Space);
    printf("\n�����Ѵ洢�ռ�  = %d", Parameter.Size);
    printf("\n�����洢���� = %d", Parameter.Entries);
    printf("\n����д��� = %d", Parameter.WritePonit);
}


/*******************************************************************************
* ����	    : �洢���ݣ��������ݵ�ָ�����洢���ݡ�
* �������  : ���ݵ�ָ��
* ���ز���  : bool����
*******************************************************************************/
static bool WriteParameter(void * dataPointer)
{
    uint * pointer;
    pointer = (uint *)dataPointer;
    
    if (dataPointer == (uint *)0) return (false);

    if (Parameter.WritePonit == Parameter.Space) return(false);

    Open();
    FLASH_ProgramWord((uint)(&CellBlock[Parameter.WritePonit].Address), (uint)pointer);
    FLASH_ProgramWord((uint)(&CellBlock[Parameter.WritePonit].Data), *pointer);
    Close();
    Parameter.WritePonit++;
    return (true);
}

/*******************************************************************************
* ����	    : ��ȡ��־�洢��Ϣ
*******************************************************************************/
static void ReadLogProfile(void)
{
    int i;
    char * readPointer;
    Log.Space = LogSpace;
    Log.ReadPointer = (char *)LogAddress;
    Log.WritePointer = Log.ReadPointer;
    Log.Size = 0;
    readPointer = (char *)LogAddress;
    
    for (i = 0; i < LogSpace; i++)
    {
        if (*readPointer == 0xFF) 
        {
            Log.WritePointer = readPointer;
            Log.Size = i;        
            break;
        }
        readPointer++;
    }
    printf("\n\n");
    printf("\n��־�ܴ洢�ռ� = %d", Log.Space);
    printf("\n��־�Ѵ洢�ռ�  = %d", Log.Size);
    printf("\n��־��ȡ�� = %x", (uint)(Log.ReadPointer));
    printf("\n��־д��� = %x", (uint)(Log.WritePointer));
    printf("\n\n");
}


/*******************************************************************************
* ����	    : �����־��Ϣ
*******************************************************************************/
static void EraseLog(void)
{
    int i;
    
    Open();
    
    for (i = 0; i < (LogSpace / PageSize); i++)
        FLASH_ErasePage(LogAddress + (i * PageSize));

    Close();
    Log.ReadPointer = (char *)LogAddress;
    Log.WritePointer = (char *)LogAddress;
    Log.Size = 0;
}
    

/*******************************************************************************
* ����	    : ��ȡ��־�洢��������¼��ȡ
* �������  : sequence: -1: ���ַ���һ��һ����ȡ��ѭ����
*           :            0: ��ʼ����ȡ��ַΪ�洢����ͷ�����ؿ�ͷ��ַ����ȡ��һ��
*           :           >0: ��ȡ��N����¼
* ���ز���  : �ַ���ָ��
*******************************************************************************/
static char * ReadLog(int sequence)
{
    char * pointer;
    char * returnPointer;
    
    pointer = (char *)LogAddress;
    if (*pointer == 0xFF) return ("Null\n");        // ����Ƿ�Ϊ��ȡ�յ�
    
    if (sequence == 0)                              // ��ȡ��0����¼
    {
        Log.ReadPointer = (char *)LogAddress;
        return((char *)LogAddress);                 // �����ַ����׵�ַ
    }
    else if (sequence == -1)                        // �����0��ʼѭ����ȡ
    {
        returnPointer = Log.ReadPointer;
        pointer = Log.ReadPointer;

        while(*pointer++ > 0x00); 
            
        if (*pointer == 0x00) pointer++;            // ���ִ洢����������������0
        
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            Log.ReadPointer = (char *)LogAddress;
        else
            Log.ReadPointer = pointer;
        
        return (returnPointer);
    }
    else if (sequence > 0)                          // Ѱ�����λ�ü�¼
    {
        while(sequence--)
        {
            if(*pointer == 0xFF) return ("Null\n");
            
            while(*pointer > 0x00); pointer++;
            while(*pointer == 0x00); pointer++;
        }
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            return ("Null\n");
        else
            return (pointer);
    }
    return ("ArgumentError\n");
}

/*******************************************************************************
* ����	    : д����־��Ϣ�����ַ�����ʽд�롣��־�����õ��˽ϴ��ջRam����Ҫ��ҵ���߼���ִ�С�
* �������  : �ַ���
* ���ز���  : bool����
*******************************************************************************/
static bool WriteLog(char *fmt, ...)
{
    int i, j, len;
    ushort * pointer;
    va_list list;
    char buffer[100];
    
    pointer = (ushort *)buffer;
    for (i = 0; i < 50; i++) *pointer++ = 0x0000;   // ��ʼ����ʱ���飬ֵΪ0
    
    va_start(list, fmt);                            // ��vsprintf�ַ���ת���������ַ�����ӡ��������
    len = vsprintf(buffer, fmt, list);
    va_end(list);

    if (len == 0) return (false);                   // �жϳ��Ȳ�Ϊ��
    
    len++;                                          // �����ַ�������������
    if (len & 0x01) len++;                          // ����ִ洢��Ҫ��ż�����ݳ��ȴ洢
    
    if (Log.Size + len >= LogSpace) return (false); // �жϿռ��Ƿ�Ϊ��
    j = len >> 1;                                   
    pointer = (ushort *)buffer;
    Open();
    for (i = 0; i < j; i++)                         // ������д
    {
        FLASH_ProgramHalfWord((uint)(Log.WritePointer), *pointer);
        Log.WritePointer += 2;
        pointer++;
    }
    Close();
    
    Log.Size += len; 
    return (true);
}

void InitStorage(void)
{
    ReadAllParameter();
    System.Device.Storage.Parameter.Write = WriteParameter;

    ReadLogProfile();
    System.Device.Storage.Log.Erase = EraseLog;
    System.Device.Storage.Log.Read = ReadLog;
    System.Device.Storage.Log.Write = WriteLog;
}



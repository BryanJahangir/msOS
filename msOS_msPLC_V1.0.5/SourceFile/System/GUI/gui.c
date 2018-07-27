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
*�ļ���     : gui.c
*����       : ͼ�ο��ļ�
*ԭ��       : ���������ƣ���һ��Menu��Ϊ���Form,һ��Form��������ؼ�����Chart��
*           : Label��TextBox�ȣ�����һ��������ȼ�����MenuTaskȥ����ִ����Щ�ؼ���Ϣ
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/

#include "system.h"

#define MessageSum 8

static char VirtualLcd[4][16];
static char GuiLcd[4][16];
static Message MessageBuffer[MessageSum];

static Form * FocusFormPointer;

/*******************************************************************************
* ����	    : ��ǩ��������
* �������  : labelPointer ��ǩָ��
* ˵��      : sprintf����֧��int��double����,�����������ͣ�������ת��Ϊ������
*******************************************************************************/
static void LabelToGuiLcd(Label * labelPointer)
{
    byte len, x, y;
    byte u8;
    short s16;
    ushort u16;
    int s32;
    uint u32;
    float f32;
    string string;
    
    char format[5];
    char buffer[20];

    x = labelPointer->X;
    y = labelPointer->Y;
        
    switch(labelPointer->Type)
    {
        case GuiDataTypeByteDec:
            u8 = *((byte *)(labelPointer->DataPointer));
            s32 = ((float)u8 + labelPointer->Offset) * labelPointer->Coefficient;
            
            len = sprintf(buffer, "%d", s32);
            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;
            
        case GuiDataTypeShortDec:
            s16 = *((short *)(labelPointer->DataPointer));
            s32 = ((float)s16 + labelPointer->Offset) * labelPointer->Coefficient;
            
            len = sprintf(buffer, "%d", s32);
            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;
            
        case GuiDataTypeUshortDec:
            u16 = *((ushort *)(labelPointer->DataPointer));
            s32 = ((float)u16 + labelPointer->Offset) * labelPointer->Coefficient;

            len = sprintf(buffer, "%d", s32);
            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;
            
        case GuiDataTypeIntDec:                
            s32 = *((int *)(labelPointer->DataPointer));
            s32 = ((double)s32 + (double)labelPointer->Offset) * (double)labelPointer->Coefficient;

            len = sprintf(buffer, "%d", s32);
            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;
            
        case GuiDataTypeUintDec:
            u32 = *((uint *)(labelPointer->DataPointer));
            u32 = ((double)u32 + (double)labelPointer->Offset) * (double)labelPointer->Coefficient;

            len = sprintf(buffer, "%u", u32);
            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;

        case GuiDataTypeFloatDec:
            f32 = *((float *)(labelPointer->DataPointer));
            f32 = (f32 + labelPointer->Offset) * labelPointer->Coefficient;            
            len = labelPointer->Digits;
            if (len)      
            {
                memcpy(format, "%.nf", 5); format[2] = HexToAscii(len);
                len = sprintf(buffer, format, (double)f32);
            }
            else
                len = sprintf(buffer, "%d", (int)f32);

            while(len--) {GuiLcd[y][x] = buffer[len]; x--;}
            break;
        case GuiDataTypeUshortHex:
            u16 = *((ushort *)(labelPointer->DataPointer));
            u16 = ((float)u16 + labelPointer->Offset) * labelPointer->Coefficient;
            len = labelPointer->Digits;
            if (len)
            {
                while(len--)
                {
                    GuiLcd[y][x] = HexToAscii(u16 & 0x000F);
                    u16 = u16 >> 4; if(x) x--;
                }
            }
            else
            {
                GuiLcd[y][x] = '0';
                while(u16)
                {
                    GuiLcd[y][x] = HexToAscii(u16 & 0x000F);
                    u16 = u16 >> 4; if(x) x--;
                }
            }
            break;   
        case GuiDataTypeUintHex:
            u32 = *((uint *)(labelPointer->DataPointer));
            u32 = ((double)u32 + (double)labelPointer->Offset) * (double)labelPointer->Coefficient;
            len = labelPointer->Digits;
            if (len)
            {
                while(len--)
                {
                    GuiLcd[y][x] = HexToAscii(u32 & 0x0000000F);
                    u32 = u32 >> 4; if(x) x--;
                }
            }
            else
            {
                GuiLcd[y][x] = '0';
                while(u32)
                {
                    GuiLcd[y][x] = HexToAscii(u32 & 0x0000000F);
                    u32 = u32 >> 4; if(x) x--;
                }
            }
            break;
            
        case GuiDataTypeString:
            string = *(char **)labelPointer->DataPointer;
            len = strlen(string);
            if(labelPointer->Align == GuiDataAlignRight) x = x + 1 - len;
            while(len--)
            {
                GuiLcd[y][x] = *string++;
                if(x < 15) x++;
            }
            break;

        case GuiDataTypeSnString:
            u32 = *((byte *)(labelPointer->DataPointer));
            string = labelPointer->StringBlockPointer[u32];
            len = strlen(string);
            if(labelPointer->Align == GuiDataAlignRight) x = x + 1 - len;
            while(len--)
            {
                GuiLcd[y][x] = *string++;
                if(x < 15) x++;
            }
            break;
    }
}

/*******************************************************************************
* ����	    : ��������ɨ��ˢ��
* �������  : formPointer ��ǰ����ָ��
*******************************************************************************/
static void ParseBackText(void)
{
    const byte * backTextPointer;
    backTextPointer = FocusFormPointer->BackTextPointer;
    
    if(backTextPointer == null)
        memset(GuiLcd, ' ', 64);  
    else
        memcpy(GuiLcd, backTextPointer, 64);
}


/*******************************************************************************
* ����	    : Chartͼ�α�ؼ�ɨ��ˢ��
* �������  : formPointer ��ǰ����ָ��
*******************************************************************************/
static void ParseChart(void)
{
    byte i, j;
    char character;
    byte column;
    Chart * chartPointer;

    chartPointer = FocusFormPointer->ChartPointer;
    if(chartPointer == null) return;
        
    character = chartPointer->Character;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 16; j++)
        {
            column = chartPointer->Column[j];
            Assert(column < 5);
            
            if(column >= (4 - i))
                GuiLcd[i][j] = character;
            else
                GuiLcd[i][j] = ' ';
        }
    }
}

/*******************************************************************************
* ����	    : Label�ؼ�ɨ��ˢ�£�Labelһ�������ı���ʾ�������ܰ����������
* �������  : formPointer ��ǰ����ָ��
*******************************************************************************/
static void ParseLabel(void)
{
    Label * labelPointer;
    
    labelPointer = FocusFormPointer->LabelPointer;

    while(labelPointer != null)
    {
        LabelToGuiLcd(labelPointer);
        labelPointer = labelPointer->NextLabelPointer;
    }
}

/*******************************************************************************
* ����	    : TextBox�ؼ�ɨ��ˢ�£�TextBox���������ı���ʾ�����ܰ��������޸Ĳ���
* �������  : formPointer ��ǰ����ָ��
*******************************************************************************/
static void ParseTextBox(void)
{
    static byte Counter;
    
    TextBox * textBoxPointer;

    textBoxPointer = FocusFormPointer->TextBoxPointer;

    while(textBoxPointer != null)
    {
        if (FocusFormPointer->FocusTextBoxPointer == textBoxPointer)
        {
            if(++Counter == 8) Counter = 0;
            if (Counter < 4)
            {
                textBoxPointer = textBoxPointer->NextTextBoxPointer;
                continue;
            }
        }
        LabelToGuiLcd((Label *)textBoxPointer);
        
        textBoxPointer = textBoxPointer->NextTextBoxPointer;
    }
}
static void ParseMessage(void)
{
    int i;
    Message * pointer;
    pointer = &MessageBuffer[0];
    for (i = 0; i < MessageSum; i++)
    {
        if (pointer->Valid)
        {
            memcpy(&GuiLcd[pointer->Y][pointer->X], pointer->Buffer, pointer->Len);
        }
        pointer++;
    }
}

/*******************************************************************************
* ����	    : LCD�����ݸ��£�������ʾ�����б仯�����и���LCD������
*******************************************************************************/
static void Update(void)
{
#ifdef LCD12864
    byte i, j;
    bool update;
    for (i = 0; i < 4; i++)
	{
        update = false;
        
        for (j = 0; j < 16; j++)
        {
            if(VirtualLcd[i][j] != GuiLcd[i][j])
            {
                VirtualLcd[i][j] = GuiLcd[i][j];
                update = true;
            }
        }
        if (update)
            System.Device.Lcd.DisplayString(i, VirtualLcd[i]);
    }
    
#endif
#ifdef msMenu
    byte i;
    bool update;
    
    update = false;
    for (i = 0; i < 64; i++)
	{
        if(*((char *)VirtualLcd + i) != *((char *)GuiLcd + i))
        {
            *((char *)VirtualLcd + i) = *((char *)GuiLcd + i);
            update = true;
        }
    }
    if (update == true)
    {
        System.Device.Usart1.WriteByte(0x02);
        for (i = 0; i < 64; i+=16)
        {
            System.Device.Usart1.Write((byte *)VirtualLcd + i, 16);
            System.Device.Usart1.WriteByte(0x0D);
            System.Device.Usart1.WriteByte(0x0A);
        }
        System.Device.Usart1.WriteByte(0x03);
    }
#endif
}

/*******************************************************************************
* ����	    : ����ҳ��
*******************************************************************************/
static void Parse(Form * formPointer)
{  
    FocusFormPointer = formPointer;
    
    ParseBackText();            //�������ֿؼ�ɨ�����

    ParseChart();               //��ؼ�ɨ�����

    ParseLabel();               //��ǩ�ؼ�ɨ�����

    ParseTextBox();             //�ı��ؼ�ɨ�����
    ParseMessage();

    Update();                   //����LCD��ʾ����
}

/*******************************************************************************
* ����	    : ��ʼ��ҳ��
* �������  : formPointer :����Formָ��
*******************************************************************************/
static void InitForm(Form *formPointer)
{
    formPointer->BackTextPointer = null;
    formPointer->ChartPointer = null;
    formPointer->LabelPointer = null;
    formPointer->TextBoxPointer = null;
    formPointer->FocusTextBoxPointer = null;
}

/*******************************************************************************
* ����	    : ����Form����Label������ʱ���Label���ݣ����غ���Ҫ��Label��ʼ������
* �������  : formPointer :����Formָ��
*           : labelPointer: �ؼ�Labelָ��
*******************************************************************************/
static void AddLabel(Form * formPointer, Label *labelPointer)
{
    labelPointer->DataPointer = null;
    
    labelPointer->Digits= 0;
    labelPointer->Offset = 0;
    labelPointer->Coefficient = 1;
    
    labelPointer->StringBlockPointer = null;
    labelPointer->Align = GuiDataAlignRight;
    labelPointer->NextLabelPointer = formPointer->LabelPointer;
    formPointer->LabelPointer = labelPointer;
}


/*******************************************************************************
* ����	    : ����Form����TextBox������ʱ���TextBox���ݣ����غ���Ҫ��TextBox��ʼ������
* �������  : formPointer :����Formָ��
*           : textBoxPointer: �ؼ�TextBoxָ��
*******************************************************************************/
static void AddTextBox(Form * formPointer, TextBox *textBoxPointer)
{    
    TextBox * pointer;
    
    textBoxPointer->DataPointer = null;
    textBoxPointer->DataMax = 0;
    textBoxPointer->DataMin = 0;
    textBoxPointer->DataStep = 0;
    textBoxPointer->DataBigStep = 0;
    
    textBoxPointer->Digits= 0;
    textBoxPointer->Offset = 0;
    textBoxPointer->Coefficient = 1;

    // ����
    textBoxPointer->KeyProcessFunction = null;
    
    textBoxPointer->StringBlockPointer = null;
    textBoxPointer->Align = GuiDataAlignRight;
    textBoxPointer->NextTextBoxPointer = null;

    if (formPointer->TextBoxPointer == null)
    {
         formPointer->TextBoxPointer = textBoxPointer;
         return;
    }
    
    pointer = formPointer->TextBoxPointer;
        
    while (pointer->NextTextBoxPointer != null)
        pointer = pointer->NextTextBoxPointer;

    pointer->NextTextBoxPointer = textBoxPointer;
}

static void AddMessage(int id, int x, int y, char *fmt, ...)
{
    int len;
    va_list list;

    va_start(list, fmt);
    len = vsprintf(MessageBuffer[id].Buffer, fmt, list);
    va_end(list);

    if (len == 0) return;
    if (id >= MessageSum) return;

    MessageBuffer[id].X = x;
    MessageBuffer[id].Y = y;
    MessageBuffer[id].Len = len;
    MessageBuffer[id].Valid = true;
}

static void DeleteMessage(int id)
{
    MessageBuffer[id].Valid = false;
}

/*******************************************************************************
* ����	    : �ı��ؼ������л���������л��ı��ؼ�����
*******************************************************************************/
static void SwitchTextBoxFocus(void)
{
    if (FocusFormPointer->FocusTextBoxPointer == null)
    {
        FocusFormPointer->FocusTextBoxPointer = FocusFormPointer->TextBoxPointer;
        return;
    }
    
    FocusFormPointer->FocusTextBoxPointer = FocusFormPointer->FocusTextBoxPointer->NextTextBoxPointer;
}

/*******************************************************************************
* ����	    : �ı��ؼ���ֵ�޸Ĵ����޸ĳ���
* �������  : key: AddKey��AddLongKey��SubKey��SubLongKey
*           :      AssistUpKey��AssistUpLongKey��AssistDownKey��AssistDownLongKey
*******************************************************************************/
static void ModifyTextBoxData(KeyEnum key)
{
    TextBox * textBoxPointer;
    void * dataPointer;
    GuiDataType dataType;
    
    int s32;
    int s32Max;
    int s32Min;
    int s32Step;
    int s32BigStep;
    
    uint u32;
    uint u32Max;
    uint u32Min;
    uint u32Step;
    uint u32BigStep;

    float f32;
    float f32Max;
    float f32Min;
    float f32Step;
    float f32BigStep;
    

    textBoxPointer = FocusFormPointer->FocusTextBoxPointer;
    if (textBoxPointer == null) return;
    
    dataPointer = textBoxPointer->DataPointer;
    dataType = textBoxPointer->Type;
    
    switch(dataType)
    {
//����������
        case GuiDataTypeShortDec:
            s32 = * (short *)(textBoxPointer->DataPointer);
            s32Max = (short)(textBoxPointer->DataMax);
            s32Min = (short)(textBoxPointer->DataMin);
            s32Step = (short)(textBoxPointer->DataStep);        
            s32BigStep = (short)(textBoxPointer->DataBigStep);
            goto ProcInt;
        case GuiDataTypeIntDec:
            s32 = * (int *)(textBoxPointer->DataPointer);
            s32Max = (int)(textBoxPointer->DataMax);
            s32Min = (int)(textBoxPointer->DataMin);
            s32Step = (int)(textBoxPointer->DataStep);        
            s32BigStep = (int)(textBoxPointer->DataBigStep);
            
ProcInt:    switch(key)
            {
                case KeyAdd: s32 += s32Step; break;
                case KeyLongAdd: s32 += s32BigStep; break;
                case KeySub: s32 -= s32Step; break;
                case KeyLongSub: s32 -= s32BigStep; break;
            }
            if (s32 > s32Max) s32 = s32Max; 
            else if (s32 < s32Min) s32 = s32Min;

            if (dataType == GuiDataTypeIntDec) *(int *)dataPointer = s32; 
            else *(short *)dataPointer = s32;
            break;
//�޷�������
        case GuiDataTypeSnString:
        case GuiDataTypeByteDec:
            u32 = * (byte *)(textBoxPointer->DataPointer);
            u32Max = (byte)(textBoxPointer->DataMax);
            u32Min = (byte)(textBoxPointer->DataMin);
            u32Step = (byte)(textBoxPointer->DataStep);        
            u32BigStep = (byte)(textBoxPointer->DataBigStep);
            goto ProcUint;
        case GuiDataTypeUshortDec:
        case GuiDataTypeUshortHex:
            u32 = * (ushort *)(textBoxPointer->DataPointer);
            u32Max = (ushort)(textBoxPointer->DataMax);
            u32Min = (ushort)(textBoxPointer->DataMin);
            u32Step = (ushort)(textBoxPointer->DataStep);        
            u32BigStep = (ushort)(textBoxPointer->DataBigStep);
            goto ProcUint;
        case GuiDataTypeUintDec:
        case GuiDataTypeUintHex:
            u32 = * (uint *)(textBoxPointer->DataPointer);            
            u32Max = (uint)(textBoxPointer->DataMax);
            u32Min = (uint)(textBoxPointer->DataMin);
            u32Step = (uint)(textBoxPointer->DataStep);        
            u32BigStep = (uint)(textBoxPointer->DataBigStep);
            
ProcUint:   switch(key)
            {
                case KeyAdd: u32 += u32Step; break;
                case KeyLongAdd: u32 += u32BigStep; break;
                case KeySub: if (u32 < u32Step) u32 = 0; else u32 -= u32Step; break;
                case KeyLongSub: if (u32 < u32BigStep) u32 = 0; else u32 -= u32BigStep; break;
            }  
            if (u32 > u32Max) u32 = u32Max;
            else if (u32 < u32Min) u32 = u32Min;

            switch(dataType)
            {   
                case GuiDataTypeSnString:
                case GuiDataTypeByteDec:
                    *(byte *)dataPointer = u32;
                    break;
                case GuiDataTypeUshortDec:
                case GuiDataTypeUshortHex:
                    *(ushort *)dataPointer = u32;
                    break;
                case GuiDataTypeUintDec:
                case GuiDataTypeUintHex:
                    *(uint *)dataPointer = u32;
                    break;
            }
            break;
//�����ȸ�����
        case GuiDataTypeFloatDec:
            f32 = * (float *)(textBoxPointer->DataPointer);
            f32Max = Float(textBoxPointer->DataMax);
            f32Min = Float(textBoxPointer->DataMin);
            f32Step = Float(textBoxPointer->DataStep);       
            f32BigStep = Float(textBoxPointer->DataBigStep);
            
            switch(key)
            {
                case KeyAdd: f32 += f32Step; break;
                case KeyLongAdd: f32 += f32BigStep; break;
                case KeySub: f32 -= f32Step; break;
                case KeyLongSub: f32 -= f32BigStep; break;
            }  
            if (f32 > f32Max) f32 = f32Max;
            else if (f32 < f32Min) f32 = f32Min;

            *(float *)dataPointer = f32;
            break;
        default:
            break;
    }
		
   if(textBoxPointer->KeyProcessFunction != null)
          textBoxPointer->KeyProcessFunction();

}

void InitGui(void)
{
    byte i;
    for (i = 0; i < 64; i++)
    {
        ((byte *)GuiLcd)[i] = ' ';
        ((byte *)VirtualLcd)[i] = ' ';
    }
    
    System.Gui.Parse = Parse;
    
    System.Gui.Form.AddLabel = AddLabel;

    System.Gui.Form.AddTextBox = AddTextBox;

    System.Gui.Form.Init = InitForm;    

    System.Gui.Form.SwitchTextBoxFocus = SwitchTextBoxFocus;

    System.Gui.Form.ModifyTextBoxData = ModifyTextBoxData;

    System.Gui.Form.AddMessage = AddMessage;
    
    System.Gui.Form.DeleteMessage = DeleteMessage;
}




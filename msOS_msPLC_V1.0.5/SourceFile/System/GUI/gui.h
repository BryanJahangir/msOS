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
*�ļ���     : gui.h
*����       : 
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/06/21       ��ʼ�汾
********************************************************************************
*/



#ifndef __GUI_H
#define __GUI_H




typedef enum 
{
    GuiDataTypeByteDec,
    GuiDataTypeShortDec,
    GuiDataTypeUshortDec,
    GuiDataTypeIntDec,
    GuiDataTypeUintDec,
    GuiDataTypeFloatDec,
    GuiDataTypeUshortHex,
    GuiDataTypeUintHex,
    GuiDataTypeString,              // ֱ����ʾ�ַ���
    GuiDataTypeSnString             // �����ʾ�ַ���(���ַ�����������)
}GuiDataType;

typedef enum
{
	GuiDataAlignLeft,
	GuiDataAlignRight
}GuiDataAlign;

typedef struct
{
    char Character;
    byte Column[16];
}Chart;

typedef struct LabelSelf
{
    byte X;
    byte Y;
    
    GuiDataType Type;
    GuiDataAlign Align;
    ushort Digits;
    
    float Offset;
    float Coefficient;
    
    void * DataPointer;
    const string * StringBlockPointer;
    struct LabelSelf * NextLabelPointer;
}Label;

typedef struct TextBoxSelf
{
    byte X;
    byte Y;
    
    GuiDataType Type;
    GuiDataAlign Align;
    ushort Digits;
    
    float Offset;
    float Coefficient;
    
    void * DataPointer;
    const string * StringBlockPointer;
    struct TextBoxSelf * NextTextBoxPointer;

    int DataMax;
    int DataMin;
    int DataStep;
    int DataBigStep;

    //void * Pointer;
    function  KeyProcessFunction;
}TextBox;

typedef struct MessageSelf
{
    bool Valid;
    byte X;
    byte Y;
    byte Len;
    char Buffer[16];
}Message;

typedef struct
{
    const byte* BackTextPointer;
    Chart     * ChartPointer;
    Label     * LabelPointer;
    TextBox   * TextBoxPointer;
    TextBox   * FocusTextBoxPointer;
}Form;


extern void InitGui(void);
    
#endif 





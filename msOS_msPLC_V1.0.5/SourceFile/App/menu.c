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
*�ļ���     : menu.c
*����       : �������
*ԭ��       : ���ӳ����а����������Form��ÿһ��������Լ���BackText�������֡�
*           : Chart��Label��ǩ��TextBox�ı�����ؼ�������Add��������֮��
*           : ��Ҫ��ʼ��ֵ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/

#include "system.h"
#include "app.h"

static void InitLogoForm(void)
{
    System.Gui.Form.Init(&App.Menu.LogoForm);
    App.Menu.LogoForm.BackTextPointer = "    ��οƼ�    "
                                        "      msOS      "
                                        "     V1.4.0     "
                                        "   2018.7.17    ";
}

void InitWorkForm(void)
{
    static TextBox PwmCycleTextBox;
    static TextBox PwmHighLevelTextBox;
  	static TextBox Lm35dtVoltageLabel;
    static Label Lm35dtTemperatureLabel;

//    static Label OnOffLabel;
//    static const string OnOffString[] = {"��", "��"};
    
    System.Gui.Form.Init(&App.Menu.WorkForm);
    App.Menu.WorkForm.BackTextPointer = "��  �ڣ�      ms"
                                        "ʱ  �䣺      ms"
                                        "��  ѹ:       mv"
                                        "��  ��:       ��";

    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &PwmCycleTextBox);
    PwmCycleTextBox.DataPointer = (void *)(&App.Data.PwmCycle);
    PwmCycleTextBox.Type = GuiDataTypeIntDec;
    PwmCycleTextBox.DataMax = 1200;
    PwmCycleTextBox.DataMin = 10;
    PwmCycleTextBox.DataBigStep = 100;
    PwmCycleTextBox.DataStep = 5;
    PwmCycleTextBox.X = 13;
    PwmCycleTextBox.Y = 0;
    PwmCycleTextBox.KeyProcessFunction = TextBoxKeyTest;
		 
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &PwmHighLevelTextBox);
    PwmHighLevelTextBox.DataPointer = (void *)(&App.Data.PwmHighLevel);
    PwmHighLevelTextBox.Type = GuiDataTypeIntDec;
    PwmHighLevelTextBox.DataMax = 1200;
    PwmHighLevelTextBox.DataMin = 10;
    PwmHighLevelTextBox.DataBigStep = 100;
    PwmHighLevelTextBox.DataStep = 5;
    PwmHighLevelTextBox.X = 13;
    PwmHighLevelTextBox.Y = 1;

    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &Lm35dtVoltageLabel);
    Lm35dtVoltageLabel.DataPointer = (void *)(&App.Data.Lm35dtVoltage);
    Lm35dtVoltageLabel.Type = GuiDataTypeIntDec;
    Lm35dtVoltageLabel.DataMax = 1200;
    Lm35dtVoltageLabel.DataMin = 10;
    Lm35dtVoltageLabel.Digits = 4;
    Lm35dtVoltageLabel.DataBigStep = 10;
    Lm35dtVoltageLabel.DataStep = 1;
    Lm35dtVoltageLabel.X = 13;
    Lm35dtVoltageLabel.Y = 2;
    Lm35dtVoltageLabel.KeyProcessFunction = TextBoxKeyTest1;
		

//    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &Lm35dtTemperatureLabel);
//    Lm35dtTemperatureLabel.DataPointer = (void *)(&App.Data.Lm35dtTemperature);
//    Lm35dtTemperatureLabel.Type = GuiDataTypeFloatDec;
//    Lm35dtTemperatureLabel.Digits = 2;
//    Lm35dtTemperatureLabel.X = 13;
//    Lm35dtTemperatureLabel.Y = 3;

//    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &OnOffLabel);
//    OnOffLabel.DataPointer = (void *)(&App.Data.OnOff);
//    OnOffLabel.Type = GuiDataTypeSnString;
//    OnOffLabel.Align = GuiDataAlignRight;
//    OnOffLabel.StringBlockPointer = OnOffString;
//    OnOffLabel.X = 15;
//    OnOffLabel.Y = 3;
}


void InitMenu(void)
{
    InitLogoForm();
    InitWorkForm();
}
/*******************************************************************************
* ������	: MenuTask
* ����	    : MenuTask���񣬱���Ϊ������ȼ����񣬽�������Form�ڵĸ����ؼ�������ʾ
* �������  : ��
* ���ز���  : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/12/16       �޸İ汾
*******************************************************************************/
void MenuTask (void)
{
    static uint MenuSystick; 
    
    InitMenu();
    MenuSystick = App.Data.Systick1000 + 100;
    while(true)             
    {
        if (App.Data.Systick1000 >= MenuSystick)
        {
            MenuSystick = App.Data.Systick1000 + 100;
            System.Gui.Parse(App.Menu.FocusFormPointer);
        }
    }
}


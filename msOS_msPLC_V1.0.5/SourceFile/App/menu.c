/*
********************************************************************************
*                                嵌入式微系统
*                                   msOS
*
*                            硬件平台:msPLC DEMO
*                          主芯片:STM32F103R8T6/RBT6
*                           深圳市雨滴科技有限公司
*
*                                作者:王绍伟
*                                网名:凤舞天
*                                标识:Wangsw
*
*                                QQ:26033613
*                               QQ群:291235815
*                        论坛:http://bbs.huayusoft.com
*                        淘宝店:http://52edk.taobao.com
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : menu.c
*作用       : 界面设计
*原理       : 例子程序中包含五个窗体Form，每一个窗体可以加载BackText背景文字、
*           : Chart表、Label标签、TextBox文本四类控件，采用Add方法加载之后，
*           : 需要初始化值。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"
#include "app.h"

static void InitLogoForm(void)
{
    System.Gui.Form.Init(&App.Menu.LogoForm);
    App.Menu.LogoForm.BackTextPointer = "    雨滴科技    "
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
//    static const string OnOffString[] = {"关", "开"};
    
    System.Gui.Form.Init(&App.Menu.WorkForm);
    App.Menu.WorkForm.BackTextPointer = "周  期：      ms"
                                        "时  间：      ms"
                                        "电  压:       mv"
                                        "温  度:       度";

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
* 函数名	: MenuTask
* 描述	    : MenuTask任务，必须为最低优先级任务，解析窗体Form内的各个控件用于显示
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/12/16       修改版本
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


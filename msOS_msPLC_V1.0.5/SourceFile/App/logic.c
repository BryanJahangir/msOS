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
*�ļ���     : logic.c
*����       : ҵ���߼������ļ�
*ԭ��       : ���������Ϣ���簴�����������ȣ���׼��һЩ�����Ĵ���ģʽ
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/


#include "system.h"
#include "app.h"



/*******************************************************************************
* ����	    : ����ҳ�潹���л���������л�����ҳ��
*******************************************************************************/
static void FormFocusProcess(void)
{    
    if (App.Menu.FocusFormPointer == &App.Menu.WorkForm)
        App.Menu.FocusFormPointer = &App.Menu.LogoForm;
    else
        App.Menu.FocusFormPointer ++;
}

/*******************************************************************************
* ����	    : �洢�����ı��ؼ�������ֵ
*******************************************************************************/
void SaveProcess(void)
{
    if (App.Menu.FocusFormPointer->FocusTextBoxPointer == null) return;
   
    System.Device.Storage.Parameter.Write((uint *)App.Menu.FocusFormPointer->FocusTextBoxPointer->DataPointer);
}


/*******************************************************************************
* ����	    : �洢���в���
*******************************************************************************/
void LongSaveProcess(void)
{
    TextBox * textBoxPointer;
    textBoxPointer = App.Menu.FocusFormPointer->TextBoxPointer;
    while (textBoxPointer != null)
    {
        System.Device.Storage.Parameter.Write(textBoxPointer->DataPointer);
        textBoxPointer = textBoxPointer->NextTextBoxPointer;
    }
}

/*******************************************************************************
* ����	    : Stopȱʡ������
*******************************************************************************/
static void StopDefaultProcess(void)
{
    App.Menu.FocusFormPointer->FocusTextBoxPointer = null;
    App.Menu.FocusFormPointer = &App.Menu.WorkForm;
}

static void TextBoxKeyProcess(KeyEnum key)
{
    void * dataPointer;
    // ���½�Ϊ����, �����ı��ؼ�ֵ�޸ĺ��ֱ�Ӳ������������ֱ�ӵ��ú���ʵ�ֶ���������ǳ���Ҫ
    dataPointer = App.Menu.FocusFormPointer->FocusTextBoxPointer->DataPointer;
    if (dataPointer == &App.Data.Frequency)
    {
        ;
    }
    else if(dataPointer == &App.Data.PowerPercent)
    {
        ;
    }
}

void TextBoxKeyTest(void)
{		
	App.Data.DO.Y0 = ~ App.Data.DO.Y0;
}

void TextBoxKeyTest1(void)
{		
	App.Data.DO.Y2 = ~ App.Data.DO.Y2;
}


static byte Counter = 0;
/*******************************************************************************
* ����	    : ������Ϣ������
* �������  : key�����ⰴ��ֵ
*******************************************************************************/
void KeyProcess(KeyEnum key)
{
    switch(key)
    {
        case KeyAdd:                // ���ְ�������
        case KeyLongAdd:
        case KeySub :
        case KeyLongSub:
            System.Gui.Form.ModifyTextBoxData(key);
            
            //TextBoxKeyProcess(key);
            break;
            
        case KeyAuxUp:              // д��־(����)
            System.Device.Storage.Log.Write("Log Test = %d\n", Counter++);
            break;
        case KeyLongAuxUp:          // ��PWMChannel3(����)
            System.Device.DO.Pwm.SetDutyRatio(PwmChannel3,App.Data.PwmHighLevel);
            
            break;
        case KeyAuxDown:            // ����־(����)
            printf(System.Device.Storage.Log.Read(-1));
            break;
        case KeyLongAuxDown:        // �ر�PWMChannel0(����)
            System.Device.DO.Pwm.Close(PwmChannel3);
            break;
            
        case KeySave:               // �������ݴ洢
            SaveProcess();     
            break;
        case KeyLongSave :          // ҳ���ݴ洢
            LongSaveProcess();
            break;
            
        case KeyTextBoxFocus:       // �ı��л�
            System.Gui.Form.SwitchTextBoxFocus();
            break; 
        case KeyLongFormFocus:      // ҳ���л�
            FormFocusProcess();
            break;   
 
        case KeyStart:              // ��ʼ����
            //App.Data.OnOff = on;
			//System.Device.DO.Pwm.SetParameter(PwmChannel1, 7200, App.Data.PwmCycle, App.Data.PwmHighLevel);
            break;
        case KeyLongStart:          // ��ʱ��(����)
            System.Device.Timer.Start(0, TimerMessage, 1000, FormFocusProcess);
            System.Device.Timer.Start(1, TimerMessage, 2000, FormFocusProcess);
            System.Device.Timer.Start(2, TimerMessage, 3000, FormFocusProcess);
            System.Device.Timer.Start(3, TimerMessage, 4000, FormFocusProcess);
            System.Device.Timer.Start(4, TimerMessage, 5000, FormFocusProcess);
            System.Device.Timer.Start(5, TimerMessage, 6000, FormFocusProcess);
            System.Device.Timer.Start(6, TimerMessage, 7000, FormFocusProcess);
            System.Device.Timer.Start(7, TimerMessage, 7000, FormFocusProcess);
            break;
            
        case KeyStop:               //ֹͣ����
            //App.Data.OnOff = off;
		    //System.Device.DO.Pwm.Close(PwmChannel1);
					break;
        case KeyLongStop:
            //...
            StopDefaultProcess();
            break;         
    }
}


static void AppSystick100(void)
{
    print("�ڶ����ύ...")
}

static void InitLogic(void)
{
    System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Data.Voltage));
    System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));
    System.Device.Adc.Register(AdcTemperature, (ushort *)(&App.Data.Temperature));
    System.Device.Systick.Register(Systick100, AppSystick100);
	//PWM��ʼ��
//    System.Device.DO.Pwm.SetParameter(PwmChannel1, 7200, App.Data.PwmCycle, App.Data.PidPowerPercent);
}

void LogicTask(void)
{
    int i;
    uint message;
    uint data; 
    InitLogic();

    App.Menu.FocusFormPointer = &App.Menu.LogoForm;     //ҳ�潹��

    System.OS.DelayMs(2000);
    
    App.Menu.FocusFormPointer = &App.Menu.WorkForm;   

    System.Device.Key.Enable(true);
    //�߼�ҵ�������ȡ��Ϣ�����䴦��
    while(true)
    {     
        message = System.OS.PendMessageQueue();
        
        data = message & 0x00FFFFFF;
        switch(message >> 24)
        {
            case MessageKey:                    //������Ϣ
                KeyProcess((KeyEnum)data);
                break;
            case MessageTimer:
                Function(data + RomBase);
                break;
            default:
                break;
        }
    }
}

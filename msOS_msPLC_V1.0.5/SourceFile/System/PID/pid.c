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
*�ļ���       : pid.c
*����         : �豸�ӿ�
*ԭ��         : ��
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2013/07/21       ��ʼ�汾
********************************************************************************
*/



#include "system.h"


PidStruct Pid;
/*******************************************************************************
* ����	    : pid���ƺ���
*******************************************************************************/

void PidSystick100(void)
{
    printf("Pid ���....\n");
    if(Pid.Enable == true)
    {
    	Pid.Error = Pid.Target - Pid.Current; //���

    	if (abs(Pid.Error) < Pid.TargetDistance)
    	{
    		Pid.AddError += Pid.Error;						
    		Pid.Differential = Pid.Error - (Pid.Target - Pid.Before);

    		if(Pid.AddError > Pid.MaxAddError) Pid.AddError = Pid.MaxAddError;
    		else if(Pid.AddError < -Pid.MaxAddError) Pid.AddError = -Pid.MaxAddError;

    		Pid.PidOut = Pid.KP * Pid.Error + Pid.KI * Pid.AddError + Pid.KD * Pid.Differential;
    		
    		Pid.Before = Pid.Current;
    	}
    	else
    		Pid.PidOut = Pid.KP * Pid.Error;
    	
    	if (Pid.PidOut > Pid.MaxDutyRatio) Pid.PidOut = Pid.MaxDutyRatio;
    	else if (Pid.PidOut <= 0) Pid.PidOut = 0;
        
    }
	
	//System.Device.DO.Pwm.SetDutyRatio(PwmChannel1, Pid.PidOut);
}


/*******************************************************************************
* ����	    : pid��ʼ��
*******************************************************************************/
void InitPid(void)
{
	Pid.KP = 6;
	Pid.KI = 0.95;
	Pid.KD = 0;
	Pid.MaxAddError = 60;
	Pid.MaxDutyRatio = 100;
    
	Pid.Current = AppDataPointer->PidCurrentValue;
	Pid.Target = AppDataPointer->PidTargetValue;
    printf("Pid Init....\n");
    Pid.PidSystick100 = PidSystick100;
//	System.Device.Systick.Register(Systick100, PidSystick100);
}

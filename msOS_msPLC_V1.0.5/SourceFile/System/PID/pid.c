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
*文件名       : pid.c
*作用         : 设备接口
*原理         : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "system.h"


PidStruct Pid;
/*******************************************************************************
* 描述	    : pid控制函数
*******************************************************************************/

void PidSystick100(void)
{
    printf("Pid 输出....\n");
    if(Pid.Enable == true)
    {
    	Pid.Error = Pid.Target - Pid.Current; //误差

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
* 描述	    : pid初始化
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

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
*文件名     : pid.h
*作用       : 定义设备层相关宏定义
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/


#ifndef __PID_H
#define __PID_H

typedef struct 
{
    float KI;
    float KP;
    float KD;
    float PidOut;
    
    float TargetDistance;
    float MaxAddError;  
    int MaxDutyRatio;
    
	float Before;  
	float Current;
    int Target;  
	float Error;
	float AddError;
	float Differential;

    bool Enable;
    
    void (*PidSystick100)(void);
    
}PidStruct;

extern PidStruct Pid;
extern void InitPid(void);

#endif



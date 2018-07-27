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
*�ļ���     : pid.h
*����       : �����豸����غ궨��
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/06/21       ��ʼ�汾
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



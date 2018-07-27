/*
********************************************************************************
*                                ??��?��??��?�̨�3
*                                   msOS
*
*                            ��2?t??����:msPLC DEMO
*                          ?��D???:STM32F103R8T6/RBT6
*                           ��??����D������?????��D?T1???
*
*                                ����??:��?����?��
*                                ��???:��??������
*                                ������?:Wangsw
*
*                                QQ:26033613
*                               QQ��o:291235815
*                        ??��3:http://bbs.huayusoft.com
*                        ��?��|�̨�:http://52edk.taobao.com
*                2??��:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*???t??     : device_rtc.c
*���¨�?       : 32.768KHz����?��?�쨬?2����������?��������?����?���䨮2013?��1??1��?D??��?t?a��?,��?D?��a�䨮����?��o����??��?a��?
*?-����       : ��?����?��������????������o?2013~2099?����?2100?����?����?��
********************************************************************************
*��?��?     ����??            ��??��            ?��?��
*V0.1    Wangsw        2015/11/25       3?��?��?��?
********************************************************************************
*/

#include "system.h"


/*******************************************************************************
* ����	    : Rtcϵͳʱ��100/S����10mSһ�ε���
*******************************************************************************/
void RtcSystick100Routine(void) 
{
    static byte Counter = 0;
    
    if (++Counter == 100)
    {
        Counter = 0;

        if (AppDataPointer->Rtc.Second < 59)
            AppDataPointer->Rtc.Second++;
        else
        {
            AppDataPointer->Rtc.Second = 0;
            if(AppDataPointer->Rtc.Minute < 59)
                AppDataPointer->Rtc.Minute++;
            else
            {
                AppDataPointer->Rtc.Minute = 0;
                if(AppDataPointer->Rtc.Hour < 23)
                    AppDataPointer->Rtc.Hour++;
                else
                {
                    AppDataPointer->Rtc.Hour = 0;
                    AppDataPointer->Rtc.Day++;
                } 
            }
        }
    }	
}


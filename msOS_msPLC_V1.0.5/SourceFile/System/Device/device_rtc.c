/*
********************************************************************************
*                                ??è?ê??￠?μí3
*                                   msOS
*
*                            ó2?t??ì¨:msPLC DEMO
*                          ?÷D???:STM32F103R8T6/RBT6
*                           é??úêDóêμ?????óD?T1???
*
*                                ×÷??:í?éü?°
*                                í???:·??èìì
*                                ±êê?:Wangsw
*
*                                QQ:26033613
*                               QQèo:291235815
*                        ??ì3:http://bbs.huayusoft.com
*                        ì?±|μê:http://52edk.taobao.com
*                2??í:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*???t??     : device_rtc.c
*×÷ó?       : 32.768KHzê±?ó?§ì?2úéúíò?êàú￡?ê±?ó′ó2013?ê1??1è?D??ú?t?aê?,±?D?òa′óèò?êoóò??ê?aê?
*?-àí       : ±?íò?êàú￡????üêêo?2013~2099?ê￡?2100?ê·?èò?ê
********************************************************************************
*°?±?     ×÷??            è??ú            ?μ?÷
*V0.1    Wangsw        2015/11/25       3?ê?°?±?
********************************************************************************
*/

#include "system.h"


/*******************************************************************************
* 描述	    : Rtc系统时钟100/S，即10mS一次调用
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


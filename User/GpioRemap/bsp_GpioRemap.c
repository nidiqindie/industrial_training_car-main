#include "bsp_GpioRemap.h"

//将特殊的IO口处理为普通IO口使用
//PA15、PB3、PB4、PC13、PC14、PC15
//缓解IO口紧张的问题
//AFIO时钟只能在这里开启一次

void GpioRemap_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    PWR_BackupAccessCmd( ENABLE );/* 允许修改RTC和后备寄存器*/
    RCC_LSEConfig( RCC_LSE_OFF ); /* 关闭外部低速时钟,PC14+PC15可以用作普通IO*/
    BKP_TamperPinCmd(DISABLE);  /* 关闭入侵检测功能,PC13可以用作普通IO*/
    PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//重定义GPIO，释放PA15、PB3、PB4,之后不能用JTAG进行调试
}


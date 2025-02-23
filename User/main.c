#include "stm32f10x.h"
#include "app.h"
#include "board.h"
#include "bsp_usart.h"
#include "Emm_V5.h"
#include "servo.h"
#include "bsp_led.h"
#include "bsp_motor.h"
#include "Serial.h"
#include "HMI.h"
#include "Serial_k210.h"
#include "OLED.h"
#include "delay.h"
#include "jy61p.h"
/*
 move_forward(speed_all, 10, 7.7);
    delay_ms(5200);
    这个速度加这个时间刚刚好，而且7.7就是跑满一边的长度

*/
//
int now = 2;
#define speed_all          200
#define big_calibrations   5
#define smill_calibrations 1
extern uint8_t Serial_RxFlag, K;
char color;
extern uint16_t DistanceX, DistanceY, Size, XX, YY;
extern int task[2];
unsigned char buf[64];
// 提取函数声明

void Roughing_to_staging_area();

void Running_from_th_ematerial_area_to_the_roughing_area();
void go_to_target(int target);
int main(void)
{

    car_init();
    start_p();
    
    while (1)
    {
        /* code */
    }
    
    // while (task[0] == 0 || task[1] == 0); // 二维码识别发送

    // sprintf((char *)buf, "page0.t0.txt=\"%d\"", task[0]); // 强制类型转化，转化为字符串
    // HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    // HMISendb(0xff);                                       // 结束符
    // delay_ms(speed_all);
    // sprintf((char *)buf, "page0.t1.txt=\"%d\"", task[1]); // 强制类型转化，转化为字符串
    // HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    // HMISendb(0xff);                                       // 结束符
    // move_right(speed_all, 20, 0.2);
    // delay_ms(900);
    // 出来
    move_left(speed_all, 20, 0.5);
    delay_ms(1000);
    start_p();
    // 前进到扫码
    move_forward(speed_all, 10, 2.4);
    delay_ms(3000);

    while (task[0] == 0 || task[1] == 0); // 二维码识别发送

    sprintf((char *)buf, "page0.t0.txt=\"%d\"", task[0]); // 强制类型转化，转化为字符串
    HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    HMISendb(0xff);                                       // 结束符
    delay_ms(speed_all);
    sprintf((char *)buf, "page0.t1.txt=\"%d\"", task[1]); // 强制类型转化，转化为字符串
    HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    HMISendb(0xff);                                       // 结束符
    move_right(speed_all, 20, 0.2);
    delay_ms(900);


    // 前进到获取物料
    move_forward(speed_all, 10, 3.4);
    delay_ms(3000);
    // 微调加抓取
    //微调（转盘色块）
    catch (task[0] / 100);
    put(task[0] / 100);
    catch ((task[0] % 100) / 10);
    put((task[0] % 100) / 10);
    catch (task[0] % 10);
    put(task[0] % 10);

    // 从物料区跑到粗加工区
    // 后退转弯到粗加工
    move_backward(speed_all, 10, 1.5);
    delay_ms(1900);
    yaw_run(90, big_calibrations);
    move_forward(speed_all, 10, 3.2);
    delay_ms(2800);
    yaw_run(90, smill_calibrations);
    move_forward(speed_all, 10, 3.5);
    delay_ms(2800);
    yaw_run(180, big_calibrations);
    
    // 加工物料
    start_p();
    delay_ms(2000);
    go_to_target(task[0] / 100);
    put_huan(task[0] / 100);
    go_to_target((task[0]%100)/10);
    put_huan((task[0]%100)/10);
    go_to_target(task[0] % 10);
    put_huan(task[0] % 10);

    //加工完成
    go_to_target(task[0] / 100);
    catch_huan();
    put(task[0] / 100);
    go_to_target((task[0]%100)/10);
    catch_huan();
    put((task[0]%100)/10);
    go_to_target(task[0] % 10);
    catch_huan();
    put(task[0] % 10);
    go_to_target(2);
    // 粗加工到暂存区
    Roughing_to_staging_area();

    //暂存区放置物料
    go_to_target(task[0] / 100);
    put_huan(task[0] / 100);
    go_to_target((task[0]%100)/10);
    put_huan((task[0]%100)/10);
    go_to_target(task[0] % 10);
    put_huan(task[0] % 10);
    go_to_target(2);
    /*
    第二圈
    */
    // 暂存区到抓取二轮物料
    move_backward(speed_all, 10, 3.1);
    delay_ms(3000);
    yaw_run(0, big_calibrations);
    move_backward(speed_all, 10, 1.6);
    delay_ms(3000);
    move_right(speed_all, 20, 0.2);
    delay_ms(500);

    //从原料区抓取物料

    //微调
    catch (task[1] / 100);
    put(task[1] / 100);
    catch ((task[1] % 100) / 10);
    put((task[1] % 100) / 10);
    catch (task[1] % 10);
    put(task[1] % 10);   


    move_left(speed_all, 20, 0.2);
    delay_ms(500);
    // 从物料区跑到粗加工区
    // 后退转弯到粗加工
    move_backward(speed_all, 10, 1.5);
    delay_ms(1900);
    yaw_run(90, big_calibrations);
    move_forward(speed_all, 10, 3.5);
    delay_ms(2800);
    yaw_run(90, smill_calibrations);
    move_forward(speed_all, 10, 3.2);
    delay_ms(2800);

    yaw_run(180, big_calibrations);
    move_right(speed_all, 20, 0.2);
    delay_ms(500);
    // 在这里写放置函数
//微调
    start_p();
    delay_ms(2000);
    put_huan(task[1] / 100);
    put_huan((task[1]%100)/10);
    put_huan(task[1] % 10);

    //加工完成取物料
    catch_huan();
    put(task[1] / 100);
    catch_huan();
    put((task[1]%100)/10);
    catch_huan();
    put(task[1] % 10);


    move_left(speed_all, 20, 0.2);
    delay_ms(500);

    // 粗加工到暂存区
    Roughing_to_staging_area();

    //放物料到暂存区
    //微调(环)
    put_huan(task[0] / 100);
    put_huan((task[0]%100)/10);
    put_huan(task[0] % 10);



    // 回到起点
    move_backward(speed_all, 10, 3.0);
    delay_ms(3000);
    yaw_run(0, big_calibrations);
    move_backward(speed_all, 10, 3.9);
    delay_ms(3000);
    yaw_run(0, smill_calibrations);
    move_backward(speed_all, 10, 4.1);
    delay_ms(3000);
    move_right(speed_all, 20, 1.0);
    delay_ms(1000);
    while (1) {
        /* code */
    }

 
    while (1) {
        // printf("%d\n",curAngle);
        // delay_ms(1000);
    }
}
// 粗加工到暂存区
void Roughing_to_staging_area()
{
    move_backward(speed_all, 10, 3.3);
    delay_ms(3000);
    yaw_run(90, big_calibrations);
    move_backward(speed_all, 10, 3.7);
    delay_ms(3000);
    yaw_run(90, smill_calibrations);
}
// 从物料区跑到粗加工区
void Running_from_th_ematerial_area_to_the_roughing_area()
{
    // 后退转弯到粗加工
    move_backward(speed_all, 10, 1.5);
    delay_ms(1900);
    yaw_run(90, big_calibrations);
    move_forward(speed_all, 10, 3.5);
    delay_ms(2800);
    yaw_run(90, smill_calibrations);
    move_forward(speed_all, 10, 3.5);
    delay_ms(2800);

    yaw_run(180, big_calibrations);
}
//粗加工区按顺序位移，已包含了微调
void go_to_target(int target)
{
    
    while (now != target)
    {
        
        if (target > now)
        {
            now++;
            move_backward(speed_all, 10, 0.6);
            delay_ms(1000);
            /* code */
        }else if(target < now)
        {
            now--;
            move_forward(speed_all, 10, 0.6);
            delay_ms(1000);
            /* code */
        }
        
    }

    weitiao(0);
}
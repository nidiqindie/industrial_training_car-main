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
#include "hwt101.h"

/*
 move_forward(speed_all, 10, 7.7);
    delay_ms(5200);
    这个速度加这个时间刚刚好，而且7.7就是跑满一边的长度

*/

// 原始任务码是1-红 2-绿 3-蓝，对应地图上1靠近暂存区的颜色是红色，2中间是绿色，3另一边上是蓝色
#define red_num 1
#define green_num 2
#define blue_num 3

#define manually_calibrated 0
int now = 2;
#define speed_all          300
#define acc_all            100
#define big_calibrations   5
#define smill_calibrations 1
extern uint8_t Serial_RxFlag, K;
char color;
extern uint16_t DistanceX, DistanceY, Size, XX, YY;
extern int task[2];
unsigned char buf[64];

extern int ring_color;
// 提取函数声明

void Roughing_to_staging_area();

void go_to_target(int target, int Type_of_fine_tuning);
extern uint8_t Rxflag_0;
extern uint8_t Rxflag_1;
extern uint8_t Rxflag_2;
void goto_rough();

int main(void)
{

    car_init();
   
    
    // 出来


    
    move_left(speed_all, acc_all, 0.83);
    delay_ms(1000);
    standby_p();
    // 前进到扫码
    move_forward(speed_all, 10, 2.9);
    delay_ms(2500);

    while (task[0] == 0 || task[1] == 0); // 二维码识别发送

    sprintf((char *)buf, "page0.t0.txt=\"%d\"", task[0]); // 强制类型转化，转化为字符串
    HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    HMISendb(0xff);                                       // 结束符
    delay_ms(speed_all);
    sprintf((char *)buf, "page0.t1.txt=\"%d\"", task[1]); // 强制类型转化，转化为字符串
    HMISends((char *)buf);                                // 发送Ri的数据给page0页面的t3文本控件
    HMISendb(0xff);                                       // 结束符
    int task_array[2][3] = {{task[0] / 100, (task[0] % 100) / 10, task[0] % 10}, {task[1] / 100, (task[1] % 100) / 10, task[1] % 10}};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            switch (task_array[i][j])
            {
                // 原始任务码是1-红 2-绿 3-蓝，对应地图上靠近暂存区的颜色是红色，中间是绿色，另一边上是蓝色
            case 1:
                task_array[i][j] = red_num;
                break;
            case 2:
                task_array[i][j] = green_num;
                break;
            case 3:
                task_array[i][j] = blue_num;
                break;
            default:
                break;
            }
        }
    }
    task[0] = task_array[0][0] * 100 + task_array[0][1] * 10 + task_array[0][2];
    task[1] = task_array[1][0] * 100 + task_array[1][1] * 10 + task_array[1][2];
    // 前进到获取物料
    move_forward(speed_all, acc_all, 2.95);
    delay_ms(2500);
    move_right(speed_all, acc_all, 0.33);
    delay_ms(500);

    // 微调加抓取
    // 微调（转盘色块）
    catch_p();
    weitiao_2();

    catch (task[0] / 100);
    put(task[0] / 100);
    catch_p();
    delay_ms(1500);
    catch ((task[0] % 100) / 10);
    put((task[0] % 100) / 10);
    catch_p();
    delay_ms(1500);
    catch (task[0] % 10);
    put(task[0] % 10);
    standby_p();
    // 从物料区跑到粗加工区
    // 后退转弯到粗加工

    move_left(speed_all, 20, 0.2);
    delay_ms(500);
    yaw_run(0, smill_calibrations);
    move_backward(speed_all, acc_all, 1.69);
    delay_ms(1900);
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    yaw_run(90, smill_calibrations);
    move_forward(speed_all, acc_all, 3.5);
    delay_ms(2800);

    yaw_run(90, smill_calibrations);
    move_forward(speed_all, acc_all, 3.415);
    delay_ms(2800);

    catch_p();
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    move_right(speed_all, 20, 0.4);
    delay_ms(500);
    yaw_run(180, smill_calibrations);


    catch_p();
    // 加工物料
    weitiao(0);
    now = ring_color==red_num?red_num:(ring_color==blue_num?blue_num:green_num);
    delay_ms(100);
    go_to_target(task[0] / 100, 0);
    put_huan(task[0] / 100);
    catch_p();
    go_to_target((task[0] % 100) / 10, 0);
    put_huan((task[0] % 100) / 10);
    catch_p();
    go_to_target(task[0] % 10, 0);
    put_huan(task[0] % 10);
    catch_p();
    // 加工完成取物料
    go_to_target(task[0] / 100, 3);
    catch_huan();
    put(task[0] / 100);
    catch_p();

    go_to_target((task[0] % 100) / 10, 3);
    catch_huan();
    put((task[0] % 100) / 10);
    catch_p();

    go_to_target(task[0] % 10, 3);
    catch_huan();
    put(task[0] % 10);
    catch_p();
    delay_ms(1);

    // 第一圈粗加工到暂存区
    Roughing_to_staging_area();

    // 暂存区放置物料
    catch_p();
    weitiao(0);
    now = ring_color==red_num?red_num:(ring_color==blue_num?blue_num:green_num);
    go_to_target(task[0] / 100, 0);
    put_huan(task[0] / 100);

    catch_p();
    go_to_target((task[0] % 100) / 10, 0);
    put_huan((task[0] % 100) / 10);

    catch_p();
    go_to_target(task[0] % 10, 0);
    put_huan(task[0] % 10);

    catch_p();

    go_to_target(1, 2);
    standby_p();
    /*
    第二圈
    */
    // 暂存区到抓取二轮物料
    move_left(speed_all, 20, 0.3);
    delay_ms(800);
    move_backward(speed_all, acc_all, 3.1);
    delay_ms(3000);
    TurnRight(150, 150, 90);
    delay_ms(2000);
    move_backward(speed_all, acc_all, 1.53);
    delay_ms(3000);

    catch_p();
    delay_ms(50);
    yaw_run(0, smill_calibrations);
    move_right(speed_all, 20, 0.1);
    delay_ms(300);

    // 从原料区抓取物料

    // 微调
    weitiao_2();

    catch_p();
    catch (task[1] / 100);
    put(task[1] / 100);

    catch_p();
    catch ((task[1] % 100) / 10);
    put((task[1] % 100) / 10);

    catch_p();
    catch (task[1] % 10);
    put(task[1] % 10);

    // 从物料区跑到粗加工区
    // 后退转弯到粗加工
    move_left(speed_all, 20, 0.2);
    delay_ms(500);
    delay_ms(50);
    yaw_run(0 + manually_calibrated, smill_calibrations);
    move_backward(speed_all, acc_all, 1.69);
    delay_ms(1900);
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    delay_ms(50);
    yaw_run(90 + manually_calibrated + 1, smill_calibrations);
    move_forward(speed_all, acc_all, 3.5);
    delay_ms(2800);
    // 转人工
    //
    delay_ms(50);
    yaw_run(90 + manually_calibrated, smill_calibrations);
    move_forward(speed_all, acc_all, 3.415);
    delay_ms(2800);

    catch_p();
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    move_right(speed_all, 20, 0.3);
    delay_ms(500);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    
    // 在这里写放置函数
    // 微调
    catch_p();
    weitiao(0);
    now = ring_color==red_num?red_num:(ring_color==blue_num?blue_num:green_num);
    delay_ms(2000);
    go_to_target(task[1] / 100, 0);
    put_huan(task[1] / 100);

    catch_p();
    go_to_target((task[1] % 100) / 10, 0);
    put_huan((task[1] % 100) / 10);

    catch_p();
    go_to_target(task[1] % 10, 0);
    put_huan(task[1] % 10);

    // 加工完成取物料
    catch_p();
    delay_ms(1500);

    go_to_target(task[1] / 100, 3);
    catch_huan();
    put(task[1] / 100);
    catch_p();

    go_to_target((task[1] % 100) / 10, 3);
    catch_huan();
    put((task[1] % 100) / 10);
    catch_p();

    go_to_target(task[1] % 10, 3);
    catch_huan();
    put(task[1] % 10);
    catch_p();
    delay_ms(300);
    // 粗加工到暂存区
    go_to_target(2, 0);
    standby_p();
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    move_left(speed_all, acc_all, 0.4);
    delay_ms(800);
    move_left(speed_all, acc_all, 3.19);
    delay_ms(3000);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    delay_ms(100);
    move_backward(speed_all, acc_all, 3.35);
    delay_ms(3000);
    TurnRight(150, 150, 89);
    delay_ms(2000);
    catch_p();
    delay_ms(50);
    yaw_run(90 + manually_calibrated, smill_calibrations);
    move_right(100, 10, 0.2);
    delay_ms(300);

    // 放物料到暂存区
    // 微调(环)
    catch_p();
    go_to_target(task[1] / 100, 2);
    maduo(task[1] / 100);

    catch_p();
    go_to_target((task[1] % 100) / 10, 2);
    maduo((task[1] % 100) / 10);

    catch_p();
    go_to_target(task[1] % 10, 2);
    maduo(task[1] % 10);

    standby_p();
    while (now != 2) {

        if (2 > now) {
            now++;
            // 前进0.59 后退0.596
            move_forward(speed_all, acc_all, 0.59);
            delay_ms(1500);
            /* code */
        } else if (2 < now) {
            now--;
            move_backward(speed_all, acc_all, 0.596);
            delay_ms(1500);
            /* code */
        }
    }
    // 回到起点
    move_left(speed_all, 20, 0.3);
    delay_ms(700);
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    move_forward(speed_all, acc_all, 3.2);
    delay_ms(3000);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    move_forward(speed_all, acc_all, 4.2);
    delay_ms(3000);
    delay_ms(50);
    yaw_run(180 + manually_calibrated, smill_calibrations);
    move_left(speed_all, 20, 4.25);
    delay_ms(3000);
    move_forward(speed_all, 20, 0.25);
    while (1) {
        /* code */
    }
}
// 从物料区跑到粗加工区
void goto_rough()
{
    move_left(speed_all, 20, 0.2);
    delay_ms(500);
    yaw_run(0, smill_calibrations);
    move_backward(speed_all, acc_all, 1.5);
    delay_ms(1900);
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    yaw_run(90, smill_calibrations);
    move_forward(speed_all, acc_all, 3.5);
    delay_ms(2800);

    yaw_run(90, smill_calibrations);
    move_forward(speed_all, acc_all, 3.35);
    delay_ms(2800);

    catch_p();
    TurnLeft(150, 150, 90);
    delay_ms(2000);
    move_right(speed_all, 20, 0.4);
    delay_ms(500);
    yaw_run(180, smill_calibrations);
    now = 2;
}
// 粗加工到暂存区
void Roughing_to_staging_area()
{
    delay_ms(300);
    go_to_target(2, 0);
    standby_p();
    yaw_run(180, smill_calibrations);
    move_left(speed_all, acc_all, 0.4);
    delay_ms(800);
    move_left(speed_all, acc_all, 3.19);
    delay_ms(3000);
    yaw_run(180, smill_calibrations);
    delay_ms(100);
    move_backward(speed_all, acc_all, 3.45);
    delay_ms(3000);
    TurnRight(150, 150, 90);
    delay_ms(2000);
    catch_p();
    yaw_run(90, smill_calibrations);
    move_right(speed_all, 20, 0.2);
    delay_ms(500);
    // yaw_run(90, smill_calibrations);
}

// 粗加工区按顺序位移，已包含了微调
// Type_of_fine_tuning=0就是色环，Type_of_fine_tuning=2就是地上色块,Type_of_fine_tuning=1就是原料区的色块,Type_of_fine_tuning=3就是抓取地上色块
// 抓取地上物块微调次数少此外与Type_of_fine_tuning=2无异
void go_to_target(int target, int Type_of_fine_tuning)
{
    delay_ms(50);
    while (now != target) {

        if (target > now) {
            now++;
            // 前进0.59 后退0.596
            move_forward(speed_all, acc_all, 0.59);
            delay_ms(1500);
            /* code */
        } else if (target < now) {
            now--;
            move_backward(speed_all, acc_all, 0.596);
            delay_ms(1500);
            /* code */
        }
    }
    if (Type_of_fine_tuning == 0)
    {
        switch (target) {
            case red_num:
                flag_color = 1;
                break;
            case green_num:
                flag_color = 2;
                break;
            case blue_num:
                flag_color = 3;
                break;
            default:
                break;
        }
        weitiao(Type_of_fine_tuning);
        flag_color = 0;
    }
    else
        weitiao(Type_of_fine_tuning);
}

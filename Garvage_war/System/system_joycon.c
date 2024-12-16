/*****************************************************************
ファイル名	: system_joycon.c
内容		: ジョイコン関連の関数
*****************************************************************/
#include <joyconlib.h>

#include "system_struct.h"
#include "system_func.h"


joyconlib_t jc;

/*ジョイコンの初期化*/
void joycon_init()
{
    joycon_err err = joycon_open(&jc, JOYCON_R);
    if (JOYCON_ERR_NONE != err) {
        printf("joycon open failed:%d\n", err);
        return;
    }
}

/*ジョイコンの終了*/
void joycon_end()
{
    joycon_close(&jc);
}

/*ジョイコンの向きを獲得する*/
Vector joycon_vector()
{
    Vector axis;

    float tx = 0, ty = 0, tz = 0;

    joycon_get_state(&jc);

    // センサーの値を表示
    printf("Sensor value is (%f,%f,%f)\n", jc.axis[0].acc_x, jc.axis[0].acc_y, jc.axis[0].acc_z);
    printf("                (%f,%f,%f)\n", jc.axis[0].gyro_x, jc.axis[0].gyro_y, jc.axis[0].gyro_z);

    // 傾きの推定
    float dx = 0, dy = 0, dz = 0;
    for (int i = 2; i >= 0; i--) {
        float x = dx, y = dy, z = dz;
        dx = jc.axis[i].gyro_x + z * jc.axis[i].gyro_y - y * jc.axis[i].gyro_z;
        dy = -z * jc.axis[i].gyro_x + jc.axis[i].gyro_y + x * jc.axis[i].gyro_z;
        dz = y * jc.axis[i].gyro_x - x * jc.axis[i].gyro_y + jc.axis[i].gyro_z;

        // 積分
        tx += dx * 0.005;
        ty += dy * 0.005;
        tz += dz * 0.005;
    }
    // 加算され続けるので，±2πを超えたら修正
    if (tx < -2 * M_PI) tx += 2 * M_PI;
    if (tx > 2 * M_PI) tx -= 2 * M_PI;
    if (ty < -2 * M_PI) ty += 2 * M_PI;
    if (ty > 2 * M_PI) ty -= 2 * M_PI;
    if (tz < -2 * M_PI) tz += 2 * M_PI;
    if (tz > 2 * M_PI) tz -= 2 * M_PI;

    printf("estimated angle:(%f,%f,%f)\n", tx * 180 / M_PI, ty * 180 / M_PI, tz * 180 / M_PI);

    axis.vx = tx * 180 / M_PI;
    axis.vy = ty * 180 / M_PI;
    axis.vz = tz * 180 / M_PI;
    
    return axis;
}

/*ジョイコンのボタンとスティック入力を読み込む*/
Joycon_Input joycon_input()
{
    Joycon_Input input = {false,false,false,false,false,false,false, {0,0,0}};

    joycon_get_state(&jc);

    if(jc.button.btn.A){
        input.A = true;
    }
    if(jc.button.btn.B){
        input.B = true;
    }
    if(jc.button.btn.X){
        input.X = true;
    }
    if(jc.button.btn.Y){
        input.Y = true;
    }
    if(jc.button.btn.SL_r){
        input.SL = true;
    }
    if(jc.button.btn.SR_r){
        input.SR = true;
    }
    if(jc.button.btn.Home){
        input.Home = true;
    }

    input.stick.vx = jc.stick.x * -1;
    input.stick.vy = 0;
    input.stick.vz = jc.stick.y * -1;

    return input;
}
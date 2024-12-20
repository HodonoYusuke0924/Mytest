#include <joyconlib.h>
#include <stdbool.h>

#include "system_struct.h"
#include "system_func.h"

#include "client_func.h"


joyconlib_t jc;

/*??��?��?��?��?��???????????*/
void joycon_init()
{
    joycon_err err = joycon_open(&jc, JOYCON_R);
    if (JOYCON_ERR_NONE != err) {
        printf("joycon open failed:%d\n", err);
        return;
    }
    else {
        printf("joycon connected");
    }
}

/*??��?��?��?��?��??�?�?*/
void joycon_end()
{
    joycon_close(&jc);
}

/*??��?��?��?��?��?????????????��????????*/
Vector joycon_vector()
{
    Vector axis;

    float tx = 0, ty = 0, tz = 0;

    joycon_get_state(&jc);

    // ??��?��?��?��????��??�?�?
    printf("Sensor value is (%f,%f,%f)\n", jc.axis[0].acc_x, jc.axis[0].acc_y, jc.axis[0].acc_z);
    printf("                (%f,%f,%f)\n", jc.axis[0].gyro_x, jc.axis[0].gyro_y, jc.axis[0].gyro_z);

    // ??��??????���?
    float dx = 0, dy = 0, dz = 0;
    for (int i = 2; i >= 0; i--) {
        float x = dx, y = dy, z = dz;
        dx = jc.axis[i].gyro_x + z * jc.axis[i].gyro_y - y * jc.axis[i].gyro_z;
        dy = -z * jc.axis[i].gyro_x + jc.axis[i].gyro_y + x * jc.axis[i].gyro_z;
        dz = y * jc.axis[i].gyro_x - x * jc.axis[i].gyro_y + jc.axis[i].gyro_z;

        // �????
        tx += dx * 0.005;
        ty += dy * 0.005;
        tz += dz * 0.005;
    }
    // ???�???????�????????????��??±2?????�??????????�?�?
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

Joycon_Input joycon_input()
{
    Joycon_Input input = {false,false,false,false,false,false, {0,0,0,}};

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

    input.stick.vx = jc.stick.x;
    input.stick.vy = jc.stick.y;
    input.stick.vz = 0;

    if(jc.button.btn.Home){
        //mainloop = false;
    }
    return input;
}
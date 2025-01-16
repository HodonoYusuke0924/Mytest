/*****************************************************************
ファイル名	: system_func.h
内容		: ゲームシステムの関数
*****************************************************************/
#include "system_struct.h"

/*ここに各ファイルの必要な関数を記載*/

/*system_joycon.c*/
extern void joycon_init(void);
extern void joycon_end(void);
extern Vector joycon_vector(void);
extern Joycon_Input joycon_input(void);

/*system_init.c*/
extern int system_init(Game_Info*);

/*timer.c*/
extern void wait(int);
extern void* timer_task(void*);

/*system_main.c*/
extern void system_main(Game_Info*);

/*system_physics*/
extern bool collision(Object_Info, Object_Info);
extern float distance_squared(Object_Info, Object_Info);
extern Point player_move(Player_Info);
extern Vector player_axis(Player_Info);
extern void acceleration(Field_Info, Object_Info, Vector, PositionCallback);
extern void generate_terrain();
extern bool under_ground(Object_Info*);
extern void adjust_point(Object_Info*);
extern void gravity(Object_Info*, float, float*);
extern void item_movement(Item_Info*);
extern void get_item(Player_Info*, Item_Info*);
extern Item_Info *search_nearest_item(Game_Info*, int, int);

/*system_calculate*/
extern float  glm_rad(float);
extern Vector normalize(Vector);
extern Vector cross(Vector, Vector);
extern float  dot(Vector, Vector);
extern Vector get_forward(float);
extern Vector get_right(float);
extern Point  rotate_point(Point, Point, Rotate, Vector);

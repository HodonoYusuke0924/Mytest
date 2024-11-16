#include "system_struct.h"

/*ここに各ファイルの必要な関数を記載*/

/*main.c*/
extern bool mainloop;

/*system_joycon.c*/
extern void joycon_init(void);
extern void joycon_end(void);
extern Vector joycon_vector(void);
extern Joycon_Input joycon_input(void);

/*system_init.c*/
extern int system_init(Item_Fixed_Info, Player_Fixed_Info, Game_Info);

/*timer.c*/
extern void wait(int);

/*system_main.c*/
extern void system_main(Item_Fixed_Info, Item_Flex_Info, Player_Fixed_Info, Player_Flex_Info, Game_Info);

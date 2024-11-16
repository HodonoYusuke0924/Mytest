/*****************************************************************
ファイル名	: main.c
内容		: ゲームのメインループ
*****************************************************************/

#include "System/system_struct.h"
#include "System/system_func.h"

bool mainloop = true;
    
int main(void)
{
    Thread_Info thread[UT_NUM];

    Item_Fixed_Info item_fixed;
    Item_Flex_Info item_flex;
        
    Player_Fixed_Info player_fixed;
    Player_Flex_Info player_flex;

    Game_Info game_info;

    /*初期化処理*/
    if(system_init(item_fixed, player_fixed, game_info)){
        return -1;
    }
    printf("\033[?25l"); // カーソル非表示
    
    /*メインループ*/
    while(mainloop){
        system_main(item_fixed, item_flex, player_fixed, player_flex, game_info);
    };

    /*終了処理*/
    joycon_end();
    printf("\n");
    printf("\033[?25h"); // カーソル再表示
    
    return 0;
}
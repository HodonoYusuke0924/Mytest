/*****************************************************************
ファイル名	: main.c
内容		: ゲームのメインループ
*****************************************************************/

#include "System/system_struct.h"
#include "System/system_func.h"

int main(void)
{
    bool play = true;
    
    Thread_Info thread[UT_NUM];

    Item_Fixed_Info item_fixed;
    Item_Flex_Info item_flex;
        
    Player_Fixed_Info player_fixed;
    Player_Flex_Info player_flex;

    Game_Info game_info;

    if(system_init(item_fixed, player_fixed, game_info)){
        return -1;
    }

    while(play){
        system_main(item_fixed, item_flex, player_fixed, player_flex, game_info);
    };

    return 0;
}
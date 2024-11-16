/*****************************************************************
ファイル名	: system_main.h
内容		: ゲームシステムの構造体
*****************************************************************/

#include "system_struct.h"
#include "system_func.h"

void system_title(Player_Flex_Info);

void system_main(Item_Fixed_Info item_fixed, Item_Flex_Info item_flex,
                 Player_Fixed_Info player_fixed, Player_Flex_Info player_flex,
                 Game_Info game_info)
{
    switch (game_info.scene)
    {
    case TITLE:
        system_title(player_flex);
        break;
    case PRE_PHASE1:
    case PHASE1:
    case END_PHASE1:
    case PRE_PHASE2:
    case PHASE2:
    case END_PHASE2:
    case RESULT:
        break;
    default:
        break;
    }
}

void system_title(Player_Flex_Info player)
{
    Player_Flex_Info compare;

    compare.input = player.input;

    player.input = joycon_input();

    if(memcmp(&compare.input, &player.input, sizeof(Joycon_Input))){
        printf("A : %d\n", player.input.A);
        printf("B : %d\n", player.input.B);
        printf("X : %d\n", player.input.X);
        printf("Y : %d\n", player.input.Y);
        printf("SL: %d\n", player.input.SL);
        printf("SR: %d\n", player.input.SR);
        printf("x : %f, y : %f\n", player.input.stick.vx, player.input.stick.vy);
    }
}
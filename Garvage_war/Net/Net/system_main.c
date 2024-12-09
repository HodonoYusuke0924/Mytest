/*****************************************************************
ファイル名	: system_main.c
内容	    : システムの各シーン処理 
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
    player.input = joycon_input();
    player.vector = joycon_vector();

    printf("\033[H\033[J"); // 画面全体をクリアしてカーソルを左上に移動
    printf("A: %d, B: %d, X: %d, Y: %d, SL: %d, SR: %d, Stick: (%.2f, %.2f)\n",
        player.input.A, player.input.B, player.input.X, player.input.Y,
        player.input.SL, player.input.SR, player.input.stick.vx, player.input.stick.vy);
    //printf("Vector: (vx, vy, vz) = (%.2f, %.2f, %.2f)\n",
    //    player.vector.vx, player.vector.vy, player.vector.vz);
    fflush(stdout);

}
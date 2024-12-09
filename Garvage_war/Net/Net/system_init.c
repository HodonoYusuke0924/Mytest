/*****************************************************************
ファイル名	: system_init.c
内容		: ゲームシステムの初期化処理
*****************************************************************/

#include "system_struct.h"
#include "system_func.h"

int system_init(Item_Fixed_Info item_option, Player_Fixed_Info player_option, Game_Info game_info)
{
    game_info.scene = TITLE;

    joycon_init();

    return 0;   //初期化に成功したら0を返す
}
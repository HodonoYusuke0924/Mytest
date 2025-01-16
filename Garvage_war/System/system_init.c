/*****************************************************************
ファイル名	: system_init.c
内容		: ゲームシステムの初期化処理
*****************************************************************/

#include "system_struct.h"
#include "system_func.h"

//重要！：ファイルからデータを読み込めるようにする
void itemdata_init(Item_Info info[ITEM_ID_MAX]) {
    for(int i = 1; i < ITEM_ID_MAX; i++){
        //仮
        info[i].stts    = IS_Disable;
        info[i].fixed.id = i;
        info[i].fixed.weight = 5;
        info[i].fixed.initspeed = 2.0f;
        info[i].fixed.damage = 10;
        info[i].fixed.effect = IE_Noting;
        info[i].fixed.radius = 1.0f;
        
        info[i].obj.size.vx = 1.0f;
        info[i].obj.size.vy = 1.0f;
        info[i].obj.size.vz = 1.0f;
    }
}

void fielddata_init(Field_Info info[FN_NUM]) {
    for(int i = 0; i < FN_NUM; i++){
        info[i].force.vx = 0.0f;
        info[i].force.vy = 9.8f;
        info[i].force.vz = 0.0f;
        info[i].id       = i;
        info[i].resist   = 0.0f;
        generate_terrain(info[i].map);
    }
}

void playerdata_init(Player_Info info[PLAYER_ID_MAX]) {
    for(int i = 0; i < PLAYER_ID_MAX; i++){
        //要調整
        info[i].fixed.max_cooltime = 2.0f;
        info[i].fixed.limit_weight = 30;
        info[i].fixed.max_weight   = 50;
        info[i].fixed.max_hp       = 50;

        info[i].stts = PS_DISCONNECT;
        info[i].obj.size.vx = 2.0f;
        info[i].obj.size.vy = 4.0f;
        info[i].obj.size.vz = 2.0f;
        for(int j = 0; j < SLOT_MAX; j++){
            info[i].having_item[j].stts = IS_Disable;
        }
    }

}

int system_init(Game_Info *game_info)
{
    game_info->scene = TITLE;
    game_info->loop = true;
    
    game_info->client_num = 1;
    
    game_info->rule.limittime_phase1 = 200;
    game_info->rule.limittime_phase1 = 200;
    
    itemdata_init(game_info->item_info);
    fielddata_init(game_info->field_info);
    playerdata_init(game_info->player_info);
    
    return 0;   //初期化に成功したら0を返す
}
/*****************************************************************
ファイル名	: system_main.c
内容	    : システムの各シーン処理，サーバ側
*****************************************************************/

#include "system_struct.h"
#include "system_func.h"

void title(Game_Info *info){
    if(0/*ここにクライアントを受け取る関数*/){
        //client_setup(info->player_info); //仮関数：クライアントのセットアップ
        info->client_num++;
        /*クライアント情報を送信する関数*/
    }

    if(info->client_num > 1){
        for(int i = 0; i < info->client_num; i++){
            //クライアントが二人以上でいずれかのクライアントがAボタンを押すとシーン切り替え
            if(info->player_info[i].input.A){
                info->scene++;
            }
        }
    }
}

void setup_phase(Game_Info *info){

}

/*フェーズ１，２のプレイヤの挙動*/
void main_phase(Game_Info *info){
    //各プレイヤの入力
    for(int i = 0; i < info->client_num; i++){
        if(info->player_info[i].input.X){
            //向きの更新
            info->player_info[i].obj.axis = player_axis(info->player_info[i]);
        }else{
            //位置の更新
            info->player_info[i].obj.center = player_move(info->player_info[i]);
        }
        adjust_point(&info->player_info[i].obj);
        if(info->player_info[i].input.A){
            //アイテムを投げる
            info->player_info[i].having_item[0].active = true;
        }
        if(info->player_info[i].having_item[0].active){
            item_movement(&info->player_info[i].having_item[0]);
        }
        if(info->player_info[i].input.B){
            //アイテムを拾う
            Item_Info *near = search_nearest_item(info, info->player_info[i].fixed.id, IS_OnField);
            get_item(&info->player_info[i], near);
        }

        //当たり判定
        Item_Info *throwing = search_nearest_item(info, info->player_info[i].fixed.id, IS_Throwing);
        if(collision(info->player_info[i].obj, throwing->obj) && throwing->stts == IS_Throwing){
            info->player_info[i].hp -= throwing->fixed.damage;
            throwing->stts = IS_Disable;
        }

        if(info->player_info[i].hp <= 0){
            info->player_info[i].result = LOSE;
            info->scene = RESULT;
        }
    }

}

void system_main(Game_Info *game_info)
{
    switch (game_info->scene)
    {
    case TITLE:
        title(game_info);
        break;
    case PRE_PHASE1:
        setup_phase(game_info);
    case PHASE1:
    /*
        // スレッドを作成
        pthread_create(&game_info->thread[UT_TIMER].thread, NULL, timer_task, &game_info->rule.limittime_phase1);
        // スレッドの終了を待つ
        pthread_join(game_info->thread[UT_TIMER].thread, NULL);
    */
        main_phase(&game_info);
        break;
    case END_PHASE1:
    case PRE_PHASE2:
    case PHASE2:
    /*
        // スレッドを作成
        pthread_create(&game_info->thread[UT_TIMER].thread, NULL, timer_task, &game_info->rule.limittime_phase2);
        // スレッドの終了を待つ
        pthread_join(game_info->thread[UT_TIMER].thread, NULL);
    */
    case END_PHASE2:
    case RESULT:
        for(int i = 0; i < game_info->client_num; i++){
            if(game_info->player_info[i].result == UNDESIDE){
                game_info->player_info[i].result = WIN;
            }
            if(game_info->player_info[i].input.A){
                game_info->scene = TITLE;
            }
        }
        game_info->scene = TITLE;
        break;
    default:
        break;
    }
}
/*****************************************************************
ファイル名	: server_main.c
内容		: サーバのメインループ
*****************************************************************/

#include "System/system_struct.h"
#include "System/system_func.h"

int main(void)
{
    Game_Info game_info;

    /*初期化処理*/
    if(system_init(&game_info)){
        return -1;
    }
    
    /*メインループ*/
    game_info.loop = true;
    
    while(game_info.loop){
        //クライアントが一人以上で入力を受け付ける
        if(game_info.client_num > 0){
            for(int i = 0; i < game_info.client_num; i++){
                //receve_client(&game_info.player_info[i].input); //仮関数
                if(game_info.player_info[i].input.Home){
                    game_info.loop = false;
                }
            }
        }
        system_main(&game_info);
        //send_client(game_info); //仮関数

        usleep(DELTA_TIME * NANO_SEC);
    };

    /*終了処理*/

    return 0;
}
#include "common.h"
#include "client_func.h"
#include "system_func.h"
#include "system_struct.h"
#include<sys/socket.h>
#include<netdb.h>

int finishFlag = 1;

    Player_Fixed_Info *Pinfo[MAX_CLIENTS] = {0};
    Game_Info *Ginfo[MAX_CLIENTS] = {0};

void GameMain(int pos)
{
    printf("動作検出 : %d\n", pos);
    while (finishFlag) {
        pos = 0;
        for(pos; pos <= MAX_CLIENTS; pos++){
        client_SendPlayerFixedInfo(pos, Pinfo[pos]);
        client_RecvPlayerFixedInfo(pos, Pinfo[pos]);
        client_SendGameInfo(pos, Ginfo[pos]);
        client_RecvGameInfo(pos, Ginfo[pos]);
        }
    }

}
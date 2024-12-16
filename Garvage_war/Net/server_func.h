/*****************************************************************
�ե�����̾	: server_func.h
��ǽ		: �����С��γ����ؿ������
*****************************************************************/

#ifndef _SERVER_FUNC_H_
#define _SERVER_FUNC_H_

#include"server_common.h"
#include "system_func.h"

/* server_net.c */
extern int SetUpServer(int num);
extern void Ending(void);
extern int RecvIntData(int pos,int *intData);
extern void SendData(int pos,void *data,int dataSize);
extern int SendRecvManager(void);
extern void server_SendStructData(int pos, void *data, int dataSize);
extern int server_RecvStructData(int pos, void *data, int dataSize);
extern void server_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info);
extern int server_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info);
extern void server_SendGameInfo(int pos, Game_Info *info);
extern int server_RecvGameInfo(int pos, Game_Info *info);

/* server_command.c */
extern int ExecuteCommand(char command,int pos);

/*server_game.c*/
extern void GameMain_server(int pos);

#endif

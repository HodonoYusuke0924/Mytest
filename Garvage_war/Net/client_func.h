/*****************************************************************
�ե�����̾	: client_func.h
��ǽ		: ���饤����Ȥγ����ؿ������
*****************************************************************/

#ifndef _CLIENT_FUNC_H_
#define _CLIENT_FUNC_H_

#include"common.h"
#include "system_func.h"

/* client_net.c */
extern int SetUpClient(char* hostName,int *clientID,int *num,char clientName[][MAX_NAME_SIZE]);
extern void CloseSoc(void);
extern int RecvIntData(int *intData);
extern void SendData(void *data,int dataSize);
extern int SendRecvManager(void);
extern void client_SendStructData(int pos, void *data, int dataSize);
extern int client_RecvStructData(int pos, void *data, int dataSize);
extern void client_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info);
extern int client_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info);
extern void client_SendGameInfo(int pos, Game_Info *info);
extern int client_RecvGameInfo(int pos, Game_Info *info);

/* client_win.c */
extern int InitWindows(int clientID,int num,char name[][MAX_NAME_SIZE]);
extern void DestroyWindow(void);
extern void WindowEvent(int num);
extern void Gomain(int x,int y,int r);

/* client_command.c */
extern int ExecuteCommand(char command);
extern void SendPositionCommand(int pos);
extern void SendEndCommand(void);
extern void SendJoyconCommand(int pos);


/*client_game.c*/
extern void GameMain(int pos);

#endif

/*****************************************************************
ファイ??E?	: server_net.c
機能		: サーバーのネット??E璽?処理
*****************************************************************/

#include"server_common.h"
#include"server_func.h"
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#include"system_struct.h"
#include"system_func.h"

/* クライアントを表す構造体 */
typedef struct{
	int		fd;
	char	name[MAX_NAME_SIZE];
}CLIENT;

static CLIENT	gClients[MAX_CLIENTS];	/* クライアント */
static int	gClientNum;					/* クライアント??E*/

static fd_set	gMask;					/* select()用のマスク */
static int	gWidth;						/* gMask中のチェックすべきビット??E*/

static int MultiAccept(int request_soc,int num);
static void Enter(int pos, int fd);
static void SetMask(int maxfd);
static void SendAllName(void);
static int RecvData(int pos,void *data,int dataSize);
static void server_SendStructData(int pos, void *data, int dataSize);
static int server_RecvStructData(int pos, void *data, int dataSize);
static void server_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info);
static int server_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info);
static void server_SendGameInfo(int pos, Game_Info *info);
static int server_RecvGameInfo(int pos, Game_Info *info);

/*****************************************************************
関数名	: SetUpServer
機能	: クライアントとのコネクションを設立し，
		  ユーザーの名前の送受信を行う
引??E: int		num		  : クライアント??E出力	: コネクションに失敗した時-1,成功した時0
*****************************************************************/
int SetUpServer(int num)
{
    struct sockaddr_in	server;
    int			request_soc;
    int                 maxfd;
    int			val = 1;
 
    /* 引き数チェック */
    assert(0<num && num<=MAX_CLIENTS);

    gClientNum = num;
    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* ソケットを笹椪す??E*/
    if((request_soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"Socket allocation failed\n");
		return -1;
    }
    setsockopt(request_soc,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    /* ソケットに名前をつけ??E*/
    if(bind(request_soc,(struct sockaddr*)&server,sizeof(server))==-1){
		fprintf(stderr,"Cannot bind\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Successfully bind!\n");
    
    /* クライアントからの接続要求を待つ */
    if(listen(request_soc, gClientNum) == -1){
		fprintf(stderr,"Cannot listen\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Listen OK\n");

    /* クライアントと接続す??E*/
    maxfd = MultiAccept(request_soc, gClientNum);
    close(request_soc);
    if(maxfd == -1)return -1;

    /* 全クライアントの全ユーザー名を送??E*/
    SendAllName();

    /* select()のためのマスク値を設定す??E*/
    SetMask(maxfd);

    return 0;
}

/*****************************************************************
関数名	: SendRecvManager
機能	: サーバーから送ら??E討�たデータを処�?す??E引??E: なし
出力	: プ?暠哀薀狃?了コマンドが送ら??E討�た�?0を返す．
		  そ??E奮阿?1を返す
*****************************************************************/
int SendRecvManager(void)
{
    char	command;
    fd_set	readOK;
    int		i;
    int		endFlag = 1;

    readOK = gMask;
    /* クライアントからデータが届いてい??E?調べ??E*/
    if(select(gWidth,&readOK,NULL,NULL,NULL) < 0){
        /* エラーが起こった */
        return endFlag;
    }

    for(i=0;i<gClientNum;i++){
		if(FD_ISSET(gClients[i].fd,&readOK)){
	    	/* クライアントからデータが届いていた */
	    	/* コマンドを読み込??E*/
			RecvData(i,&command,sizeof(char));
	    	/* コマンドに対す??E萢?を行う */
	    	endFlag = ExecuteCommand(command,i);
	    	if(endFlag == 0)break;
		}
    }
    return endFlag;
}

/*****************************************************************
関数名	: RecvIntData
機能	: クライアントからint型のデータを受け取??E引??E: int		pos	        : クライアント番??E		  int		*intData	: 受信したデータ
出力	: 受け取ったバイト??E*****************************************************************/
int RecvIntData(int pos,int *intData)
{
    int n,tmp;
    
    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum);
    assert(intData!=NULL);

    n = RecvData(pos,&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

/*****************************************************************
関数名	: SendData
機能	: クライアントにデータを送??E引??E: int	   pos		: クライアント番??E							  ALL_CLIENTSが指定さ??E浸?には全員に送??E		  void	   *data	: 送??E如璽?
		  int	   dataSize	: 送??E如璽燭離汽ぅ?
出力	: なし
*****************************************************************/
void SendData(int pos,void *data,int dataSize)
{
    int	i;
   
    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum || pos==ALL_CLIENTS);
    assert(data!=NULL);
    assert(0<dataSize);

    if(pos == ALL_CLIENTS){
    	/* 全クライアントにデータを送??E*/
		for(i=0;i<gClientNum;i++){
			write(gClients[i].fd,data,dataSize);
		}
    }
    else{
		write(gClients[pos].fd,data,dataSize);
    }
}

// ?\?????????M?p
void server_SendStructData(int pos, void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(pos, data, dataSize); // SendData?????p
}

/*****************************************************************
関数名	: Ending
機能	: 全クライアントとのコネクションを切断す??E引??E: なし
出力	: なし
*****************************************************************/
void Ending(void)
{
    int	i;

    printf("... Connection closed\n");
    for(i=0;i<gClientNum;i++)close(gClients[i].fd);
}

/*****
static
*****/
/*****************************************************************
関数名	: MultiAccept
機能	: 接続要求のあったクライアントとのコネクションを設立す??E引??E: int		request_soc	: ソケット
		  int		num     	: クライアント??E出力	: ソケットディスク??E廛?
*****************************************************************/
static int MultiAccept(int request_soc,int num)
{
    int	i,j;
    int	fd;
    
    for(i=0;i<num;i++){
		if((fd = accept(request_soc,NULL,NULL)) == -1){
			fprintf(stderr,"Accept error\n");
			for(j=i-1;j>=0;j--)close(gClients[j].fd);
			return -1;
		}
		Enter(i,fd);
    }
    return fd;
}

/*****************************************************************
関数名	: Enter
機能	: クライアントのユーザー名を受信す??E引??E: int		pos		: クライアント番??E		  int		fd		: ソケットディスク??E廛?
出力	: なし
*****************************************************************/
static void Enter(int pos, int fd)
{
	/* クライアントのユーザー名を受信す??E*/
	read(fd,gClients[pos].name,MAX_NAME_SIZE);
#ifndef NDEBUG
	printf("%s is accepted\n",gClients[pos].name);
#endif
	gClients[pos].fd = fd;
}

/*****************************************************************
関数名	: SetMask
機能	: int		maxfd	: ソケットディスク??E廛燭虜蚤臙?
引??E: なし
出力	: なし
*****************************************************************/
static void SetMask(int maxfd)
{
    int	i;

    gWidth = maxfd+1;

    FD_ZERO(&gMask);    
    for(i=0;i<gClientNum;i++)FD_SET(gClients[i].fd,&gMask);
}

/*****************************************************************
関数名	: SendAllName
機能	: 全クライアントに全ユーザー名を送??E引??E: なし
出力	: なし
*****************************************************************/
static void SendAllName(void)
{
  int	i,j,tmp1,tmp2;

    tmp2 = htonl(gClientNum);
    for(i=0;i<gClientNum;i++){
		tmp1 = htonl(i);
		SendData(i,&tmp1,sizeof(int));
		SendData(i,&tmp2,sizeof(int));
		for(j=0;j<gClientNum;j++){
			SendData(i,gClients[j].name,MAX_NAME_SIZE);
		}
	}
}

/*****************************************************************
関数名	: RecvData
機能	: クライアントからデータを受け取??E引??E: int		pos	        : クライアント番??E		  void		*data		: 受信したデータ
		  int		dataSize	: 受信す??E如璽燭離汽ぅ?
出力	: 受け取ったバイト??E*****************************************************************/
static int RecvData(int pos,void *data,int dataSize)
{
    int n;
    
    /* 引き数チェック */
    assert(0<=pos && pos<gClientNum);
    assert(data!=NULL);
    assert(0<dataSize);

    n = read(gClients[pos].fd,data,dataSize);
    
    return n;
}

int server_RecvStructData(int pos, void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    return RecvData(pos, data, dataSize); // RecvData?????p
}

// ?v???C???[?????????M
void server_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info) {
    server_SendStructData(pos, info, sizeof(Player_Fixed_Info));
}

// ?v???C???[?????????M
int server_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info) {
    return server_RecvStructData(pos, info, sizeof(Player_Fixed_Info));
}

// ?Q?[???????????M
void server_SendGameInfo(int pos, Game_Info *info) {
    server_SendStructData(pos, info, sizeof(Game_Info));
}

// ?Q?[???????????M
int server_RecvGameInfo(int pos, Game_Info *info) {
    return server_RecvStructData(pos, info, sizeof(Game_Info));
}

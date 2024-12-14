/*****************************************************************
ファイ??E?	: client_net.c
機能		: クライアントのネット??E璽?処理
*****************************************************************/

#include"common.h"
#include"client_func.h"
#include<sys/socket.h>
#include<netdb.h>

#include"system_struct.h"
#include"system_func.h"

#define	BUF_SIZE	100

static int	gSocket;	/* ソケット */
static fd_set	gMask;	/* select()用のマスク */
static int	gWidth;		/* gMask中ののチェックすべきビット??E*/

static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE]);
static void SetMask(void);
static int RecvData(void *data,int dataSize);
static void client_SendStructData(int pos, void *data, int dataSize);
static int client_RecvStructData(int pos, void *data, int dataSize);
static void client_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info);
static int client_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info);
static void client_SendGameInfo(int pos, Game_Info *info);
static int client_RecvGameInfo(int pos, Game_Info *info);


/*****************************************************************
関数名	: SetUpClient
機能	: サーバーとのコネクションを設立し，
		  ユーザーの名前の送受信を行う
引??E: char	*hostName		: ホスト
		  int	*num			: 全クライアント??E		  char	clientNames[][]		: 全クライアントのユーザー名
出力	: コネクションに失敗した時-1,成功した時0
*****************************************************************/
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    struct hostent	*servHost;
    struct sockaddr_in	server;
    int			len;
    char		str[BUF_SIZE];

    /* ホスト名からホスト情報を得??E*/
    if((servHost = gethostbyname(hostName))==NULL){
		fprintf(stderr,"Unknown host\n");
		return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* ソケットを笹椪す??E*/
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"socket allocation failed\n");
		return -1;
    }

    /* サーバーと接続す??E*/
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
		fprintf(stderr,"cannot connect\n");
		close(gSocket);
		return -1;
    }
    fprintf(stderr,"connected\n");

    /* 名前を読み込みサーバーに送??E*/
    do{
		printf("Enter Your Name\n");
		fgets(str,BUF_SIZE,stdin);
		len = strlen(str)-1;
		str[len]='\0';
    }while(len>MAX_NAME_SIZE-1 || len==0);
    SendData(str,MAX_NAME_SIZE);

    printf("Please Wait\n");

    /* 全クライアントのユーザー名を得??E*/
    GetAllName(clientID,num,clientNames);

    /* select()のためのマスク値を設定す??E*/
    SetMask();
    
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
    fd_set	readOK;
    char	command;
    int		i;
    int		endFlag = 1;
    struct timeval	timeout;

    /* select()の待ち時間を設定す??E*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 20;

    readOK = gMask;
    /* サーバーからデータが届いてい??E?調べ??E*/
    select(gWidth,&readOK,NULL,NULL,&timeout);
    if(FD_ISSET(gSocket,&readOK)){
		/* サーバーからデータが届いていた */
    	/* コマンドを読み込??E*/
		RecvData(&command,sizeof(char));
    	/* コマンドに対す??E萢?を行う */
		endFlag = ExecuteCommand(command);
    }
    return endFlag;
}

/*****************************************************************
関数名	: RecvIntData
機能	: サーバーからint型のデータを受け取??E引??E: int		*intData	: 受信したデータ
出力	: 受け取ったバイト??E*****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;
    
    /* 引き数チェック */
    assert(intData!=NULL);

    n = RecvData(&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

// ?\?????????M?p
int client_RecvStructData(int pos, void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    return RecvData(data, dataSize); // RecvData?????p
}

/*****************************************************************
関数名	: SendData
機能	: サーバーにデータを送??E引??E: void		*data		: 送??E如璽?
		  int		dataSize	: 送??E如璽燭離汽ぅ?
出力	: なし
*****************************************************************/
void SendData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

void client_SendStructData(int pos, void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(data, dataSize);

}
/*****************************************************************
関数名	: CloseSoc
機能	: サーバーとのコネクションを切断す??E引??E: なし
出力	: なし
*****************************************************************/
void CloseSoc(void)
{
    printf("...Connection closed\n");
    close(gSocket);
}

/*****
static
*****/
/*****************************************************************
関数名	: GetAllName
機能	: サーバーから全クライアントのユーザー名を受信す??E引??E: int		*num			: クライアント??E		  char		clientNames[][]	: 全クライアントのユーザー名
出力	: なし
*****************************************************************/
static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    int	i;

    /* クライアント番号の読み込み */
    RecvIntData(clientID);
    /* クライアント数の読み込み */
    RecvIntData(num);

    /* 全クライアントのユーザー名を読み込??E*/
    for(i=0;i<(*num);i++){
		RecvData(clientNames[i],MAX_NAME_SIZE);
    }
#ifndef NDEBUG
    printf("#####\n");
    printf("client number = %d\n",(*num));
    for(i=0;i<(*num);i++){
		printf("%d:%s\n",i,clientNames[i]);
    }
#endif
}

/*****************************************************************
関数名	: SetMask
機能	: select()のためのマスク値を設定す??E引??E: なし
出力	: なし
*****************************************************************/
static void SetMask(void)
{
    int	i;

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
関数名	: RecvData
機能	: サーバーからデータを受け取??E引??E: void		*data		: 受信したデータ
		  int		dataSize	: 受信す??E如璽燭離汽ぅ?
出力	: 受け取ったバイト??E*****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* 引き数チェック */
    assert(data != NULL);
    assert(0 < dataSize);

    return read(gSocket,data,dataSize);
}


// ?v???C???[?????????M
void client_SendPlayerFixedInfo(int pos, Player_Fixed_Info *info) {
    client_SendStructData(pos, info, sizeof(Player_Fixed_Info));
}

// ?v???C???[?????????M
int client_RecvPlayerFixedInfo(int pos, Player_Fixed_Info *info) {
    return client_RecvStructData(pos, info, sizeof(Player_Fixed_Info));
}

// ?Q?[???????????M
void client_SendGameInfo(int pos, Game_Info *info) {
    client_SendStructData(pos, info, sizeof(Game_Info));
}

// ?Q?[???????????M
int client_RecvGameInfo(int pos, Game_Info *info) {
    return client_RecvStructData(pos, info, sizeof(Game_Info));
}

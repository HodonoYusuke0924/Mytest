/*****************************************************************
¥Õ¥¡¥¤??E?	: client_net.c
µ¡Ç½		: ¥¯¥é¥¤¥¢¥ó¥È¤Î¥Í¥Ã¥È??E¼¥?½èÍý
*****************************************************************/

#include"common.h"
#include"client_func.h"
#include<sys/socket.h>
#include<netdb.h>

#include"system_struct.h"
#include"system_func.h"

#define	BUF_SIZE	100

static int	gSocket;	/* ¥½¥±¥Ã¥È */
static fd_set	gMask;	/* select()ÍÑ¤Î¥Þ¥¹¥¯ */
static int	gWidth;		/* gMaskÃæ¤Î¤Î¥Á¥§¥Ã¥¯¤¹¤Ù¤­¥Ó¥Ã¥È??E*/

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
´Ø¿ôÌ¾	: SetUpClient
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤È¤Î¥³¥Í¥¯¥·¥ç¥ó¤òÀßÎ©¤·¡¤
		  ¥æ¡¼¥¶¡¼¤ÎÌ¾Á°¤ÎÁ÷¼õ¿®¤ò¹Ô¤¦
°ú??E: char	*hostName		: ¥Û¥¹¥È
		  int	*num			: Á´¥¯¥é¥¤¥¢¥ó¥È??E		  char	clientNames[][]		: Á´¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾
½ÐÎÏ	: ¥³¥Í¥¯¥·¥ç¥ó¤Ë¼ºÇÔ¤·¤¿»þ-1,À®¸ù¤·¤¿»þ0
*****************************************************************/
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    struct hostent	*servHost;
    struct sockaddr_in	server;
    int			len;
    char		str[BUF_SIZE];

    /* ¥Û¥¹¥ÈÌ¾¤«¤é¥Û¥¹¥È¾ðÊó¤òÆÀ??E*/
    if((servHost = gethostbyname(hostName))==NULL){
		fprintf(stderr,"Unknown host\n");
		return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* ¥½¥±¥Ã¥È¤òºûÜ®¤¹??E*/
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"socket allocation failed\n");
		return -1;
    }

    /* ¥µ¡¼¥Ð¡¼¤ÈÀÜÂ³¤¹??E*/
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
		fprintf(stderr,"cannot connect\n");
		close(gSocket);
		return -1;
    }
    fprintf(stderr,"connected\n");

    /* Ì¾Á°¤òÆÉ¤ß¹þ¤ß¥µ¡¼¥Ð¡¼¤ËÁ÷??E*/
    do{
		printf("Enter Your Name\n");
		fgets(str,BUF_SIZE,stdin);
		len = strlen(str)-1;
		str[len]='\0';
    }while(len>MAX_NAME_SIZE-1 || len==0);
    SendData(str,MAX_NAME_SIZE);

    printf("Please Wait\n");

    /* Á´¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾¤òÆÀ??E*/
    GetAllName(clientID,num,clientNames);

    /* select()¤Î¤¿¤á¤Î¥Þ¥¹¥¯ÃÍ¤òÀßÄê¤¹??E*/
    SetMask();
    
    return 0;
}

/*****************************************************************
´Ø¿ôÌ¾	: SendRecvManager
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤«¤éÁ÷¤é??EÆ¤­¤¿¥Ç¡¼¥¿¤ò½èÍ?¤¹??E°ú??E: ¤Ê¤·
½ÐÎÏ	: ¥×?Â×°¥é¥à½?Î»¥³¥Þ¥ó¥É¤¬Á÷¤é??EÆ¤­¤¿»?0¤òÊÖ¤¹¡¥
		  ¤½??EÊ³°¤?1¤òÊÖ¤¹
*****************************************************************/
int SendRecvManager(void)
{
    fd_set	readOK;
    char	command;
    int		i;
    int		endFlag = 1;
    struct timeval	timeout;

    /* select()¤ÎÂÔ¤Á»þ´Ö¤òÀßÄê¤¹??E*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 20;

    readOK = gMask;
    /* ¥µ¡¼¥Ð¡¼¤«¤é¥Ç¡¼¥¿¤¬ÆÏ¤¤¤Æ¤¤??E?Ä´¤Ù??E*/
    select(gWidth,&readOK,NULL,NULL,&timeout);
    if(FD_ISSET(gSocket,&readOK)){
		/* ¥µ¡¼¥Ð¡¼¤«¤é¥Ç¡¼¥¿¤¬ÆÏ¤¤¤Æ¤¤¤¿ */
    	/* ¥³¥Þ¥ó¥É¤òÆÉ¤ß¹þ??E*/
		RecvData(&command,sizeof(char));
    	/* ¥³¥Þ¥ó¥É¤ËÂÐ¤¹??EèÍ?¤ò¹Ô¤¦ */
		endFlag = ExecuteCommand(command);
    }
    return endFlag;
}

/*****************************************************************
´Ø¿ôÌ¾	: RecvIntData
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤«¤éint·¿¤Î¥Ç¡¼¥¿¤ò¼õ¤±¼è??E°ú??E: int		*intData	: ¼õ¿®¤·¤¿¥Ç¡¼¥¿
½ÐÎÏ	: ¼õ¤±¼è¤Ã¤¿¥Ð¥¤¥È??E*****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;
    
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
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
´Ø¿ôÌ¾	: SendData
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤Ë¥Ç¡¼¥¿¤òÁ÷??E°ú??E: void		*data		: Á÷??EÇ¡¼¥?
		  int		dataSize	: Á÷??EÇ¡¼¥¿¤Î¥µ¥¤¥?
½ÐÎÏ	: ¤Ê¤·
*****************************************************************/
void SendData(void *data,int dataSize)
{
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

void client_SendStructData(int pos, void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(data, dataSize);

}
/*****************************************************************
´Ø¿ôÌ¾	: CloseSoc
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤È¤Î¥³¥Í¥¯¥·¥ç¥ó¤òÀÚÃÇ¤¹??E°ú??E: ¤Ê¤·
½ÐÎÏ	: ¤Ê¤·
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
´Ø¿ôÌ¾	: GetAllName
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤«¤éÁ´¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾¤ò¼õ¿®¤¹??E°ú??E: int		*num			: ¥¯¥é¥¤¥¢¥ó¥È??E		  char		clientNames[][]	: Á´¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾
½ÐÎÏ	: ¤Ê¤·
*****************************************************************/
static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    int	i;

    /* ¥¯¥é¥¤¥¢¥ó¥ÈÈÖ¹æ¤ÎÆÉ¤ß¹þ¤ß */
    RecvIntData(clientID);
    /* ¥¯¥é¥¤¥¢¥ó¥È¿ô¤ÎÆÉ¤ß¹þ¤ß */
    RecvIntData(num);

    /* Á´¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾¤òÆÉ¤ß¹þ??E*/
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
´Ø¿ôÌ¾	: SetMask
µ¡Ç½	: select()¤Î¤¿¤á¤Î¥Þ¥¹¥¯ÃÍ¤òÀßÄê¤¹??E°ú??E: ¤Ê¤·
½ÐÎÏ	: ¤Ê¤·
*****************************************************************/
static void SetMask(void)
{
    int	i;

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
´Ø¿ôÌ¾	: RecvData
µ¡Ç½	: ¥µ¡¼¥Ð¡¼¤«¤é¥Ç¡¼¥¿¤ò¼õ¤±¼è??E°ú??E: void		*data		: ¼õ¿®¤·¤¿¥Ç¡¼¥¿
		  int		dataSize	: ¼õ¿®¤¹??EÇ¡¼¥¿¤Î¥µ¥¤¥?
½ÐÎÏ	: ¼õ¤±¼è¤Ã¤¿¥Ð¥¤¥È??E*****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
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

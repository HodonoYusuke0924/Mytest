/*****************************************************************
¥Õ¥¡¥¤¥E¾	: server_net.c
µ¡Ç½		: ¥µ¡¼¥Ğ¡¼¤Î¥Í¥Ã¥È¥E¼¥¯½èÍı
*****************************************************************/

#include"server_common.h"
#include"server_func.h"
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

/* ¥¯¥é¥¤¥¢¥ó¥È¤òÉ½¤¹¹½Â¤ÂÎ */
typedef struct{
	int		fd;
	char	name[MAX_NAME_SIZE];
}CLIENT;

static CLIENT	gClients[MAX_CLIENTS];	/* ¥¯¥é¥¤¥¢¥ó¥È */
static int	gClientNum;					/* ¥¯¥é¥¤¥¢¥ó¥È¿E*/

static fd_set	gMask;					/* select()ÍÑ¤Î¥Ş¥¹¥¯ */
static int	gWidth;						/* gMaskÃæ¤Î¥Á¥§¥Ã¥¯¤¹¤Ù¤­¥Ó¥Ã¥È¿E*/

static int MultiAccept(int request_soc,int num);
static void Enter(int pos, int fd);
static void SetMask(int maxfd);
static void SendAllName(void);
static int RecvData(int pos,void *data,int dataSize);
static void RecvStructData(void *data, int dataSize);
static int RecvStructData(void *data, int dataSize);
static void SendPlayerFixedInfo(Player_Fixed_Info *info);
static int RecvPlayerFixedInfo(Player_Fixed_Info *info);
static void SendGameInfo(Game_Info *info);
static int RecvGameInfo(Game_Info *info);

/*****************************************************************
´Ø¿ôÌ¾	: SetUpServer
µ¡Ç½	: ¥¯¥é¥¤¥¢¥ó¥È¤È¤Î¥³¥Í¥¯¥·¥ç¥ó¤òÀßÎ©¤·¡¤
		  ¥æ¡¼¥¶¡¼¤ÎÌ¾Á°¤ÎÁ÷¼õ¿®¤ò¹Ô¤¦
°ú¿E: int		num		  : ¥¯¥é¥¤¥¢¥ó¥È¿E½ĞÎÏ	: ¥³¥Í¥¯¥·¥ç¥ó¤Ë¼ºÇÔ¤·¤¿»ş-1,À®¸ù¤·¤¿»ş0
*****************************************************************/
int SetUpServer(int num)
{
    struct sockaddr_in	server;
    int			request_soc;
    int                 maxfd;
    int			val = 1;
 
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<num && num<=MAX_CLIENTS);

    gClientNum = num;
    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* ¥½¥±¥Ã¥È¤òºûÜ®¤¹¤E*/
    if((request_soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"Socket allocation failed\n");
		return -1;
    }
    setsockopt(request_soc,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    /* ¥½¥±¥Ã¥È¤ËÌ¾Á°¤ò¤Ä¤±¤E*/
    if(bind(request_soc,(struct sockaddr*)&server,sizeof(server))==-1){
		fprintf(stderr,"Cannot bind\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Successfully bind!\n");
    
    /* ¥¯¥é¥¤¥¢¥ó¥È¤«¤é¤ÎÀÜÂ³Í×µá¤òÂÔ¤Ä */
    if(listen(request_soc, gClientNum) == -1){
		fprintf(stderr,"Cannot listen\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Listen OK\n");

    /* ¥¯¥é¥¤¥¢¥ó¥È¤ÈÀÜÂ³¤¹¤E*/
    maxfd = MultiAccept(request_soc, gClientNum);
    close(request_soc);
    if(maxfd == -1)return -1;

    /* Á´¥¯¥é¥¤¥¢¥ó¥È¤ÎÁ´¥æ¡¼¥¶¡¼Ì¾¤òÁ÷¤E*/
    SendAllName();

    /* select()¤Î¤¿¤á¤Î¥Ş¥¹¥¯ÃÍ¤òÀßÄê¤¹¤E*/
    SetMask(maxfd);

    return 0;
}

/*****************************************************************
´Ø¿ôÌ¾	: SendRecvManager
µ¡Ç½	: ¥µ¡¼¥Ğ¡¼¤«¤éÁ÷¤é¤EÆ¤­¤¿¥Ç¡¼¥¿¤ò½èÍı¤¹¤E°ú¿E: ¤Ê¤·
½ĞÎÏ	: ¥×¥úÁ°¥é¥à½ªÎ»¥³¥Ş¥ó¥É¤¬Á÷¤é¤EÆ¤­¤¿»ş0¤òÊÖ¤¹¡¥
		  ¤½¤EÊ³°¤Ï1¤òÊÖ¤¹
*****************************************************************/
int SendRecvManager(void)
{
    char	command;
    fd_set	readOK;
    int		i;
    int		endFlag = 1;

    readOK = gMask;
    /* ¥¯¥é¥¤¥¢¥ó¥È¤«¤é¥Ç¡¼¥¿¤¬ÆÏ¤¤¤Æ¤¤¤E«Ä´¤Ù¤E*/
    if(select(gWidth,&readOK,NULL,NULL,NULL) < 0){
        /* ¥¨¥é¡¼¤¬µ¯¤³¤Ã¤¿ */
        return endFlag;
    }

    for(i=0;i<gClientNum;i++){
		if(FD_ISSET(gClients[i].fd,&readOK)){
	    	/* ¥¯¥é¥¤¥¢¥ó¥È¤«¤é¥Ç¡¼¥¿¤¬ÆÏ¤¤¤Æ¤¤¤¿ */
	    	/* ¥³¥Ş¥ó¥É¤òÆÉ¤ß¹ş¤E*/
			RecvData(i,&command,sizeof(char));
	    	/* ¥³¥Ş¥ó¥É¤ËÂĞ¤¹¤EèÍı¤ò¹Ô¤¦ */
	    	endFlag = ExecuteCommand(command,i);
	    	if(endFlag == 0)break;
		}
    }
    return endFlag;
}

/*****************************************************************
´Ø¿ôÌ¾	: RecvIntData
µ¡Ç½	: ¥¯¥é¥¤¥¢¥ó¥È¤«¤éint·¿¤Î¥Ç¡¼¥¿¤ò¼õ¤±¼è¤E°ú¿E: int		pos	        : ¥¯¥é¥¤¥¢¥ó¥ÈÈÖ¹E		  int		*intData	: ¼õ¿®¤·¤¿¥Ç¡¼¥¿
½ĞÎÏ	: ¼õ¤±¼è¤Ã¤¿¥Ğ¥¤¥È¿E*****************************************************************/
int RecvIntData(int pos,int *intData)
{
    int n,tmp;
    
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<=pos && pos<gClientNum);
    assert(intData!=NULL);

    n = RecvData(pos,&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

/*****************************************************************
´Ø¿ôÌ¾	: SendData
µ¡Ç½	: ¥¯¥é¥¤¥¢¥ó¥È¤Ë¥Ç¡¼¥¿¤òÁ÷¤E°ú¿E: int	   pos		: ¥¯¥é¥¤¥¢¥ó¥ÈÈÖ¹E							  ALL_CLIENTS¤¬»ØÄê¤µ¤E¿»ş¤Ë¤ÏÁ´°÷¤ËÁ÷¤E		  void	   *data	: Á÷¤EÇ¡¼¥¿
		  int	   dataSize	: Á÷¤EÇ¡¼¥¿¤Î¥µ¥¤¥º
½ĞÎÏ	: ¤Ê¤·
*****************************************************************/
void SendData(int pos,void *data,int dataSize)
{
    int	i;
   
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<=pos && pos<gClientNum || pos==ALL_CLIENTS);
    assert(data!=NULL);
    assert(0<dataSize);

    if(pos == ALL_CLIENTS){
    	/* Á´¥¯¥é¥¤¥¢¥ó¥È¤Ë¥Ç¡¼¥¿¤òÁ÷¤E*/
		for(i=0;i<gClientNum;i++){
			write(gClients[i].fd,data,dataSize);
		}
    }
    else{
		write(gClients[pos].fd,data,dataSize);
    }
}

// \‘¢‘Ì‚Ì‘—M—p
void SendStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(data, dataSize); // SendData‚ğ—˜—p
}

/*****************************************************************
´Ø¿ôÌ¾	: Ending
µ¡Ç½	: Á´¥¯¥é¥¤¥¢¥ó¥È¤È¤Î¥³¥Í¥¯¥·¥ç¥ó¤òÀÚÃÇ¤¹¤E°ú¿E: ¤Ê¤·
½ĞÎÏ	: ¤Ê¤·
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
´Ø¿ôÌ¾	: MultiAccept
µ¡Ç½	: ÀÜÂ³Í×µá¤Î¤¢¤Ã¤¿¥¯¥é¥¤¥¢¥ó¥È¤È¤Î¥³¥Í¥¯¥·¥ç¥ó¤òÀßÎ©¤¹¤E°ú¿E: int		request_soc	: ¥½¥±¥Ã¥È
		  int		num     	: ¥¯¥é¥¤¥¢¥ó¥È¿E½ĞÎÏ	: ¥½¥±¥Ã¥È¥Ç¥£¥¹¥¯¥E×¥¿
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
´Ø¿ôÌ¾	: Enter
µ¡Ç½	: ¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾¤ò¼õ¿®¤¹¤E°ú¿E: int		pos		: ¥¯¥é¥¤¥¢¥ó¥ÈÈÖ¹E		  int		fd		: ¥½¥±¥Ã¥È¥Ç¥£¥¹¥¯¥E×¥¿
½ĞÎÏ	: ¤Ê¤·
*****************************************************************/
static void Enter(int pos, int fd)
{
	/* ¥¯¥é¥¤¥¢¥ó¥È¤Î¥æ¡¼¥¶¡¼Ì¾¤ò¼õ¿®¤¹¤E*/
	read(fd,gClients[pos].name,MAX_NAME_SIZE);
#ifndef NDEBUG
	printf("%s is accepted\n",gClients[pos].name);
#endif
	gClients[pos].fd = fd;
}

/*****************************************************************
´Ø¿ôÌ¾	: SetMask
µ¡Ç½	: int		maxfd	: ¥½¥±¥Ã¥È¥Ç¥£¥¹¥¯¥E×¥¿¤ÎºÇÂçÃÍ
°ú¿E: ¤Ê¤·
½ĞÎÏ	: ¤Ê¤·
*****************************************************************/
static void SetMask(int maxfd)
{
    int	i;

    gWidth = maxfd+1;

    FD_ZERO(&gMask);    
    for(i=0;i<gClientNum;i++)FD_SET(gClients[i].fd,&gMask);
}

/*****************************************************************
´Ø¿ôÌ¾	: SendAllName
µ¡Ç½	: Á´¥¯¥é¥¤¥¢¥ó¥È¤ËÁ´¥æ¡¼¥¶¡¼Ì¾¤òÁ÷¤E°ú¿E: ¤Ê¤·
½ĞÎÏ	: ¤Ê¤·
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
´Ø¿ôÌ¾	: RecvData
µ¡Ç½	: ¥¯¥é¥¤¥¢¥ó¥È¤«¤é¥Ç¡¼¥¿¤ò¼õ¤±¼è¤E°ú¿E: int		pos	        : ¥¯¥é¥¤¥¢¥ó¥ÈÈÖ¹E		  void		*data		: ¼õ¿®¤·¤¿¥Ç¡¼¥¿
		  int		dataSize	: ¼õ¿®¤¹¤EÇ¡¼¥¿¤Î¥µ¥¤¥º
½ĞÎÏ	: ¼õ¤±¼è¤Ã¤¿¥Ğ¥¤¥È¿E*****************************************************************/
static int RecvData(int pos,void *data,int dataSize)
{
    int n;
    
    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<=pos && pos<gClientNum);
    assert(data!=NULL);
    assert(0<dataSize);

    n = read(gClients[pos].fd,data,dataSize);
    
    return n;
}

int RecvStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    return RecvData(data, dataSize); // RecvData‚ğ—˜—p
}

// ƒvƒŒƒCƒ„[î•ñ‚ğ‘—M
void SendPlayerFixedInfo(Player_Fixed_Info *info) {
    SendStructData(info, sizeof(Player_Fixed_Info));
}

// ƒvƒŒƒCƒ„[î•ñ‚ğóM
int RecvPlayerFixedInfo(Player_Fixed_Info *info) {
    return RecvStructData(info, sizeof(Player_Fixed_Info));
}

// ƒQ[ƒ€î•ñ‚ğ‘—M
void SendGameInfo(Game_Info *info) {
    SendStructData(info, sizeof(Game_Info));
}

// ƒQ[ƒ€î•ñ‚ğóM
int RecvGameInfo(Game_Info *info) {
    return RecvStructData(info, sizeof(Game_Info));
}

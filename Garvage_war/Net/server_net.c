/*****************************************************************
�ե���??E?	: server_net.c
��ǽ		: �����С��Υͥå�??E��?����
*****************************************************************/

#include"server_common.h"
#include"server_func.h"
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#include"system_struct.h"
#include"system_func.h"

/* ���饤����Ȥ�ɽ����¤�� */
typedef struct{
	int		fd;
	char	name[MAX_NAME_SIZE];
}CLIENT;

static CLIENT	gClients[MAX_CLIENTS];	/* ���饤����� */
static int	gClientNum;					/* ���饤�����??E*/

static fd_set	gMask;					/* select()�ѤΥޥ��� */
static int	gWidth;						/* gMask��Υ����å����٤��ӥå�??E*/

static int MultiAccept(int request_soc,int num);
static void Enter(int pos, int fd);
static void SetMask(int maxfd);
static void SendAllName(void);
static int RecvData(int pos,void *data,int dataSize);

/*****************************************************************
�ؿ�̾	: SetUpServer
��ǽ	: ���饤����ȤȤΥ��ͥ���������Ω����
		  �桼������̾������������Ԥ�
��??E: int		num		  : ���饤�����??E����	: ���ͥ������˼��Ԥ�����-1,����������0
*****************************************************************/
int SetUpServer(int num)
{
    struct sockaddr_in	server;
    int			request_soc;
    int                 maxfd;
    int			val = 1;
 
    /* �����������å� */
    assert(0<num && num<=MAX_CLIENTS);

    gClientNum = num;
    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* �����åȤ��ܮ��??E*/
    if((request_soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"Socket allocation failed\n");
		return -1;
    }
    setsockopt(request_soc,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    /* �����åȤ�̾����Ĥ�??E*/
    if(bind(request_soc,(struct sockaddr*)&server,sizeof(server))==-1){
		fprintf(stderr,"Cannot bind\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Successfully bind!\n");
    
    /* ���饤����Ȥ������³�׵���Ԥ� */
    if(listen(request_soc, gClientNum) == -1){
		fprintf(stderr,"Cannot listen\n");
		close(request_soc);
		return -1;
    }
    fprintf(stderr,"Listen OK\n");

    /* ���饤����Ȥ���³��??E*/
    maxfd = MultiAccept(request_soc, gClientNum);
    close(request_soc);
    if(maxfd == -1)return -1;

    /* �����饤����Ȥ����桼����̾����??E*/
    SendAllName();

    /* select()�Τ���Υޥ����ͤ����ꤹ??E*/
    SetMask(maxfd);

    return 0;
}

/*****************************************************************
�ؿ�̾	: SendRecvManager
��ǽ	: �����С���������??EƤ����ǡ�������?��??E��??E: �ʤ�
����	: ��?��װ���?λ���ޥ�ɤ�����??EƤ����?0���֤���
		  ��??Eʳ��?1���֤�
*****************************************************************/
int SendRecvManager(void)
{
    char	command;
    fd_set	readOK;
    int		i;
    int		endFlag = 1;

    readOK = gMask;
    /* ���饤����Ȥ���ǡ������Ϥ��Ƥ�??E?Ĵ��??E*/
    if(select(gWidth,&readOK,NULL,NULL,NULL) < 0){
        /* ���顼�������ä� */
        return endFlag;
    }

    for(i=0;i<gClientNum;i++){
		if(FD_ISSET(gClients[i].fd,&readOK)){
	    	/* ���饤����Ȥ���ǡ������Ϥ��Ƥ��� */
	    	/* ���ޥ�ɤ��ɤ߹�??E*/
			RecvData(i,&command,sizeof(char));
	    	/* ���ޥ�ɤ��Ф�??E��?��Ԥ� */
	    	endFlag = ExecuteCommand(command,i);
	    	if(endFlag == 0)break;
		}
    }
    return endFlag;
}

/*****************************************************************
�ؿ�̾	: RecvIntData
��ǽ	: ���饤����Ȥ���int���Υǡ����������??E��??E: int		pos	        : ���饤�������??E		  int		*intData	: ���������ǡ���
����	: ������ä��Х���??E*****************************************************************/
int RecvIntData(int pos,int *intData)
{
    int n,tmp;
    
    /* �����������å� */
    assert(0<=pos && pos<gClientNum);
    assert(intData!=NULL);

    n = RecvData(pos,&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

/*****************************************************************
�ؿ�̾	: SendData
��ǽ	: ���饤����Ȥ˥ǡ�������??E��??E: int	   pos		: ���饤�������??E							  ALL_CLIENTS�����ꤵ??E��?�ˤ���������??E		  void	   *data	: ��??Eǡ��?
		  int	   dataSize	: ��??Eǡ����Υ����?
����	: �ʤ�
*****************************************************************/
void SendData(int pos,void *data,int dataSize)
{
    int	i;
   
    /* �����������å� */
    assert(0<=pos && pos<gClientNum || pos==ALL_CLIENTS);
    assert(data!=NULL);
    assert(0<dataSize);

    if(pos == ALL_CLIENTS){
    	/* �����饤����Ȥ˥ǡ�������??E*/
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
�ؿ�̾	: Ending
��ǽ	: �����饤����ȤȤΥ��ͥ����������Ǥ�??E��??E: �ʤ�
����	: �ʤ�
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
�ؿ�̾	: MultiAccept
��ǽ	: ��³�׵�Τ��ä����饤����ȤȤΥ��ͥ���������Ω��??E��??E: int		request_soc	: �����å�
		  int		num     	: ���饤�����??E����	: �����åȥǥ�����??Eץ?
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
�ؿ�̾	: Enter
��ǽ	: ���饤����ȤΥ桼����̾�������??E��??E: int		pos		: ���饤�������??E		  int		fd		: �����åȥǥ�����??Eץ?
����	: �ʤ�
*****************************************************************/
static void Enter(int pos, int fd)
{
	/* ���饤����ȤΥ桼����̾�������??E*/
	read(fd,gClients[pos].name,MAX_NAME_SIZE);
#ifndef NDEBUG
	printf("%s is accepted\n",gClients[pos].name);
#endif
	gClients[pos].fd = fd;
}

/*****************************************************************
�ؿ�̾	: SetMask
��ǽ	: int		maxfd	: �����åȥǥ�����??Eץ��κ����?
��??E: �ʤ�
����	: �ʤ�
*****************************************************************/
static void SetMask(int maxfd)
{
    int	i;

    gWidth = maxfd+1;

    FD_ZERO(&gMask);    
    for(i=0;i<gClientNum;i++)FD_SET(gClients[i].fd,&gMask);
}

/*****************************************************************
�ؿ�̾	: SendAllName
��ǽ	: �����饤����Ȥ����桼����̾����??E��??E: �ʤ�
����	: �ʤ�
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
�ؿ�̾	: RecvData
��ǽ	: ���饤����Ȥ���ǡ����������??E��??E: int		pos	        : ���饤�������??E		  void		*data		: ���������ǡ���
		  int		dataSize	: ������??Eǡ����Υ����?
����	: ������ä��Х���??E*****************************************************************/
static int RecvData(int pos,void *data,int dataSize)
{
    int n;
    
    /* �����������å� */
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

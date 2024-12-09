/*****************************************************************
�ե�����E�	: client_net.c
��ǽ		: ���饤����ȤΥͥåȥ�E�������
*****************************************************************/

#include"common.h"
#include"client_func.h"
#include<sys/socket.h>
#include<netdb.h>

#define	BUF_SIZE	100

static int	gSocket;	/* �����å� */
static fd_set	gMask;	/* select()�ѤΥޥ��� */
static int	gWidth;		/* gMask��ΤΥ����å����٤��ӥåȿ�E*/

static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE]);
static void SetMask(void);
static int RecvData(void *data,int dataSize);
static void SendStructData(void *data, int dataSize);
static int RecvStructData(void *data, int dataSize);
static void SendPlayerFixedInfo(Player_Fixed_Info *info);
static int RecvPlayerFixedInfo(Player_Fixed_Info *info);
static void SendGameInfo(Game_Info *info);
static int RecvGameInfo(Game_Info *info);


/*****************************************************************
�ؿ�̾	: SetUpClient
��ǽ	: �����С��ȤΥ��ͥ���������Ω����
		  �桼������̾������������Ԥ�
����E: char	*hostName		: �ۥ���
		  int	*num			: �����饤����ȿ�E		  char	clientNames[][]		: �����饤����ȤΥ桼����̾
����	: ���ͥ������˼��Ԥ�����-1,����������0
*****************************************************************/
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    struct hostent	*servHost;
    struct sockaddr_in	server;
    int			len;
    char		str[BUF_SIZE];

    /* �ۥ���̾����ۥ��Ⱦ��������E*/
    if((servHost = gethostbyname(hostName))==NULL){
		fprintf(stderr,"Unknown host\n");
		return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* �����åȤ��ܮ����E*/
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"socket allocation failed\n");
		return -1;
    }

    /* �����С�����³����E*/
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
		fprintf(stderr,"cannot connect\n");
		close(gSocket);
		return -1;
    }
    fprintf(stderr,"connected\n");

    /* ̾�����ɤ߹��ߥ����С�������E*/
    do{
		printf("Enter Your Name\n");
		fgets(str,BUF_SIZE,stdin);
		len = strlen(str)-1;
		str[len]='\0';
    }while(len>MAX_NAME_SIZE-1 || len==0);
    SendData(str,MAX_NAME_SIZE);

    printf("Please Wait\n");

    /* �����饤����ȤΥ桼����̾������E*/
    GetAllName(clientID,num,clientNames);

    /* select()�Τ���Υޥ����ͤ����ꤹ��E*/
    SetMask();
    
    return 0;
}

/*****************************************************************
�ؿ�̾	: SendRecvManager
��ǽ	: �����С��������餁EƤ����ǡ������������E����E: �ʤ�
����	: �ץ�����ཪλ���ޥ�ɤ����餁EƤ�����0���֤���
		  ����Eʳ���1���֤�
*****************************************************************/
int SendRecvManager(void)
{
    fd_set	readOK;
    char	command;
    int		i;
    int		endFlag = 1;
    struct timeval	timeout;

    /* select()���Ԥ����֤����ꤹ��E*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 20;

    readOK = gMask;
    /* �����С�����ǡ������Ϥ��Ƥ���E�Ĵ�٤�E*/
    select(gWidth,&readOK,NULL,NULL,&timeout);
    if(FD_ISSET(gSocket,&readOK)){
		/* �����С�����ǡ������Ϥ��Ƥ��� */
    	/* ���ޥ�ɤ��ɤ߹���E*/
		RecvData(&command,sizeof(char));
    	/* ���ޥ�ɤ��Ф���E�����Ԥ� */
		endFlag = ExecuteCommand(command);
    }
    return endFlag;
}

/*****************************************************************
�ؿ�̾	: RecvIntData
��ǽ	: �����С�����int���Υǡ���������褁E����E: int		*intData	: ���������ǡ���
����	: ������ä��Х��ȿ�E*****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;
    
    /* �����������å� */
    assert(intData!=NULL);

    n = RecvData(&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

// �\���̂̎�M�p
int RecvStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    return RecvData(data, dataSize); // RecvData�𗘗p
}

/*****************************************************************
�ؿ�̾	: SendData
��ǽ	: �����С��˥ǡ���������E����E: void		*data		: ����Eǡ���
		  int		dataSize	: ����Eǡ����Υ�����
����	: �ʤ�
*****************************************************************/
void SendData(void *data,int dataSize)
{
    /* �����������å� */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

void SendStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(data, dataSize); // SendData�𗘗p
}

/*****************************************************************
�ؿ�̾	: CloseSoc
��ǽ	: �����С��ȤΥ��ͥ����������Ǥ���E����E: �ʤ�
����	: �ʤ�
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
�ؿ�̾	: GetAllName
��ǽ	: �����С����������饤����ȤΥ桼����̾���������E����E: int		*num			: ���饤����ȿ�E		  char		clientNames[][]	: �����饤����ȤΥ桼����̾
����	: �ʤ�
*****************************************************************/
static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    int	i;

    /* ���饤������ֹ���ɤ߹��� */
    RecvIntData(clientID);
    /* ���饤����ȿ����ɤ߹��� */
    RecvIntData(num);

    /* �����饤����ȤΥ桼����̾���ɤ߹���E*/
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
�ؿ�̾	: SetMask
��ǽ	: select()�Τ���Υޥ����ͤ����ꤹ��E����E: �ʤ�
����	: �ʤ�
*****************************************************************/
static void SetMask(void)
{
    int	i;

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
�ؿ�̾	: RecvData
��ǽ	: �����С�����ǡ���������褁E����E: void		*data		: ���������ǡ���
		  int		dataSize	: ��������Eǡ����Υ�����
����	: ������ä��Х��ȿ�E*****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* �����������å� */
    assert(data != NULL);
    assert(0 < dataSize);

    return read(gSocket,data,dataSize);
}


// �v���C���[���𑗐M
void SendPlayerFixedInfo(Player_Fixed_Info *info) {
    SendStructData(info, sizeof(Player_Fixed_Info));
}

// �v���C���[������M
int RecvPlayerFixedInfo(Player_Fixed_Info *info) {
    return RecvStructData(info, sizeof(Player_Fixed_Info));
}

// �Q�[�����𑗐M
void SendGameInfo(Game_Info *info) {
    SendStructData(info, sizeof(Game_Info));
}

// �Q�[��������M
int RecvGameInfo(Game_Info *info) {
    return RecvStructData(info, sizeof(Game_Info));
}

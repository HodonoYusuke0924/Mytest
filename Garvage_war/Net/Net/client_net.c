/*****************************************************************
е╒ебедеБE╛	: client_net.c
╡б╟╜		: епещедевеєе╚д╬е═е├е╚еБE╝еп╜ш═¤
*****************************************************************/

#include"common.h"
#include"client_func.h"
#include<sys/socket.h>
#include<netdb.h>

#define	BUF_SIZE	100

static int	gSocket;	/* е╜е▒е├е╚ */
static fd_set	gMask;	/* select()═╤д╬е▐е╣еп */
static int	gWidth;		/* gMask├цд╬д╬е┴езе├епд╣д┘дне╙е├е╚┐БE*/

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
┤╪┐Ї╠╛	: SetUpClient
╡б╟╜	: е╡б╝е╨б╝д╚д╬е│е═епе╖ечеєдЄ└▀╬йд╖бд
		  ецб╝е╢б╝д╬╠╛┴░д╬┴ў╝ї┐одЄ╣╘дж
░·┐БE: char	*hostName		: е█е╣е╚
		  int	*num			: ┴┤епещедевеєе╚┐БE		  char	clientNames[][]		: ┴┤епещедевеєе╚д╬ецб╝е╢б╝╠╛
╜╨╬╧	: е│е═епе╖ечеєд╦╝║╟╘д╖д┐╗■-1,└о╕∙д╖д┐╗■0
*****************************************************************/
int SetUpClient(char *hostName,int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    struct hostent	*servHost;
    struct sockaddr_in	server;
    int			len;
    char		str[BUF_SIZE];

    /* е█е╣е╚╠╛длдще█е╣е╚╛Ё╩єдЄ╞└дБE*/
    if((servHost = gethostbyname(hostName))==NULL){
		fprintf(stderr,"Unknown host\n");
		return -1;
    }

    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy(servHost->h_addr,(char*)&server.sin_addr,servHost->h_length);

    /* е╜е▒е├е╚дЄ║√▄од╣дБE*/
    if((gSocket = socket(AF_INET,SOCK_STREAM,0)) < 0){
		fprintf(stderr,"socket allocation failed\n");
		return -1;
    }

    /* е╡б╝е╨б╝д╚└▄┬│д╣дБE*/
    if(connect(gSocket,(struct sockaddr*)&server,sizeof(server)) == -1){
		fprintf(stderr,"cannot connect\n");
		close(gSocket);
		return -1;
    }
    fprintf(stderr,"connected\n");

    /* ╠╛┴░дЄ╞╔д▀╣■д▀е╡б╝е╨б╝д╦┴ўдБE*/
    do{
		printf("Enter Your Name\n");
		fgets(str,BUF_SIZE,stdin);
		len = strlen(str)-1;
		str[len]='\0';
    }while(len>MAX_NAME_SIZE-1 || len==0);
    SendData(str,MAX_NAME_SIZE);

    printf("Please Wait\n");

    /* ┴┤епещедевеєе╚д╬ецб╝е╢б╝╠╛дЄ╞└дБE*/
    GetAllName(clientID,num,clientNames);

    /* select()д╬д┐дсд╬е▐е╣еп├═дЄ└▀─ъд╣дБE*/
    SetMask();
    
    return 0;
}

/*****************************************************************
┤╪┐Ї╠╛	: SendRecvManager
╡б╟╜	: е╡б╝е╨б╝длдщ┴ўдщдБE╞днд┐е╟б╝е┐дЄ╜ш═¤д╣дБE░·┐БE: д╩д╖
╜╨╬╧	: е╫е·┴░ещер╜к╬╗е│е▐еєе╔дм┴ўдщдБE╞днд┐╗■0дЄ╩╓д╣бе
		  д╜дБE╩│░д╧1дЄ╩╓д╣
*****************************************************************/
int SendRecvManager(void)
{
    fd_set	readOK;
    char	command;
    int		i;
    int		endFlag = 1;
    struct timeval	timeout;

    /* select()д╬┬╘д┴╗■┤╓дЄ└▀─ъд╣дБE*/
    timeout.tv_sec = 0;
    timeout.tv_usec = 20;

    readOK = gMask;
    /* е╡б╝е╨б╝длдще╟б╝е┐дм╞╧ддд╞дддБEл─┤д┘дБE*/
    select(gWidth,&readOK,NULL,NULL,&timeout);
    if(FD_ISSET(gSocket,&readOK)){
		/* е╡б╝е╨б╝длдще╟б╝е┐дм╞╧ддд╞ддд┐ */
    	/* е│е▐еєе╔дЄ╞╔д▀╣■дБE*/
		RecvData(&command,sizeof(char));
    	/* е│е▐еєе╔д╦┬╨д╣дБEш═¤дЄ╣╘дж */
		endFlag = ExecuteCommand(command);
    }
    return endFlag;
}

/*****************************************************************
┤╪┐Ї╠╛	: RecvIntData
╡б╟╜	: е╡б╝е╨б╝длдщint╖┐д╬е╟б╝е┐дЄ╝їд▒╝шдБE░·┐БE: int		*intData	: ╝ї┐од╖д┐е╟б╝е┐
╜╨╬╧	: ╝їд▒╝шд├д┐е╨еде╚┐БE*****************************************************************/
int RecvIntData(int *intData)
{
    int n,tmp;
    
    /* ░·дн┐Їе┴езе├еп */
    assert(intData!=NULL);

    n = RecvData(&tmp,sizeof(int));
    (*intData) = ntohl(tmp);
    
    return n;
}

// Н\СвС╠В╠ОєРMЧp
int RecvStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    return RecvData(data, dataSize); // RecvDataВЁЧШЧp
}

/*****************************************************************
┤╪┐Ї╠╛	: SendData
╡б╟╜	: е╡б╝е╨б╝д╦е╟б╝е┐дЄ┴ўдБE░·┐БE: void		*data		: ┴ўдБE╟б╝е┐
		  int		dataSize	: ┴ўдБE╟б╝е┐д╬е╡еде║
╜╨╬╧	: д╩д╖
*****************************************************************/
void SendData(void *data,int dataSize)
{
    /* ░·дн┐Їе┴езе├еп */
    assert(data != NULL);
    assert(0 < dataSize);

    write(gSocket,data,dataSize);
}

void SendStructData(void *data, int dataSize) {
    assert(data != NULL && dataSize > 0);
    SendData(data, dataSize); // SendDataВЁЧШЧp
}

/*****************************************************************
┤╪┐Ї╠╛	: CloseSoc
╡б╟╜	: е╡б╝е╨б╝д╚д╬е│е═епе╖ечеєдЄ└┌├╟д╣дБE░·┐БE: д╩д╖
╜╨╬╧	: д╩д╖
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
┤╪┐Ї╠╛	: GetAllName
╡б╟╜	: е╡б╝е╨б╝длдщ┴┤епещедевеєе╚д╬ецб╝е╢б╝╠╛дЄ╝ї┐од╣дБE░·┐БE: int		*num			: епещедевеєе╚┐БE		  char		clientNames[][]	: ┴┤епещедевеєе╚д╬ецб╝е╢б╝╠╛
╜╨╬╧	: д╩д╖
*****************************************************************/
static void GetAllName(int *clientID,int *num,char clientNames[][MAX_NAME_SIZE])
{
    int	i;

    /* епещедевеєе╚╚╓╣цд╬╞╔д▀╣■д▀ */
    RecvIntData(clientID);
    /* епещедевеєе╚┐Їд╬╞╔д▀╣■д▀ */
    RecvIntData(num);

    /* ┴┤епещедевеєе╚д╬ецб╝е╢б╝╠╛дЄ╞╔д▀╣■дБE*/
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
┤╪┐Ї╠╛	: SetMask
╡б╟╜	: select()д╬д┐дсд╬е▐е╣еп├═дЄ└▀─ъд╣дБE░·┐БE: д╩д╖
╜╨╬╧	: д╩д╖
*****************************************************************/
static void SetMask(void)
{
    int	i;

    FD_ZERO(&gMask);
    FD_SET(gSocket,&gMask);

    gWidth = gSocket+1;
}

/*****************************************************************
┤╪┐Ї╠╛	: RecvData
╡б╟╜	: е╡б╝е╨б╝длдще╟б╝е┐дЄ╝їд▒╝шдБE░·┐БE: void		*data		: ╝ї┐од╖д┐е╟б╝е┐
		  int		dataSize	: ╝ї┐од╣дБE╟б╝е┐д╬е╡еде║
╜╨╬╧	: ╝їд▒╝шд├д┐е╨еде╚┐БE*****************************************************************/
int RecvData(void *data,int dataSize)
{
    /* ░·дн┐Їе┴езе├еп */
    assert(data != NULL);
    assert(0 < dataSize);

    return read(gSocket,data,dataSize);
}


// ГvГМГCГДБ[ПюХёВЁСЧРM
void SendPlayerFixedInfo(Player_Fixed_Info *info) {
    SendStructData(info, sizeof(Player_Fixed_Info));
}

// ГvГМГCГДБ[ПюХёВЁОєРM
int RecvPlayerFixedInfo(Player_Fixed_Info *info) {
    return RecvStructData(info, sizeof(Player_Fixed_Info));
}

// ГQБ[ГАПюХёВЁСЧРM
void SendGameInfo(Game_Info *info) {
    SendStructData(info, sizeof(Game_Info));
}

// ГQБ[ГАПюХёВЁОєРM
int RecvGameInfo(Game_Info *info) {
    return RecvStructData(info, sizeof(Game_Info));
}

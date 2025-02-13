/*****************************************************************
ファイル名	: client_command.c
機能		: クライアントのコマンド処理
*****************************************************************/

#include"common.h"
#include"client_func.h"

static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void RecvGomain(void);
static void RecvJoyconData(void);

/*****************************************************************
関数名	: ExecuteCommand
機能	: サーバーから送られてきたコマンドを元に，
		  引き数を受信し，実行する
引数	: char	command		: コマンド
出力	: プログラム終了コマンドがおくられてきた時には0を返す．
		  それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command)
{
    int	endFlag = 1;
#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n",command);
#endif
    switch(command){
		case END_COMMAND:
			endFlag = 0;
			break;
	    case MAIN_COMMAND:
			RecvGomain();
			break;
		case JOYCON_COMMAND:
			RecvJoyconData();
			break;
    }
    return endFlag;
}


/*****************************************************************
関数名	: SendMainCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendMainCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

    printf("main関数起動\n");

#ifndef NDEBUG
    printf("#####\n");
    printf("SendMainCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, MAIN_COMMAND,&dataSize);
    /* クライアント番号のセット */
    SetIntData2DataBlock(data,pos,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


void SendJoyconCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

    printf("main関数起動\n");

#ifndef NDEBUG
    printf("#####\n");
    printf("SendMainCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, MAIN_COMMAND,&dataSize);
    /* クライアント番号のセット */
    SetIntData2DataBlock(data,pos,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****************************************************************
関数名	: SendEndCommand
機能	: プログラムの終了を知らせるために，
		  サーバーにデータを送る
引数	: なし
出力	: なし
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,END_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****
static
*****/
/*****************************************************************
関数名	: SetIntData2DataBlock
機能	: int 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetIntData2DataBlock(void *data,int intData,int *dataSize)
{
    int tmp;

    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    tmp = htonl(intData);

    /* int 型のデータを送信用データの最後にコピーする */
    memcpy(data + (*dataSize),&tmp,sizeof(int));
    /* データサイズを増やす */
    (*dataSize) += sizeof(int);
}

/*****************************************************************
関数名	: SetCharData2DataBlock
機能	: char 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetCharData2DataBlock(void *data,char charData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* char 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}

/*****************************************************************
関数名	: RecvGomain
機能	: 受け取ったプレイヤーの座標情報を相互に表示，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvGomain(void)
{
    int	pos,x,y;

    /*クライアントナンバー取得*/
    RecvIntData(&pos);

    /* 座標に対する引き数を受信する */
    RecvIntData(&x);
    RecvIntData(&y);

    /* 円を表示する ：要変更　受け取ったプレイヤーのmain関数を実行*/
    Gomain(x,y,5);
    //メイン関数挿入
    printf("%d\n",pos);
    GameMain(pos);

}

/*****************************************************************
関数名	: RecvJoyconleData
機能	: ジョイコンデータ受信
引数	: なし
出力	: なし
*****************************************************************/
static void RecvJoyconData(void)
{

}

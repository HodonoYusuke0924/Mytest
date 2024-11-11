/*****************************************************************
ファイル名	: system_struct.h
内容		: ゲームシステムの構造体
*****************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

/*三次元座標(原点を０とした絶対座標)*/
typedef struct {
float x;
float y;
float z;
} Point;

/*三次元方向ベクトル*/
typedef struct {
float vx;
float vy;
float vz;
} Vector;


/*アイテムの持つ特殊効果*/
typedef enum {
IE_Noting,		//効果なし
/*ここに特殊効果の内容*/
} ItemEffect;

/*アイテムの固定値*/
typedef struct {
    int id			    //アイテムid
    char name[16];		//アイテム名
    char comment[128];	//説明
    float weight;		//重さ
    float initspeed;	//初速度
    int damage;		    //与ダメージ
    ItemEffect effect;	//特殊効果
    Vector cuboid;	    //立方体の長さ
    float radius;		//半径
} Item_Fixed_Info;

/*アイテムの状態*/
typedef enum {
    IS_Disable,		//非表示
    IS_OnField,		//フィールド上にある
    IS_InSlot,		//アイテムスロットにある
    IS_Having,		//プレイヤーが手に持っている
    IS_Throwing,	//投げられている
    IS_Clashing,	//衝突した
    IS_Special		//特殊状態
} ItemStts;

/*アイテムの変数*/
typedef struct {
    ItemStts stts;		//状態
    float speed;		//速度
    Point position;	    //位置
    Vector vector;	    //向き
} Item_Flex_Info;

/*プレイヤの固定値*/
typedef struct {
    int id;			    //プレイヤid
    char name[16];		//プレイヤ名
    int max_cooltime;	//投擲クールタイムの既定値
    int limit_weight;	//制限所持重量
    int max_weight;		//最大所持重量
} Player_Fixed_Info;

/*プレイヤの状態*/
typedef enum {
    PS_Nomal,		//通常状態
    PS_Invincible,	//無敵
    PS_CantMove,	//硬直
    PS_LimitWeight,	//所持制限状態
    PS_Special		//特殊状態
} PlayerStts;

/*プレイヤ勝敗*/
typedef enum {
    UNDESIDE,		//決着前
    WIN,			//勝ち
    LOSE,			//負け
    DRAW			//引き分け
} Result;

/*プレイヤの入力状態*/
typedef struct {
    bool A;
    bool B;
    bool X;
    bool Y;
    bool SL;
    bool SR;
    Vector stick;
} Joycon_Input;

/*プレイヤの変数*/
typedef struct {
    PlayerStts stts;	    //プレイヤの状態
    Result result;		    //勝敗結果
    bool throw;		        //投擲状態のフラグ
    int cooltime;		    //投擲クールタイム
    float speed;		    //速度
    Point point;		    //位置
    Vector vector;  	    //向き
    int total_weight;       //所持品の総重量 
    Joycon_Input button;    //入力
} Player_Flex_Info;

/*フィールド名*/
typedef enum {
    FN_Garbage,		    //ゴミ捨て場
    /*ここにフィールドを追加*/
} FieldName;

/*フィールド情報*/
typedef struct {
    FieldName field;	//使用フィールド
    float resist;		//抵抗値
    Vector force;   	//重力などの力
    int item_table[32];	//出現するアイテム群
} Field_Info;

/*シーン*/
typedef enum {
    TITLE,			//タイトル
    SELECT,			//選択画面（ステージなど）
    PRE_PHASE1,		//フェーズ１準備
    PHASE1,			//フェーズ１
    END_PHASE1,		//フェーズ１終了
    PRE_PHASE2,		//フェーズ２準備
    PHASE2,			//フェーズ２
    END_PHASE2,		//フェーズ２終了
    RESULT			//リザルト
} SCENE;
/*****************************************************************
ファイル名	: system_struct.h
内容		: ゲームシステムの構造体
*****************************************************************/
#ifndef SYSTEM_STRUCT_H
#define SYSTEM_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/*使用スレッド*/
typedef enum {
    UT_TIMER,
    UT_UI,
    UT_NET
} USE_THREAD;

/*スレッドの構造体*/
typedef struct {
    USE_THREAD id;      //使用するスレッド
    pthread_t thread;
} Thread_Info;

#define THREAD_ID_NUM 3 //スレッドの数

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

/*回転(各軸の正の位置から見て反時計回り正：-π <= rad <= π)*/
typedef struct { 
    float rx;   
    float ry;   
    float rz;   
} Rotate;

/*オブジェクトの位置，大きさ*/
typedef struct {
    Point  center;  //位置
    Vector size;    //サイズ
    Vector axis;    //向き
} Object_Info;

typedef enum {
    IN_Void,
    IN_Cube,
    /*ここにアイテムの名前*/
    ITEM_ID_MAX
} ItemName;

/*アイテムの持つ特殊効果*/
typedef enum {
    IE_Noting,		//効果なし
    /*ここに特殊効果の内容*/
    IE_NUM
} ItemEffect;

/*アイテムの固定値*/
typedef struct {
    int id;			    //アイテムid
    char name[16];		//アイテム名
    char comment[128];	//説明
    int weight; 		//重さ
    float initspeed;	//初速度
    int damage;		    //与ダメージ
    ItemEffect effect;	//特殊効果
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
    IS_Special,		//特殊状態
    IS_NUM
} ItemStts;

/*アイテムの変数*/
typedef struct {
    ItemStts stts;		//状態
    float speed;		//速度
    float active_time;  //投げられてからの時間
    bool active;
    Item_Fixed_Info fixed;    //固定値情報
    Object_Info obj;    //位置，向き，ヒットボックスの情報
} Item_Info;

/*プレイヤの固定値*/
typedef struct {
    int id;			    //プレイヤid
    char name[16];		//プレイヤ名
    int max_cooltime;	//投擲クールタイムの既定値
    int limit_weight;	//制限所持重量
    int max_weight;		//最大所持重量
    int max_hp;         //最大HP
} Player_Fixed_Info;

#define PLAYER_ID_MAX 2 //プレイヤの最大数(仮)

/*プレイヤの状態*/
typedef enum {
    PS_DISCONNECT,  //非接続状態
    PS_Nomal,		//通常状態
    PS_Invincible,	//無敵
    PS_CantMove,	//硬直
    PS_LimitWeight,	//所持制限状態
    PS_Special,		//特殊状態
    PS_NUM
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
    bool Home;
    Vector stick;
} Joycon_Input;

#define SLOT_MAX 17
/*プレイヤの変数*/
typedef struct {
    PlayerStts stts;	    //プレイヤの状態
    bool jump;              //ジャンプのフラグ
    float jump_time;        //ジャンプの時間
    bool throw;		        //投擲状態のフラグ
    Result result;		    //勝敗結果
    int cooltime;		    //投擲クールタイム
    int hp;                 //体力
    float speed;		    //速度
    Item_Info having_item[SLOT_MAX];//所持しているアイテム
    int total_weight;               //所持品の総重量 
    Joycon_Input input;             //入力
    Player_Fixed_Info fixed;        //固定値情報
    Object_Info obj;    //位置，向き，ヒットボックスの情報
} Player_Info;

/*フィールド名*/
typedef enum {
    FN_Garbage,		    //ゴミ捨て場
    /*ここにフィールドを追加*/
    FN_NUM
} FieldName;

/*フィールド情報*/
typedef struct {
    FieldName id;	    //フィールドid
    float resist;		//抵抗値
    Vector force;   	//重力などの力
    int map[128][128];  //フィールドの地形情報
    int item_table[32];	//出現するアイテム群
} Field_Info;

#define MAP_GLID 1     //グリッドサイズ
#define MAP_SIZE 129  // 高さマップのサイズ

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
    RESULT,			//リザルト
    SCENE_NUM
} SCENE;

/*ゲームルール*/
typedef struct {
    int limittime_phase1;
    int limittime_phase2;
}Game_Rule;

#define FPS 60
#define NANO_SEC 1000000
#define DELTA_TIME ((float) 1 / FPS)

/*ゲーム情報*/
typedef struct {
    SCENE scene;        //シーン
    int client_num;     //クライアント数
    bool loop;          //ゲームループ
    Thread_Info thread[THREAD_ID_NUM];
    Item_Info item_info[ITEM_ID_MAX];
    Player_Info player_info[PLAYER_ID_MAX];
    Field_Info field_info[FN_NUM];
    Game_Rule rule;
} Game_Info;

/*コールバック関数*/
typedef void (*PositionCallback)(Point position);


#endif // SYSTEM_STRUCT_H

/*****************************************************************
ファイル名	: client_win.c
機能		: クライアントのユーザーインターフェース処理
*****************************************************************/

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL2_gfxPrimitives.h>
#include"common.h"
#include"client_func.h"

static SDL_Window *gMainWindow;
static SDL_Renderer *gMainRenderer;
static SDL_Rect gButtonRect[2];

static int CheckButtonNO(int x,int y,int num);

/*****************************************************************
関数名	: InitWindows
機能	: メインウインドウの表示，設定を行う
引数	: int	clientID		: クライアント番号
		  int	num				: 全クライアント数
出力	: 正常に設定できたとき0，失敗したとき-1
*****************************************************************/
int InitWindows(int clientID,int num,char name[][MAX_NAME_SIZE])
{
	int i;
	SDL_Texture *texture;
	SDL_Surface *image;
	SDL_Rect src_rect;
	SDL_Rect dest_rect;
	char clientButton[4][6]={"0.jpg","1.jpg","2.jpg","3.jpg"};
	char endButton[]="END.jpg";
	char allButton[]="Start.jpg";
	char *s,title[10];

    /* 引き数チェック */
    assert(0<num && num<=MAX_CLIENTS);
	
	/* SDLの初期化 */
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("failed to initialize SDL.\n");
		return -1;
	}
	
	/* メインのウインドウを作成する */
	if((gMainWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1400, 800, 0)) == NULL) {
		printf("failed to initialize videomode.\n");
		return -1;
	}

	gMainRenderer = SDL_CreateRenderer(gMainWindow, -1, SDL_RENDERER_SOFTWARE);//SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC);//0);

	/* ウインドウのタイトルをセット */
	sprintf(title,"%d",clientID);
	SDL_SetWindowTitle(gMainWindow, title);
	
	/* 背景を白にする */
	SDL_SetRenderDrawColor(gMainRenderer, 255, 255, 255, 255);
  	SDL_RenderClear(gMainRenderer);

	/* ボタンの作成 */
	for(i=0;i<2;i++){
		gButtonRect[i].x = 400+400*i;
		gButtonRect[i].y=500;
		gButtonRect[i].w=150;
		gButtonRect[i].h=60;
      
		if(i==0){
			s=allButton;
		}
		else if(i==1){
			s=endButton;
		}
		/*
		else{
			s=clientButton[i];
		}*/
		image = IMG_Load(s);
		texture = SDL_CreateTextureFromSurface(gMainRenderer, image);
		src_rect = (SDL_Rect){0, 0, image->w, image->h};
		SDL_RenderCopy(gMainRenderer, texture, &src_rect, (&gButtonRect[i]));
		SDL_FreeSurface(image);
	}
	SDL_RenderPresent(gMainRenderer);
	
	return 0;
}

/*****************************************************************
関数名	: DestroyWindow
機能	: SDLを終了する
引数	: なし
出力	: なし
*****************************************************************/
void DestroyWindow(void)
{
	SDL_Quit();
}

/*****************************************************************
関数名	: WindowEvent
機能	: メインウインドウに対するイベント処理を行う
引数	: int		num		: 全クライアント数
出力	: なし
*****************************************************************/
void WindowEvent(int num)
{
	SDL_Event event;
	SDL_MouseButtonEvent *mouse;
	int buttonNO;

    /* 引き数チェック */
    assert(0<num && num<=MAX_CLIENTS);

	if(SDL_PollEvent(&event)){

		switch(event.type){
			case SDL_QUIT:
				SendEndCommand();
				break;
			case SDL_MOUSEBUTTONUP:
				mouse = (SDL_MouseButtonEvent*)&event;
				if(mouse->button == SDL_BUTTON_LEFT){
					buttonNO = CheckButtonNO(mouse->x,mouse->y,num);
#ifndef NDEBUG
					printf("#####\n");
					printf("WindowEvent()\n");
					printf("Button %d is pressed\n",buttonNO);
#endif
					if(buttonNO == 0){				
						SendMainCommand(buttonNO);
						/*startボタンを押した*/
					}
					else if(buttonNO == 1){
						/*Endボタンを押した*/
						SendEndCommand();
					}
					
				}
				break;
		}
	}
}



/*****************************************************************
関数名	: Gomain
機能	: メインウインドウに円を表示する
引数	: int		x		: 円の x 座標
		  int		y		: 円の y 座標
		  int		r		: 円の半径
出力	: なし
*****************************************************************/
void Gomain(int x,int y,int r)
{
#ifndef NDEBUG
	printf("#####\n");
    printf("Gomain()\n");
    printf("x=%d,y=%d,tyokkei=%d\n",x,y,5);
#endif

    circleColor(gMainRenderer,x,y,r,0xff0000ff);
	SDL_RenderPresent(gMainRenderer);
}



/*****
static
*****/
/*****************************************************************
関数名	: CheckButtonNO
機能	: クリックされたボタンの番号を返す
引数	: int	   x		: マウスの押された x 座標
		  int	   y		: マウスの押された y 座標
		  char	   num		: 全クライアント数
出力	: 押されたボタンの番号を返す
		  ボタンが押されていない時は-1を返す
*****************************************************************/
static int CheckButtonNO(int x,int y,int num)
{
	int i;

 	for(i=0;i<2;i++){
		if(gButtonRect[i].x < x &&
			gButtonRect[i].y < y &&
      		gButtonRect[i].x + gButtonRect[i].w > x &&
			gButtonRect[i].y + gButtonRect[i].h > y){
			return i;
		}
	}
 	return -1;
}

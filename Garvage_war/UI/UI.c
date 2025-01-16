#ifndef __APPLE
#include <GL/glut.h> // Windows, Linuxの場合のヘッダ
#else
#endif

#include <math.h>
#include <pthread.h>

#include "ui.h"
#include "type.h"
#include "logic.h"
#include <stdio.h>
#include <stdbool.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>  // rand() のために必要
#include <time.h>

#define Pai 3.1415926
static float r = 0.0f; // ポリゴンの回転角
int phase_timer = 10; // フェーズ1のタイマー（秒単位）
time_t last_update_time;

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

pthread_mutex_t data_mutex;
float enemy_hp = 1.0f;

float player_hp = 1.0f;

int frame_count = 0;  // フレームカウンター

// スクロールの状態を管理する変数
float scrollOffsetY = 0.0f; // 縦方向のスクロール量

GLuint textureID;

GLuint loadTexture(const char *filePath){
    SDL_Surface *surface = IMG_Load(filePath);
    if (!surface){
        printf("Failed to load image: %s, SDL_image Error: %s\n", filePath, IMG_GetError());
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // SDl_Surfaceからピクセルデータを転送
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
                 (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB,
                GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SDL_FreeSurface(surface); // サーフェスを解放
    return textureID;
}

GLuint textureIDs[9]; // 3x3 正方形の場合
const char* textureFiles[] = {
    "kai.png", "sword.png", "kai.png",
    "sword.png", "kai.png", "sword.png",
    "kai.png", "sword.png", "kai.png"
};

void loadAllTextures() {
    for (int i = 0; i < 9; i++) {
        textureIDs[i] = loadTexture(textureFiles[i]);
        if (textureIDs[i] == 0) {
            printf("Failed to load texture for file: %s\n", textureFiles[i]);
        }
    }
}


void initOpenGL(){
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // SDLで画像をロードしてOpenGLテクスチャを生成
    textureID = loadTexture("sword.png");
    if (!textureID){
        printf("Failed to load texture\n");
    }
}

// 文字を描画する関数
void drawText(float x, float y, const char *text){
    // 指定座標に文字列を開始
    glRasterPos2f(x, y);
    // 文字列の終端（ヌル文字 '\0'）に達するまでループ
    while(*text){
        // GLUT_BITMAP_HELVETICA_18 フォントを使用して、*text が指す文字を描画
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        // ポインタをインクリメントして次の文字に移り
        text++;
    }
}

void renderStartScreen() {
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // テクスチャ描画
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glBegin(GL_QUADS);
         glTexCoord2f(1.0f, 1.0f); glVertex2f(-1.0f, -1.0f); // UV座標を180度回転させて表示
         glTexCoord2f(0.0f, 1.0f); glVertex2f(1.0f, -1.0f);
         glTexCoord2f(0.0f, 0.0f); glVertex2f(1.0f, 1.0f);
         glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    
    // タイトル描画
    glColor3f(1.0, 1.0, 1.0); // 白色
    drawText(-0.1f, 0.7f, "Garbage War");

    // スタートボタン
    glColor3f(1.0, 1.0, 0.2); // 黄色
    glBegin(GL_QUADS);
        glVertex2f(-0.5, -0.7);
        glVertex2f(0.5, -0.7);
        glVertex2f(0.5, -0.3);
        glVertex2f(-0.5, -0.3);
    glEnd();

    // ボタンテキスト
    glColor3f(1.0, 1.0, 1.0); // 白色
    drawText(-0.15f, -0.6f, "Game Start");

    glFlush();
}

void mouseCallback(int button, int state, int x, int y) {
    if (game_phase == 0 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 座標を正規化 (-1.0 ~ 1.0) に変換
        float nx = (x / (float)WINDOW_WIDTH) * 2.0 - 1.0;
        float ny = -((y / (float)WINDOW_HEIGHT) * 2.0 - 1.0);

        // ボタンの範囲をチェック
        if (nx >= -0.5 && nx <= 0.5 && ny >= -0.7 && ny <= 0.3) {
            game_phase = 1; // ゲームフェーズ1へ移行
        }
    }
}

void initUI() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void Rect2DFill(Rect rect) {
    float x1 = rect.x;
    float x2 = rect.x + rect.w;
    float y1 = rect.y;
    float y2 = rect.y + rect.h;

    glBegin(GL_POLYGON);
        glVertex2f(x1, y1);
        glVertex2f(x1, y2);
        glVertex2f(x2, y2);
        glVertex2f(x2, y1);
    glEnd();
}

/*画面上座標を割合に直す*/ 
Point PointToRate(Point point, Point max_size){
    point.x = (point.x/max_size.x)*2 - 1;//範囲を0~2から-1~1にする
    point.y = -1 * ((point.y/max_size.y)*2 - 1);
    return point;
}

//円を描画するための関数
void Circle2DFill(float radius, int x, int y){
    for (float th1 = 0.0; th1 <= 360.0; th1 = th1 + 1.0)
    {
        float th2 = th1 + 10.0;
        float th1_rad = th1 / 180.0*Pai;
        float th2_rad = th2 / 180.0*Pai;

        float x1 = radius * cos(th1_rad);
        float y1 = radius * sin(th1_rad);
        float x2 = radius * cos(th2_rad);
        float y2 = radius * sin(th2_rad);

        Point p1 = {x1+x,y1+y};
        Point p2 = {x2+x,y2+y};
        Point window = {WINDOW_WIDTH, WINDOW_HEIGHT};

        p1 = PointToRate(p1, window);
        p2 = PointToRate(p2, window);

        glBegin(GL_LINES);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glEnd();
    }
}

void drawResultScreen() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // 背景を黒色に
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0); // 白色の文字
    if (player_hp <= 0.0f) {
        drawText(-0.4f, 0.0f, "YOU LOSE");
    } else if (enemy_hp <= 0.0f) {
        drawText(-0.4f, 0.0f, "YOU WIN");
    }

    // 他のUI要素が表示されないようにここで終了
    glutSwapBuffers();
}

/*点描画関数*/
void DrawPoint(float map_radius, int map_center_x, int map_center_y, int point_x, int point_y)
{
    /*ウィンドウ座標をOpenGLの座標へ変換*/
    Point map_center = {map_center_x, map_center_y};
    Point point = {point_x, point_y};
    Point window_size = {WINDOW_WIDTH, WINDOW_HEIGHT};

    Point normalized_center = PointToRate(map_center, window_size);
    Point normalized_point = PointToRate(point, window_size);

    /*点が円の範囲内にあるか確認*/
    float dx = normalized_point.x - normalized_center.x;
    float dy = normalized_point.y - normalized_center.y;
    float distance_squared = dx * dx + dy * dy;

    /*半径を正規化*/
    float normalized_radius = map_radius / WINDOW_WIDTH * 2.0f;
    // 円の中であれば
    if(distance_squared <= normalized_radius * normalized_radius){
        glColor3f(1.0, 0.0, 0.0);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glVertex2f(normalized_point.x, normalized_point.y);
        glEnd();

    }
}

void drawItemBox(float startX, float startY, int rows, int cols, GLuint* textureIDs){
    float boxWidth = 0.6f;   // 青いエリアの幅
    float boxHeight = 0.6f;  // 青いエリアの高さ

    float boxSize = 0.2f; // 各正方形のサイズ
    float spacing = 0.05f; // 正方形間の間隔

    glEnable(GL_SCISSOR_TEST);
    glScissor((startX + 1.0f) / 2.0f * 800,               
          (startY - boxHeight + 1.0f) / 2.0f * 800,   
          boxWidth * 800,                              
          boxHeight * 800);
    glDisable(GL_SCISSOR_TEST);

     glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float x = startX + j * (boxSize + spacing); // x座標
            float y = startY - i * (boxSize + spacing) + scrollOffsetY; // y座標

            // テクスチャをバインド
            int textureIndex = i * cols + j; // 行列のインデックス
            glBindTexture(GL_TEXTURE_2D, textureIDs[textureIndex]);
             
            // テクスチャ付きの正方形を描画
            // 青いエリア内の正方形だけを描画
            if (y - boxSize < startY && y > startY - boxHeight) {

            glColor3f(1.0f, 1.0f, 1.0f); // テクスチャ本来の色を使う
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);                      // 左上
                glTexCoord2f(1.0f, 0.0f); glVertex2f(x + boxSize, y);           // 右上
                glTexCoord2f(1.0f, 1.0f); glVertex2f(x + boxSize, y - boxSize); // 右下
                glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y - boxSize);           // 左下
            glEnd();
    }
    }
    }
    glDisable(GL_TEXTURE_2D);
  
}

// キーボード入力でスクロール量を変更
void handleKeyboard(unsigned char key, int x, int y) {
    float scrollSpeed = 0.05f;
    if (key == 'w') scrollOffsetY += scrollSpeed; // 上方向スクロール
    if (key == 's') scrollOffsetY -= scrollSpeed; // 下方向スクロール

    // スクロール範囲を制限
    if (scrollOffsetY > 0.0f) scrollOffsetY = 0.0f; // 上にはみ出ない
    if (scrollOffsetY < -0.6f) scrollOffsetY = -0.6f; // 下にはみ出ない（例）

    glutPostRedisplay(); // 再描画
}

//メイン画面
void drawMain(float r, float g, float b){
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(-1.0, -0.4);//左下
        glVertex2f(-1.0, 0.5);//左上
        glVertex2f(-0.1, 0.5);//右上
        glVertex2f(-0.1, -0.4);//右下
    glEnd();
}

void drawMain2(float r, float g, float b){
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(-1.0, -0.9);//左下
        glVertex2f(-1.0, 0.5);//左上
        glVertex2f(0.5, 0.5);//右上
        glVertex2f(0.5, -0.9);//右下
    glEnd();
}

void drawEnemy(float x, float y, float size){
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(x - size, y - size);
        glVertex2f(x - size, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size, y - size);
    glEnd();
}

void updateTimer(){
    //現在時刻取得
    time_t current_time = time(NULL);

    // 1秒経過ごとに更新
    if (current_time - last_update_time >= 1){
        last_update_time = current_time;

        if (phase_timer > 0){
            phase_timer--;
        }else if (game_phase == 1){
            game_phase = 2;
            phase_timer = 0;
        }
    }
}

void updateUI() {
    pthread_mutex_lock(&phase_mutex);
    
    // 背景色（全体のクリアカラー）
    glClearColor(0.0, 0.0, 0.0, 1.0); // 黒色
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // フェーズに応じたメイン画面の色設定

    if (game_phase == 0){
        renderStartScreen();
       
    } else if (game_phase == 1) {  
        drawMain(0.0f, 0.5f, 1.0f); // フェーズ1の色
        glColor3f(1.0, 1.0, 1.0);

        char timer_text[20];
        snprintf(timer_text, sizeof(timer_text), "残り%ds", phase_timer);
        drawText(-0.5f, 0.8f, "PHASE 1");
        drawText(-0.4f, 0.7f,"残り");
        drawText(-0.3f, 0.8f,timer_text);

        //アイテム
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.1, -0.4);
        glVertex2f(0.1, 0.5);
        glVertex2f(1.0, 0.5);
        glVertex2f(1.0, -0.4);
    glEnd();

    //GLuint textureID = loadTexture("kai.png");
    loadAllTextures(); 
    drawItemBox(-0.9, 0.4, 3, 3, textureIDs);
    //drawItemBox(0.2, 0.4, 3, 3, textureID);

    } else if (game_phase == 2) {
        drawMain2(1.0f, 0.5f, 0.0f); // フェーズ2の色
        glColor3f(1.0, 1.0, 1.0);
        drawText(-0.5f, 0.8f, "PHASE2");


    // 他のUI要素をフェーズ2に変更
    glColor3f(0.8, 0.0, 0.0);
    Rect phase2_special_bar = {-1.0, 0.4, 0.1, 0.5};
    Rect2DFill(phase2_special_bar);
    

    drawEnemy(0.2f, -0.2f, 0.1f); // 位置(x, y) = (0.2, -0.2), サイズ = 0.1
     
    // プレイヤーHPバー
    glColor3f(0.0, 1.0, 0.0);
    Rect player_hp_bar = {-1.0, 0.6, 0.1, player_hp * 0.7};
    Rect2DFill(player_hp_bar);

    // 敵HPバー
    glColor3f(1.0, 0.0, 0.0);
    Rect enemy_hp_bar = {-0.2, 0.6, 0.1, enemy_hp * 0.7};
    Rect2DFill(enemy_hp_bar);

    // ラベル表示
    glColor3f(0.0, 1.0, 0.0);
    drawText(-0.2, 0.75, "ENEMY");
    drawText(-1.0, 0.75, "YOU");

    //アイテム
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.55, -0.9);
        glVertex2f(0.55, 0.45);
        glVertex2f(1.0, 0.45);
        glVertex2f(1.0, -0.9);
    glEnd();

    drawItemBox(0.55, 0.35, 5, 2, textureIDs);

    // マップ
    glColor3f(0.0, 1.0, 1.0);
    float radius = 120;
    int map_center_x = WINDOW_WIDTH - radius; // 右上の中心x
    int map_center_y = radius;                // 右上の中心y
    Circle2DFill(radius, map_center_x, map_center_y);

    // 点の描画
    int point_x1 = WINDOW_WIDTH - 50;
    int point_y1 = 100;
    int point_x2 = WINDOW_WIDTH - 30;
    int point_y2 = 50;
    DrawPoint(radius, map_center_x, map_center_y, point_x1, point_y1);
    DrawPoint(radius, map_center_x, map_center_y, point_x2, point_y2);
 
    /*敵(仮の四角形)
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3d(-0.2, +0.2, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3d(+0.2, +0.2, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3d(-0.2, -0.2, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3d(+0.2, -0.2, 0.0);
    glEnd();*/
    
     // 敵のHPをランダムに減らす (100フレームに1回実行)
    if (frame_count % 100 == 0) {
        reduceEnemyHP();
    }

    frame_count++;
     } else if (game_phase == 3) {
        drawResultScreen(); // リザルト画面描画
        pthread_mutex_unlock(&phase_mutex);
       
    return; 
}

     pthread_mutex_unlock(&phase_mutex);

    // 他のUI要素の描画（メインやマップ）
    //メイン画面
   
       
    //その他の3D描画など
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(r, 0.0, 1.0, 0.0);
    /*この部分にポリゴン描画してた*/
    glPopMatrix();

    glutSwapBuffers(); // 描画バッファのスワップ
}

void displayUI() {
    updateUI();
}

/*画面がリサイズされたとき*/
void resize(int w, int h){
    /*ピューポートの設定*/
    glViewport(0, 0, w, h);
    /*射影行列の設定*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, 1.0, 1.0, 80.0); // 画角、アスペクト比、前方面、後方面
}
void timer(int value){
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

/*暇なときのアイドルイベント*/

void idle()
{
    //ポリゴンの回転
    r = r + 3.0f;
    while(360.0f < r) r -= 360.f;

    updateTimer();

    glutPostRedisplay(); // 再描画リクエスト
}



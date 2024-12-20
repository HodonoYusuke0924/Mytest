#ifndef __APPLE
#include <GL/glut.h> // Windows, Linuxの場合のヘッダ
#else
#endif

#include <math.h>
#include <pthread.h>

#include "ui.h"
#include "type.h"
#include "logic.h"


#define Pai 3.1415926
static float r = 0.0f; // ポリゴンの回転角

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

pthread_mutex_t data_mutex;
float enemy_hp = 1.0f;

float player_hp = 1.0f;

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

//メイン画面
void drawMain(float r, float g, float b){
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

void updateUI() {
    pthread_mutex_lock(&phase_mutex);
    
    // 背景色（全体のクリアカラー）
    glClearColor(0.0, 0.0, 0.0, 1.0); // 黒色
    glClear(GL_COLOR_BUFFER_BIT);

    // フェーズに応じたメイン画面の色設定
    if (game_phase == 1) {
        drawMain(0.0f, 0.5f, 1.0f); // フェーズ1の色
    } else if (game_phase == 2) {
        drawMain(1.0f, 0.5f, 0.0f); // フェーズ2の色
    

    // フェーズ2追加
    drawEnemy(0.2f, -0.2f, 0.1f); // 位置(x, y) = (0.2, -0.2), サイズ = 0.1

    // 他のUI要素をフェーズ2に変更
    glColor3f(0.8, 0.0, 0.0);
    Rect phase2_special_bar = {-1.0, 0.4, 0.1, 0.5};
    Rect2DFill(phase2_special_bar);
    }

     if (game_phase == 2){
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
     }

     glutSwapBuffers();

     pthread_mutex_unlock(&phase_mutex);

    // 他のUI要素の描画（メインやマップ）
    //メイン画面
   
    //アイテム
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.55, -0.9);
        glVertex2f(0.55, 0.45);
        glVertex2f(1.0, 0.45);
        glVertex2f(1.0, -0.9);
    glEnd();

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

    // その他の3D描画など
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(r, 0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3d(-0.2, +0.2, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3d(+0.2, +0.2, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3d(-0.2, -0.2, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3d(+0.2, -0.2, 0.0);
    glEnd();
    glPopMatrix();

    glutSwapBuffers(); // 描画バッファのスワップ
}


void displayUI() {
    updateUI();
    glutSwapBuffers();
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

    glutPostRedisplay(); // 再描画リクエスト
}



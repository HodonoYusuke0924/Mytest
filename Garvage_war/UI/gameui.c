#ifndef __APPLE
#include <GL/glut.h> // Windows, Linuxの場合のヘッダ
#else
#endif

#include <math.h>

#define Pai 3.1415926

int WINDOW_WIDTH = 1000; //ウィンドウの横幅
int WINDOW_HEIGHT = 1000; //ウィンドウの高さ
int x = 100;
int y = -100;

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

        glBegin(GL_LINES);
        glVertex2f(x1+x, y1+y);
        glVertex2f(x2+x, y2+y);
        glEnd();
    }
}

//OpenGLの描画に使う関数
void display(){
    //画面を決まった色で塗りつぶす
    glClear(GL_COLOR_BUFFER_BIT);

    //メイン画面
    glColor3f(0.0, 0.5, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(-1.0, -1.0);//左下
        glVertex2f(-1.0, 0.5);//左上
        glVertex2f(0.5, 0.5);//右上
        glVertex2f(0.5, -1.0);//右下
    glEnd();

    //HP画面(自分)
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
        glVertex2f(-1.0, 0.6);
        glVertex2f(-1.0, 0.7);
        glVertex2f(-0.3, 0.7);
        glVertex2f(-0.3, 0.6);
    glEnd();

    //HP画面(敵)
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(-0.2, 0.6);
        glVertex2f(-0.2, 0.7);
        glVertex2f(0.5, 0.7);
        glVertex2f(0.5, 0.6);
    glEnd();

    //アイテム
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
        glVertex2f(0.5, -1.0);
        glVertex2f(0.5, 0.5);
        glVertex2f(1.0, 0.5);
        glVertex2f(1.0, -1.0);
    glEnd();

    //マップ
    glColor3f(0.0, 1.0, 1.0);
    Circle2DFill(0.2, 1.0, 1.0); // 半径，円の中心のx座標，円の中心のy座標

    // 表示内容を更新
    glFlush();
}

void init(){
//クリアカラーを黒に設定
glClearColor(0.0, 0.0, 0.0, 0.0);
//塗りつぶす色を白に設定
glColor3f(1.0, 1.0, 1.0);
//2D投影行列
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char **argv){
    //GLUTの初期化(どのOpenGL関数よりも前に呼び出す必要がある)
    glutInit(&argc, argv);
    //カラーモデルとシングルバッファモードの設定
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //ウィンドウのサイズを設定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //表示されるウィンドウの位置を指定
    glutInitWindowPosition(x, y);
    //ウィンドウの作成(引数はウィンドウのタイトル)
    glutCreateWindow("First GLUT program");
    //描画に使う関数(コールバック関数)の登録
    glutDisplayFunc(display);
    //描画ループの開始
    glutMainLoop();//(main()関数で最後の文でなければならない)
}


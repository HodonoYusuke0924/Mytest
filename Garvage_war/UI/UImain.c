/*
コンパイル
gcc cuboid.c -o cuboid -lGL -lGLU -lglfw -lm
*/

#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "system_struct.h"
#include "system_func.h"

//RGBAカラータイプ(glColor4fに対応)
typedef struct {
    float r;
float g;
    float b;
    float a;
} Color;
//基本色
const Color BLACK   = {0.0f, 0.0f, 0.0f, 1.0f};    //黒
const Color WHITE   = {1.0f, 1.0f, 1.0f, 1.0f};    //白
const Color RED     = {1.0f, 0.0f, 0.0f, 1.0f};    //赤
const Color GREEN   = {0.0f, 1.0f, 0.0f, 1.0f};    //緑
const Color BLUE    = {0.0f, 0.0f, 1.0f, 1.0f};    //青
const Color YELLOW  = {1.0f, 1.0f, 0.0f, 1.0f};    //黄色
const Color CYAN    = {0.0f, 1.0f, 1.0f, 1.0f};    //シアン
const Color MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};    //マゼンタ

//構造体から色を設定するヘルパー関数
void setColor(Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
}

#define WINDOW_W 800
#define WINDOW_H 600

/*
座標の規定

    y
    |   z
    |  /
    | /
    |/
----|------------- x
  0/|    

*/

//画面設定
#define WINDOW_W 800
#define WINDOW_H 600

void drawPorigons(Point porigons[]) {
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < 129 - 2; i++){
        if(porigons[i + 2].x == 0 && porigons[i + 2].y == 0 && porigons[i + 2].z == 0){
            glVertex3f(porigons[i    ].x, porigons[i    ].y, porigons[i    ].z);
            glVertex3f(porigons[i + 1].x, porigons[i + 1].y, porigons[i + 1].z);
            glVertex3f(porigons[0    ].x, porigons[0    ].y, porigons[0    ].z);
            break;
        }
        glVertex3f(porigons[i    ].x, porigons[i    ].y, porigons[i    ].z);
        glVertex3f(porigons[i + 1].x, porigons[i + 1].y, porigons[i + 1].z);
        glVertex3f(porigons[i + 2].x, porigons[i + 2].y, porigons[i + 2].z);
    }
    glEnd();
}

// 直方体を描画する関数(p:中心点, r:回転, v:向き, rect:各辺の長さ, color:色)
void drawCuboid(Point p, /*Rotate r, Vector v,*/ Vector rect, Color color) {
    float a = rect.vx;
    float b = rect.vy;
    float c = rect.vz;

    // 直方体の8つの頂点を計算
    Point vertices[8] = {
        {p.x - a / 2, p.y - b / 2, p.z - c / 2},  // 0
        {p.x + a / 2, p.y - b / 2, p.z - c / 2},  // 1
        {p.x + a / 2, p.y + b / 2, p.z - c / 2},  // 2
        {p.x - a / 2, p.y + b / 2, p.z - c / 2},  // 3
        {p.x - a / 2, p.y - b / 2, p.z + c / 2},  // 4
        {p.x + a / 2, p.y - b / 2, p.z + c / 2},  // 5
        {p.x + a / 2, p.y + b / 2, p.z + c / 2},  // 6
        {p.x - a / 2, p.y + b / 2, p.z + c / 2}   // 7
    };
/*
    for(int i = 0; i < 8; i++){
        rotate_point(p, vertices[i], r, v);
    }
*/
    // 直方体の各面を塗りつぶしで描画
    glBegin(GL_QUADS);

    setColor(color);
    // 底面
    glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);

    // 上面
    glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
    glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
    glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
    glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);

    // 側面（後面）
    glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
    glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);

    // 側面（左面）
    glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
    glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);

    // 側面（右面）
    glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
    glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);

    setColor(RED);
    // 側面（前面）
    glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
    glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);

    glEnd();
}

//フィールドのポリゴン作成
void disp_field() {
    glBegin(GL_TRIANGLES);
    for (int z = 0; z < MAP_SIZE - 1; z++) {
        for (int x = 0; x < MAP_SIZE - 1; x++) {
            // 頂点の定義
            /*
            float x0 =  x      * MAP_GLID, y0 = terrain[z    ][x    ], z0 =  z      * MAP_GLID;
            float x1 = (x + 1) * MAP_GLID, y1 = terrain[z    ][x + 1], z1 =  z      * MAP_GLID;
            float x2 =  x      * MAP_GLID, y2 = terrain[z + 1][x    ], z2 = (z + 1) * MAP_GLID;
            float x3 = (x + 1) * MAP_GLID, y3 = terrain[z + 1][x + 1], z3 = (z + 1) * MAP_GLID;
            */
            float x0=0,y0=0,z0=0,x1=0,y1=0,z1=0,x2=0,y2=0,z2=0,x3=0,y3=0,z3=0;
            glColor3f(0.3f, 0.7f, 0.2f);  // 緑色
            glVertex3f(x0, y0, z0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);

            glColor3f(0.2f, 0.5f, 0.3f);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
        }
    }
    glEnd();
}

GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, "3D Cube", NULL, NULL);

//初期化処理
void initUI() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);
    //背景の色
    glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.33, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void endUI() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void titleUI(Player_Info player_info){

}

void loadingUI(){

}

Object_Info camera;
Object_Info target;

void battleUI3D(Game_Info game){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 現在のカメラの視点を設定
    gluLookAt(camera.center.x, camera.center.y, camera.center.z, 
              target.center.x  , target.center.y  , target.center.z  , 
              0.0       , 1.0       , 0.0        );

    //フィールド描画
    disp_field();
}
void battleUI2D(){

}

void endUI(){

}

void resultUI(int result){

}

void UserInterface(Game_Info game_info, int pID){
    switch (game_info.scene)
    {
    case TITLE:
        titleUI(game_info.player_info[pID]);
        break;
    case PRE_PHASE1:
    case PRE_PHASE2:
        loadingUI();
        break;
    case PHASE1:
    case PHASE2:
        battleUI3D(game_info);
        battleUI2D();
        break;
    case END_PHASE1:
    case END_PHASE2:
        endUI();
        break;
    case RESULT:
        resutUI(game_info.player_info[pID].result);
        break;
    default:
        break;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

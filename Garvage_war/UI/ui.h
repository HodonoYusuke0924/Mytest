#ifndef UI_H
#define UI_H

#include <GL/glut.h>
#include <pthread.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <FreeImage.h>

/*外部で扱う共有変数の宣言*/
extern pthread_mutex_t phase_mutex;
extern int game_phase;
extern float enemy_hp;
extern float player_hp;

/*UI関連の初期化や更新関数*/
void* phaseTransitionThread(void* arg);
void initUI();
void displayUI();
void mouseCallback();
void handleKeyboard(unsigned char key, int x, int y);
void initOPenGL();
void resize();
void timer();
void idle();

#endif
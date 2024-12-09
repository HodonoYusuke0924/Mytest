#ifndef LOGIC_H
#define LOGIC_H

#include <pthread.h>

extern pthread_mutex_t phase_mutex;
extern int game_phase;
extern float enemy_x, enemy_y;


void* phaseTransitionThread(void* arg);

/*ゲームのロジックのスレッド*/
void* gameLogicThread(void* arg);

#endif
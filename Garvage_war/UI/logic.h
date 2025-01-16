#ifndef LOGIC_H
#define LOGIC_H

#include <pthread.h>
#include <stdbool.h>

extern pthread_mutex_t phase_mutex;
extern float enemy_attack_damage;
extern int game_phase;

void* phaseTransitionThread(void* arg);

/*ゲームのロジックのスレッド*/
void* gameLogicThread(void* arg);


void reduceEnemyHP();
void playerTakeDamage(float damage);
bool checkCollision(float enemy_x, float enemy_y, float enemy_size, float player_x, float player_y, float player_size);
void checkGameOver();
void updateGame();
#endif
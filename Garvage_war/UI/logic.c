#include "logic.h"
#include <pthread.h>
#include <unistd.h> // usleep

//extern pthread_mutex_t data_mutex;
extern float enemy_hp;
extern float player_hp;

pthread_mutex_t phase_mutex = PTHREAD_MUTEX_INITIALIZER;
int game_phase = 1; // 初期

void* phaseTransitionThread(void* arg){
    sleep(10); //10秒経過

    pthread_mutex_lock(&phase_mutex);
    game_phase = 2;
    pthread_mutex_unlock(&phase_mutex);

    return NULL; 
}

void* gameLogicThread(void* arg) {
    // ロジック処理の実装
    return NULL;
}

/*void* gameLogicThread(void* arg){
    while(1){
        pthread_mutex_lock(&data_mutex);

        //hpを減らしていく//
        if (enemy_hp > 0.0f) enemy_hp -= 0.01f;
        if (player_hp > 0.0f) player_hp -= 0.005f;

        pthread_mutex_unlock(&data_mutex);

        usleep(100000); //100ms間隔
    }
    return NULL;
}*/

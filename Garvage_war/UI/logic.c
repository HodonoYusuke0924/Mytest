#include "logic.h"
#include <pthread.h>
#include <unistd.h> // usleep
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 

extern pthread_mutex_t data_mutex;
extern float enemy_hp;
extern float player_hp;
float enemy_attack_damage = 0.1f;

pthread_mutex_t phase_mutex = PTHREAD_MUTEX_INITIALIZER;
int game_phase = 0; // 初期

void* phaseTransitionThread(void* arg){
    while(1){
    pthread_mutex_lock(&phase_mutex);
    if(game_phase == 1){
       pthread_mutex_lock(&phase_mutex);
       int t = 100;
       sleep(t); //10秒経過
       pthread_mutex_lock(&phase_mutex);
       game_phase = 2;
    }
    pthread_mutex_unlock(&phase_mutex);
    break;
    }
    return NULL;
}

void* gameLogicThread(void* arg) {
    // ロジック処理の実装
    return NULL;
}

// 敵のHPを減少させる
void reduceEnemyHP() {
    if (enemy_hp > 0.0f) {
        float damage = (rand() % 50 + 10) / 100.0f;  // 0.1 ～ 0.5 のランダム値
        enemy_hp -= damage;
        if (enemy_hp < 0.0f) enemy_hp = 0.0f;

        printf("Enemy HP reduced by %.2f, current HP: %.2f\n", damage, enemy_hp);
    }
}

// プレイヤーのHPを減少させる
void playerTakeDamage(float damage) {
    pthread_mutex_lock(&data_mutex);
    player_hp -= damage;
    if (player_hp < 0.0f) player_hp = 0.0f;
    pthread_mutex_unlock(&data_mutex);
}

// 衝突をチェックする
bool checkCollision(float enemy_x, float enemy_y, float enemy_size, float player_x, float player_y, float player_size) {
    float dx = enemy_x - player_x;
    float dy = enemy_y - player_y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= (enemy_size + player_size);
}

// ゲームオーバーをチェックする
void checkGameOver() {
    pthread_mutex_lock(&data_mutex);
    if (player_hp <= 0.0f || enemy_hp <= 0.0f) {
        game_phase = 3;  // リザルト画面へ
    }
    pthread_mutex_unlock(&data_mutex);
    printf("Game phase: %d, Player HP: %.2f, Enemy HP: %.2f\n", game_phase, player_hp, enemy_hp);
}

// ゲームの更新処理
void updateGame() {
    float enemy_x = 0.2f;
    float enemy_y = -0.2f;
    float enemy_size = 0.1f;
    float player_x = 0.0f;
    float player_y = 0.0f;
    float player_size = 0.1f;

    if (checkCollision(enemy_x, enemy_y, enemy_size, player_x, player_y, player_size)) {
        playerTakeDamage(enemy_attack_damage);
    }

    checkGameOver();
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

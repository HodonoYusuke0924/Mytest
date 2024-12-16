
/*****************************************************************
ファイル名	: timer.c
内容	    : タイマー処理 
*****************************************************************/
#include "system_struct.h"
#include "system_func.h"

/*指定秒数待機する関数*/
void wait(int seconds)
{
    clock_t start_time = clock();
    clock_t end_time   = seconds * CLOCKS_PER_SEC + start_time;

    while(clock() < end_time);
}

// タイマー処理を行うスレッド関数
void* timer_task(void* arg) {
    int interval = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        printf("Thread with interval %d executed at: %ld\n", interval, time(NULL));
        sleep(interval);  // 指定した間隔で処理を実行
    }
    return NULL;
}

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


#include <stdbool.h>
#include <pthread.h>
#include <GL/glut.h>
#include "ui.h"
#include "logic.h"

bool running = true;

int WINDOW_WIDTH = 1000; //ウィンドウの横幅
int WINDOW_HEIGHT = 1000; //ウィンドウの高さ

int main(int argc, char** argv) {
    pthread_mutex_init(&data_mutex, NULL);

    /*処理スレッド*/
    pthread_t logic_thread;
    pthread_create(&logic_thread, NULL, gameLogicThread, NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, -100);
    glutCreateWindow("Multi-threaded UI");
    initUI();

    // スレッド初期化
    pthread_t phase_thread;
    pthread_create(&phase_thread, NULL, phaseTransitionThread, NULL);


    /*GLUTコールバック*/
    glutDisplayFunc(displayUI);
    glutTimerFunc(0, timer, 0);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);
    
    glutMainLoop();

    /*プログラム終了時の後処理*/
    running = false;
    pthread_join(phase_thread, NULL);
    pthread_mutex_destroy(&data_mutex);

    return 0;
}

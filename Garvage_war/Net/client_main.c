#include "common.h"
#include "client_func.h"
#include "system_func.h"

int main(int argc, char *argv[]) {
    int num;
    char name[MAX_CLIENTS][MAX_NAME_SIZE];
    int endFlag = 1;
    char *serverName;
    int clientID;

    // �T�[�o�[���̎擾
    if (argc == 2) {
        serverName = argv[1];
    } else {
        fprintf(stderr, "Usage: %s <server IP address>\n", argv[0]);
        return -1;
    }

    // �N���C�A���g�̃Z�b�g�A�b�v
    if (SetUpClient(serverName, &clientID, &num, name) == -1) {
        fprintf(stderr, "setup failed : SetUpClient\n");
        return -1;
    }

    // GUI������
    if (InitWindows(clientID, num, name) == -1) {
        fprintf(stderr, "setup failed : InitWindows\n");
        return -1;
    }

    joycon_init();

    while (endFlag) {
        WindowEvent(num);
        endFlag = SendRecvManager();
    }

    DestroyWindow();
    joycon_end();
    CloseSoc();

    return 0;
}

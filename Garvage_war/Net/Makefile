CC      = gcc
CFLAGS  = -Wall -g -DNDEBUG
PICFLAGS = -fPIC
LDFLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_gfx -pthread -ljoyconlib -lhidapi-hidraw

# ���֥������ȥե���������
OBJS_SERVER = server_main.o server_net.o server_command.o server_game.o
OBJS_CLIENT = client_main.o client_net.o client_command.o client_win.o client_game.o
OBJS_JOYCON = system_init.o system_joycon.o system_main.o timer.o

# �������åȤ����
TARGET_SERVER = server
TARGET_CLIENT = client
LIB_JOYCON = libjoycon.so

# �ӥ�ɥ롼��
.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_CLIENT) $(LIB_JOYCON)

$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TARGET_CLIENT): $(OBJS_CLIENT) $(LIB_JOYCON)
	$(CC) -o $@ $^ $(LDFLAGS)

$(LIB_JOYCON): $(OBJS_JOYCON)
	$(CC) -shared -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

system_init.o system_joycon.o system_main.o timer.o: %.o: %.c
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET_SERVER) $(TARGET_CLIENT) $(LIB_JOYCON)

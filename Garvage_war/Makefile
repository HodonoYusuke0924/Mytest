# コンパイラ
CC = gcc
# コンパイルオプション
CFLAGS = -Wall -g
# リンクするライブラリ
LIBS = -pthread -ljoyconlib -lhidapi-hidraw -lm

# ターゲット名（生成される実行ファイル）
TARGET = GarabageWar

# ソースファイルの検出 (現在のディレクトリとサブディレクトリ内の全ての .c ファイル)
SRCS = $(shell find . -maxdepth 1 -name "*.c" && find ./System -name "*.c")
# オブジェクトファイルのリスト（対応する .o ファイルに変換）
OBJS = $(SRCS:.c=.o)

# ターゲットの作成ルール
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# 各 .c ファイルから対応する .o ファイルを作成するルール
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# クリーンアップ（オブジェクトファイルと実行ファイルの削除）
clean:
	rm -f $(OBJS) $(TARGET)

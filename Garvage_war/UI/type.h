#ifndef TYPES_H
#define TYPES_H

typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    float x;    // 左上の x 座標
    float y;    // 左上の y 座標
    float h;    // 高さ
    float w;    // 幅
} Rect;

#endif // TYPES_H

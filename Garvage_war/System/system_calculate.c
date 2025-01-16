/*****************************************************************
ファイル名	: system_calculate.c
内容		: ベクトルや行列の演算
*****************************************************************/
#include "system_struct.h"
#include "system_func.h"

//度数からラジアン(弧度)への変換
float glm_rad(float degrees) {
    return degrees * M_PI / 180.0f;
}

/*ベクトル演算*/
//正規化
Vector normalize(Vector v) {
    float length = sqrtf(v.vx * v.vx + v.vy * v.vy + v.vz * v.vz);
    if(length == 0.0f || length == 1.0f) return v;
    
    Vector nomarized_v = {v.vx / length, v.vy / length, v.vz / length};
    return nomarized_v; 
}
//外積
Vector cross(Vector v1, Vector v2) {
    Vector result = { v1.vy * v2.vz - v2.vy * v1.vz,
                      v1.vz * v2.vx - v2.vz * v1.vx,
                      v1.vx * v2.vy - v2.vx * v1.vy};
    return result;
}
//内積
float dot(Vector v1, Vector v2) {
    return v1.vx * v2.vx + v1.vy * v2.vy * v1.vz * v2.vz;
}
//正面のベクトルを得る
Vector get_forward(float yaw) {
    Vector forward = { cosf(glm_rad(yaw)),
                       0,
                       sinf(glm_rad(yaw))};
    forward = normalize(forward);
    return forward;
}
//正面からの水平右向きの垂直ベクトルを得る
Vector get_right(float yaw) {
    Vector right = { -sinf(glm_rad(yaw)),
                     0.0f,
                     cosf(glm_rad(yaw))};
    right = normalize(right);
    return right;
}

/*行列演算*/
//乗算(C = A × B)
void multiply_matrix(float A[3][3], float B[3][3], float C[3][3]) {
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            C[i][j] = 0;
            for(int k = 0; k < 3; ++k){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
//逆行列(inverseR = Rの逆行列)
void transpose_matrix(float R[3][3], float inverseR[3][3]) {
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            inverseR[j][i] = R[i][j];
        }
    }
}

/*回転演算*/
//各軸の回転行列の乗算(ZYXの順に計算)(r:回転量 R:回転行列)
void matrix(Rotate r, float R[3][3]){
    float cx = cos(r.rx), sx = -sin(r.rx);
    float cy = cos(r.ry), sy = -sin(r.ry);
    float cz = cos(r.rz), sz = -sin(r.rz);

    R[0][0] = cz * cy;
    R[0][1] = cz * sy * sx - sz * cx;
    R[0][2] = cz * sy * cx + sz * sx;
    R[1][0] = sz * cy;
    R[1][1] = sz * sy * sx + cz * cx;
    R[1][2] = sz * sy * cx - cz * sx;
    R[2][0] = -sy;
    R[2][1] = cy * sx;
    R[2][2] = cy * cx;
}
/* 有向ベクトルの座標回転の下位互換のためコメントアウト．向きがないやつに使えるかも？
//回転した座標を返す関数(c_p: 回転の中心座標,m_p: 回転する点, r: 回転量)
Point rotational_movement(Point c_p, Point m_p, Rotate r){
    //原点に移動
    Point s_p = {m_p.x - c_p.x, m_p.y - c_p.y, m_p.z - c_p.z};  
    Point moved_p;

    float R[3][3];
    matrix(r, R);

    moved_p.x = R[0][0] * s_p.x + R[0][1] * s_p.y + R[0][2] * s_p.z;
    moved_p.y = R[1][0] * s_p.x + R[1][1] * s_p.y + R[1][2] * s_p.z;
    moved_p.z = R[2][0] * s_p.x + R[2][1] * s_p.y + R[2][2] * s_p.z;

    moved_p.x += c_p.x;
    moved_p.y += c_p.y;
    moved_p.z += c_p.z;

    return moved_p;
}
*/
//回転軸に合わせた回転行列(v:回転軸 r:回転量 R:回転行列)
void transform_matrix(Vector v, Rotate r, float R[3][3]){
    Vector new_vz  = normalize(v);  //固有のZ軸

    Vector temp = {0, 1, 0};
    if(fabs(new_vz.vy) > 0.9f) {
        temp.vx = 1.0f; temp.vy = 0.0f; temp.vz = 0.0f;
    }

    Vector new_vx = normalize(cross(temp, new_vz));     //固有のX軸
    Vector new_vy = normalize(cross(new_vz, new_vx));   //固有のY軸

    float T[3][3] = { {new_vx.vx, new_vx.vy, new_vx.vz},
                      {new_vy.vx, new_vy.vy, new_vy.vz},
                      {new_vz.vx, new_vz.vy, new_vz.vz}};

    float temp1[3][3], temp2[3][3];
    matrix(r, temp1);
    multiply_matrix(T, temp1, temp2);
    transpose_matrix(T, temp1);
    multiply_matrix(temp1, temp2, R);
}
//向きに合わせて回転し，座標を返す(c_pからベクトルが出るイメージ)
//(c_p:中心座標 m_p:回転する点の座標 r:回転量 v:回転軸の向き)
Point rotate_point(Point c_p, Point m_p, Rotate r, Vector v){
    //原点に移動
    Point s_p = {m_p.x - c_p.x, m_p.y - c_p.y, m_p.z - c_p.z};
    Point moved_p;

    float R[3][3];
    transform_matrix(v, r, R);
    
    moved_p.x = R[0][0] * s_p.x + R[0][1] * s_p.y + R[0][2] * s_p.z;
    moved_p.y = R[1][0] * s_p.x + R[1][1] * s_p.y + R[1][2] * s_p.z;
    moved_p.z = R[2][0] * s_p.x + R[2][1] * s_p.y + R[2][2] * s_p.z;

    moved_p.x += c_p.x;
    moved_p.y += c_p.y;
    moved_p.z += c_p.z;

    return moved_p;
}
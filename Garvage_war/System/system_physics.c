/*****************************************************************
ファイル名	: system_physics.c
内容		: ゲームの物理処理
*****************************************************************/
#include "system_struct.h"
#include "system_func.h"

/*当たり判定の計算*/
bool collision(Object_Info obj1, Object_Info obj2){
    Vector min1 = {
        obj1.center.x - obj1.size.vx / 2,
        obj1.center.y - obj1.size.vy / 2,
        obj1.center.z - obj1.size.vz / 2
    };
    Vector max1 = {
        obj1.center.x + obj1.size.vx / 2,
        obj1.center.y + obj1.size.vy / 2,
        obj1.center.z + obj1.size.vz / 2
    };

    Vector min2 = {
        obj2.center.x - obj2.size.vx / 2,
        obj2.center.y - obj2.size.vy / 2,
        obj2.center.z - obj2.size.vz / 2
    };
    Vector max2 = {
        obj2.center.x + obj2.size.vx / 2,
        obj2.center.y + obj2.size.vy / 2,
        obj2.center.z + obj2.size.vz / 2
    };

    // 各軸での範囲が交差しているか判定
    return (min1.vx <= max2.vx && max1.vx >= min2.vx) &&
           (min1.vy <= max2.vy && max1.vy >= min2.vy) &&
           (min1.vz <= max2.vz && max1.vz >= min2.vz);
}

/*オブジェクトの距離の二乗を計算*/
float distance_squared(Object_Info obj1, Object_Info obj2){
    float x = obj1.center.x - obj2.center.x;
    float y = obj1.center.y - obj2.center.y;
    float z = obj1.center.z - obj2.center.z;

    return x*x + y*y + z*z;
}

/*プレイヤの移動*/
Point player_move(Player_Info info){
    Vector stick = info.input.stick;
    Point point = info.obj.center;
    Vector axis = info.obj.axis;

    float theta = atan2f(axis.vz, axis.vx);
    float fhi = atan2f(stick.vz, stick.vx);
    float vector_size = sqrtf(stick.vx * stick.vx + stick.vz * stick.vz);

    point.x += info.speed * vector_size * cosf(theta + fhi) * DELTA_TIME;
    point.z += info.speed * vector_size * sinf(theta + fhi) * DELTA_TIME;

    return point;
}

#define SENSITIVITY 1.0
/*プレイヤの向きの計算*/
Vector player_axis(Player_Info info){
    Vector stick = info.input.stick;
    static float yaw;
    static float pitch;
    yaw += stick.vx * SENSITIVITY;
    pitch += stick.vz * SENSITIVITY;
    Vector forward = { cosf(glm_rad(yaw)),
                       sinf(glm_rad(pitch)),
                       sinf(glm_rad(yaw))};
    return forward;
}

/*加速度計算*/
/*
void acceleration(Field_Info field, Object_Info obj, Vector velocity, PositionCallback callback){
    float time = 0.0;
    Vector resist;
    
    while(field.map[(int)(obj.center.x / MAP_GLID)][(int)(obj.center.z / MAP_GLID)] <= obj.center.y - obj.size.vy / 2){
        callback(obj.center);

        //抵抗値の算出（空気抵抗をもとに計算）
        resist.vx = -0.5 * (velocity.vx * fabs(velocity.vx)) * field.resist * (obj.size.vy * obj.size.vz);
        resist.vy = -0.5 * (velocity.vy * fabs(velocity.vy)) * field.resist * (obj.size.vz * obj.size.vx);
        resist.vx = -0.5 * (velocity.vz * fabs(velocity.vz)) * field.resist * (obj.size.vx * obj.size.vy);

        //速度を計算
        velocity.vx += obj.axis.vx * (field.force.vx + field.resist) * DELTA_TIME;
        velocity.vy += obj.axis.vy * (field.force.vy + field.resist) * DELTA_TIME;
        velocity.vz += obj.axis.vz * (field.force.vz + field.resist) * DELTA_TIME;

        //速さ（速度の絶対値）を計算
        float speed = sqrt(velocity.vx * velocity.vx + velocity.vy * velocity.vy + velocity.vz * velocity.vz);

        //方向ベクトルの更新
        obj.axis.vx = velocity.vx / speed;
        obj.axis.vy = velocity.vy / speed;
        obj.axis.vz = velocity.vz / speed;

        //位置の更新
        obj.center.x += velocity.vx * DELTA_TIME;
        obj.center.y += velocity.vy * DELTA_TIME;
        obj.center.z += velocity.vz * DELTA_TIME;

        time += DELTA_TIME;
    }
}
*/
/*アイテムスロットを整理する*/
void itemslot_sort(int slot[SLOT_MAX]){
    if(slot[0] == 0) return;
    for(int i = 1; i < slot[0] + 1; i++){
        if(slot[i] == 0){
            for(int j = i; j < slot[0] + 1; j++){
                slot[j] = slot[j + 1];
            }
        }
    }
}

/*マップ関連*/
// ランダム値生成
float random_range(float min, float max) {    
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

// 高さマップ生成
void generate_terrain(int terrain[MAP_SIZE][MAP_SIZE]) {
    for (int z = 0; z < MAP_SIZE; z++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            terrain[z][x] = 0; //sin((float)x / 10.0f) * cos((float)z / 10.0f) * 5.0f + random_range(-incline/2, incline/2);
        }
    }
}
//地面の下にいればTRUEを返す
bool under_ground(Object_Info *s){
    //地形情報のグローバル関数がないためコメントアウト
    return s->center.y < /*terrain[(int)s->center.z / MAP_GLID][(int)s->center.x / MAP_GLID]  + */s->size.vy / 2 ? true : false;
}

/*移動制限とめり込み回避*/
void adjust_point(Object_Info *s){
    //マップの移動制限
    if(s->center.x > (MAP_SIZE - 1) * MAP_GLID && s->center.x < 0){
        s->center.x = (s->center.x <= 0 ? 0.5f : (MAP_SIZE - 1) * MAP_GLID - 0.5f);
    }
    if(s->center.z > (MAP_SIZE - 1) * MAP_GLID && s->center.z < 0){
        s->center.z = (s->center.z <= 0 ? 0.5f : (MAP_SIZE - 1) * MAP_GLID - 0.5f);
    }
    if(under_ground(s)){
        s->center.y = /*terrain[(int)s->center.z / MAP_GLID][(int)s->center.x / MAP_GLID] + */s->size.vy / 2;
    }
}

//重力の計算
void gravity(Object_Info *s, float y_vector, float *timer) {
    s->center.y += y_vector * (*timer) - 0.5f * 9.8f * (*timer) * (*timer);
}

/*アイテムが拾える範囲にあるかを計算する*/
bool item_avilable(Player_Info p_info, Item_Info i_info){
    return distance_squared(p_info.obj,i_info.obj) < i_info.fixed.radius * i_info.fixed.radius ? true : false;
}

//放物運動の計算
void item_movement(Item_Info *info){
    if(info->active){
        if(!under_ground(&info->obj)){
            info->obj.center.x += info->obj.axis.vx * info->active_time * info->speed;
            info->obj.center.z += info->obj.axis.vz * info->active_time * info->speed;
            gravity(&info->obj, info->obj.axis.vy + 5, &info->active_time); 
            info->active_time += DELTA_TIME;
        }else{
            info->active = false;
            info->active_time = 0.0f;
        }
    }

    if(info->active_time < 0.3f){
        info->stts = IS_Having;
    }else if(info->active){
        info->stts = IS_Throwing;
    }else{
        info->stts = IS_OnField;
    }

    adjust_point(&info->obj);
}

/*アイテムを取得する*/
void get_item(Player_Info *p_info, Item_Info *i_info){
    if(item_avilable(*p_info, *i_info) && i_info->stts == IS_OnField && i_info->fixed.id != IN_Void){
        i_info->stts = IS_InSlot;
        for(int i = 0; i < SLOT_MAX; i++){
            if(p_info->having_item[i].stts != IS_InSlot){
                p_info->having_item[i] = *i_info;
            }
        }
    }
}


/*指定したステータスの最も近いアイテムを探す*/
Item_Info *search_nearest_item(Game_Info *info, int c_id, int stts){
    static Item_Info *nearest;
    float distance;
    float n_distance = -1;

    for(int i = 1; i < ITEM_ID_MAX; i++){
        if(info->item_info[i].stts == stts){
            distance = distance_squared(info->player_info[c_id].obj, info->item_info[i].obj);
            if(n_distance < 0 || n_distance > distance){
                n_distance = distance;
                nearest = &info->item_info[i];
            }
        }
    }

    if(n_distance == -1){
        return &info->item_info[IN_Void];
    }
    return nearest;
}
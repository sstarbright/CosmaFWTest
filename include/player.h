#ifndef PLAYER_H
#define PLAYER_H

#include "../CosmaFW/include/cfw.h"

typedef struct PlayerData PlayerData;

struct PlayerData {
    Vector2 position;
    Vector2 direction;
    float wallRadius;
};

#endif
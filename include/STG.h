#ifndef STG_H
#define STG_H

#include "TinyXml/tinyxml.h"
#include "Painter/Painter.h"
#include "Sonido/Sonido.h"
#include "Input/Input.h"
#include "Character/Character.h"
#include "Character/Player.h"
#include "Character/Enemy.h"
#include "Stage/Stage.h"

class STG
{
    Sonido* sonido;
    Painter* painter;
    Receiver* receiver;
    Player*player;
    Enemy*enemy;
    Stage*stage;

    bool isOutOfBounds(int pos_x,int pos_y);
    void mainLoop();
    void logic();
    void render();
    void deletePatterns(int stage_bound_x1,int stage_bound_y1,int stage_bound_x2,int stage_bound_y2);
    void checkCharacterOutOfBounds();
public:
    STG(Sonido* sonido,Painter* painter,Receiver* receiver,Player*player,Enemy*enemy,Stage*stage);
};

#endif

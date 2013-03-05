#ifndef ENEMY_H
#define ENEMY_H

#include <map>
#include <list>
#include <vector>

#include "TinyXml/tinyxml.h"
#include "Painter/Painter.h"
#include "Sonido/Sonido.h"
#include "Input/Input.h"
#include "Spells/Bullet.h"
#include "Spells/Pattern.h"
#include "Character/Character.h"
#include "Character/Player.h"

class Enemy : public Character
{
    std::map<int, vector<VariableChange*>* >variable_changes;
    float angle;
    float angle_change;
    Player*player;
public:
    Enemy(Sonido* sonido,Painter* painter,Receiver* receiver,std::string directory,Player*player);
    void logic(int stage_velocity);
    void modifiersControl();
    void render();
    void loadModifiersFromXML(std::string directory);
    virtual void addActivePattern(Pattern* pattern);
};

#endif
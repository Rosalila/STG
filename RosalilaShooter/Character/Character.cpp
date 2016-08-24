#include "Character.h"

Character::Character(std::string name,int sound_channel_base)
{
    //Setting up the other variables
    this->name=name;
    this->directory="";
    this->active_patterns=new std::list<Pattern*>;
    this->x=500;
    this->y=500;
    this->shooting=true;
    this->orientation="idle";
    this->current_type="1";
    this->visible=true;

    //Sprites animation
    this->animation_iteration=0;
    this->current_sprite=0;

    this->life_bar_x=0;
    this->life_bar_y=0;

    this->iteration=0;

    //Color effect
    current_color_effect_r=255;
    current_color_effect_g=255;
    current_color_effect_b=255;
    current_color_effect_a=255;

    //Shake
    current_screen_shake_x=0;
    current_screen_shake_y=0;
    shake_time=0;
    shake_magnitude=0;

    this->sound_channel_base=sound_channel_base;

    //Flat Shadow
    flat_shadow_texture = NULL;

    loadFromXML();
}

void Character::loadFromXML()
{
    loadMainXML();

    loadBulletsXML();

    loadPatternsXML();
}

void Character::loadMainXML()
{
    Node* root_node = Rosalila()->Parser->getNodes(assets_directory+directory+"main.xml");

    this->velocity=5;

    Node* attributes_node = root_node->getNodeByName("Attributes");

    if(attributes_node->hasAttribute("velocity"))
    {
        this->velocity=atoi(attributes_node->attributes["velocity"].c_str());
    }

    this->animation_velocity=5;
    if(attributes_node->hasAttribute("animation_velocity"))
    {
        this->animation_velocity=atoi(attributes_node->attributes["animation_velocity"].c_str());
    }

    this->max_hp=100;
    this->hp=100;
    if(attributes_node->hasAttribute("hp"))
    {
        this->max_hp=atoi(attributes_node->attributes["hp"].c_str());
    }
    this->hp=this->max_hp;

    this->x=100;
    if(attributes_node->hasAttribute("initial_x"))
    {
        this->x=atoi(attributes_node->attributes["initial_x"].c_str());
    }

    this->y=500;
    if(attributes_node->hasAttribute("initial_y"))
    {
        this->y=atoi(attributes_node->attributes["initial_y"].c_str());
    }

    this->life_bar_x=0;
    this->life_bar_y=0;
    this->life_bar_rect_offset_x=0;
    this->life_bar_rect_offset_y=0;
    this->life_bar_rect_height=0;
    this->life_bar_rect_width=0;
    this->color.red=0;
    this->color.green=0;
    this->color.blue=0;
    this->color.alpha=255;

    Node* life_bar_node = root_node->getNodeByName("LifeBar");

    if(life_bar_node!=NULL)
    {
        if(life_bar_node->hasAttribute("x"))
            this->life_bar_x=atoi(life_bar_node->attributes["x"].c_str());

        if(life_bar_node->hasAttribute("y"))
            this->life_bar_y=atoi(life_bar_node->attributes["y"].c_str());

        if(life_bar_node->hasAttribute("color_r"))
            this->color.red=atoi(life_bar_node->attributes["color_r"].c_str());

        if(life_bar_node->hasAttribute("color_g"))
            this->color.green=atoi(life_bar_node->attributes["color_g"].c_str());

        if(life_bar_node->hasAttribute("color_b"))
            this->color.blue=atoi(life_bar_node->attributes["color_b"].c_str());

        if(life_bar_node->hasAttribute("color_a"))
            this->color.alpha=atoi(life_bar_node->attributes["color_a"].c_str());

        if(life_bar_node->hasAttribute("rect_offset_x"))
            this->life_bar_rect_offset_x=atoi(life_bar_node->attributes["color_offset_x"].c_str());

        if(life_bar_node->hasAttribute("rect_offset_y"))
            this->life_bar_rect_offset_y==atoi(life_bar_node->attributes["color_offset_y"].c_str());

        if(life_bar_node->hasAttribute("rect_height"))
            this->life_bar_rect_height=atoi(life_bar_node->attributes["rect_height"].c_str());

        if(life_bar_node->hasAttribute("rect_width"))
            this->life_bar_rect_width=atoi(life_bar_node->attributes["rect_width"].c_str());

        if(life_bar_node->hasAttribute("image"))
            this->life_bar=Rosalila()->Graphics->getTexture(assets_directory+directory+life_bar_node->attributes["image"]);
    }

    Node* hitboxes_node = root_node->getNodeByName("Hitboxes");

    for(map<string,Node*>::iterator hitbox_iterator=hitboxes_node->nodes.begin();
        hitbox_iterator!=hitboxes_node->nodes.end();
        hitbox_iterator++)
    {
        Node* hitbox_node=hitbox_iterator->second;

        int hitbox_x=atoi(hitbox_node->attributes["x"].c_str());
        int hitbox_y=atoi(hitbox_node->attributes["y"].c_str());
        int hitbox_width=atoi(hitbox_node->attributes["width"].c_str());
        int hitbox_height=atoi(hitbox_node->attributes["height"].c_str());
        int hitbox_angle=atoi(hitbox_node->attributes["angle"].c_str());

        Hitbox* hitbox=new Hitbox(hitbox_x,hitbox_y,hitbox_width,hitbox_height,hitbox_angle);
        this->hitboxes.push_back(hitbox);

    }


    Node* sounds_node = root_node->getNodeByName("Sounds");

    if(sounds_node)
    {
        if(sounds_node->hasAttribute("hit"))
        {
            Rosalila()->Sound->addSound(this->name+".hit",assets_directory+directory+"/sounds/"+sounds_node->attributes["hit"]);
        }
    }

    vector<Node*> sprites_nodes = root_node->getNodesByName("Sprites");

    for(int i=0;i<sprites_nodes.size();i++)
    {
        std::vector<Image*>sprites_vector;

        std::string sprites_orientation=sprites_nodes[i]->attributes["orientation"];

        if(sprites_nodes[i]->hasAttribute("sound"))
        {
            std::string sprites_sound=sprites_nodes[i]->attributes["sound"];
            Rosalila()->Sound->addSound(name+"."+sprites_orientation,assets_directory+directory+"sounds/"+sprites_sound);
        }

        vector<Node*> sprite_nodes = sprites_nodes[i]->getNodesByName("Sprite");

        for(int j=0;j<sprite_nodes.size();j++)
        {
            string sprite_path = sprite_nodes[j]->attributes["path"];
            sprites_vector.push_back(Rosalila()->Graphics->getTexture(assets_directory+directory+"sprites/"+sprite_path));
        }

        sprites[sprites_orientation]=sprites_vector;
    }

    Node* flat_shadow_node = root_node->getNodeByName("FlatShadow");

    if(flat_shadow_node)
    {
        flat_shadow_texture = Rosalila()->Graphics->getTexture(assets_directory+directory+"sprites/"+flat_shadow_node->attributes["image_path"]);

        Node* case_right_node = flat_shadow_node->getNodeByName("CaseRight");

        if(case_right_node)
        {
            vector<Node*> case_right_point_nodes = case_right_node->getNodesByName("Point");

            for(int i=0;i<case_right_point_nodes.size();i++)
            {
                int x=atoi(case_right_point_nodes[i]->attributes["x"].c_str());
                int y=atoi(case_right_point_nodes[i]->attributes["y"].c_str());
                shadow_align_points_right.push_back(new Point(x,y));
            }

            Node* case_right_inbetween_node = case_right_node->getNodeByName("Inbetween");

            if(case_right_inbetween_node)
            {
                vector<Node*> case_right_inbetween_point_nodes = case_right_inbetween_node->getNodesByName("Point");

                for(int i=0;i<case_right_inbetween_point_nodes.size();i++)
                {
                    int x=atoi(case_right_inbetween_point_nodes[i]->attributes["x"].c_str());
                    int y=atoi(case_right_inbetween_point_nodes[i]->attributes["y"].c_str());
                    inbetween_shadow_align_points_right.push_back(new Point(x,y));
                }
            }
        }

        Node* case_left_node = flat_shadow_node->getNodeByName("CaseLeft");

        if(case_left_node)
        {
            vector<Node*> case_left_point_nodes = case_left_node->getNodesByName("Point");

            for(int i=0;i<case_left_point_nodes.size();i++)
            {
                int x=atoi(case_left_point_nodes[i]->attributes["x"].c_str());
                int y=atoi(case_left_point_nodes[i]->attributes["y"].c_str());
                shadow_align_points_left.push_back(new Point(x,y));
            }

            Node* case_left_inbetween_node = case_left_node->getNodeByName("Inbetween");

            if(case_left_inbetween_node)
            {
                vector<Node*> case_left_inbetween_point_nodes = case_left_inbetween_node->getNodesByName("Point");

                for(int i=0;i<case_left_inbetween_point_nodes.size();i++)
                {
                    int x=atoi(case_left_inbetween_point_nodes[i]->attributes["x"].c_str());
                    int y=atoi(case_left_inbetween_point_nodes[i]->attributes["y"].c_str());
                    inbetween_shadow_align_points_left.push_back(new Point(x,y));
                }
            }
        }

        Node* case_top_node = flat_shadow_node->getNodeByName("CaseTop");

        if(case_top_node)
        {
            vector<Node*> case_top_point_nodes = case_top_node->getNodesByName("Point");

            for(int i=0;i<case_top_point_nodes.size();i++)
            {
                int x=atoi(case_top_point_nodes[i]->attributes["x"].c_str());
                int y=atoi(case_top_point_nodes[i]->attributes["y"].c_str());
                shadow_align_points_top.push_back(new Point(x,y));
            }

            Node* case_top_inbetween_node = case_top_node->getNodeByName("Inbetween");

            if(case_top_inbetween_node)
            {
                vector<Node*> case_top_inbetween_point_nodes = case_top_node->getNodeByName("Inbetween")->getNodesByName("Point");

                for(int i=0;i<case_top_inbetween_point_nodes.size();i++)
                {
                    int x=atoi(case_top_inbetween_point_nodes[i]->attributes["x"].c_str());
                    int y=atoi(case_top_inbetween_point_nodes[i]->attributes["y"].c_str());
                    inbetween_shadow_align_points_top.push_back(new Point(x,y));
                }
            }
        }
    }
}

void Character::loadBulletsXML()
{
    //Loading bullets
    std::string bullets_path=assets_directory+directory+"bullets.xml";
    TiXmlDocument doc_bullets_t(bullets_path.c_str());
    doc_bullets_t.LoadFile();
    TiXmlDocument *doc_bullets;
    doc_bullets=&doc_bullets_t;
    TiXmlNode *bullet_file=doc_bullets->FirstChild("BulletsFile");

    //Loading bullets
    for(TiXmlNode* bullet_node=bullet_file->FirstChild("Bullet");
            bullet_node!=NULL;
            bullet_node=bullet_node->NextSibling("Bullet"))
    {
        std::string node_name=bullet_node->ToElement()->Attribute("name");
        vector<string> random_sounds;

        if(bullet_node->ToElement()->Attribute("sound")!=NULL)
        {
            std::string sound=assets_directory+directory+"sounds/"+bullet_node->ToElement()->Attribute("sound");
            Rosalila()->Sound->addSound("bullet."+node_name,sound);
            random_sounds.push_back("bullet."+node_name);
        }
        if(bullet_node->ToElement()->Attribute("sound_hit")!=NULL)
        {
            std::string sound_hit=assets_directory+directory+"sounds/"+bullet_node->ToElement()->Attribute("sound_hit");
            Rosalila()->Sound->addSound("bullet_hit."+node_name,sound_hit);
        }

        int randomize_sound_frequency=1;
        if(bullet_node->ToElement()->Attribute("randomize_sound_frequency")!=NULL)
        {
            randomize_sound_frequency=atoi(bullet_node->ToElement()->Attribute("randomize_sound_frequency"));
        }

        int arpeggio_length=1;
        if(bullet_node->ToElement()->Attribute("arpeggio_length")!=NULL)
        {
            arpeggio_length=atoi(bullet_node->ToElement()->Attribute("arpeggio_length"));
        }

        int sound_channel=0;
        if(bullet_node->ToElement()->Attribute("sound_channel")!=NULL)
        {
            sound_channel=atoi(bullet_node->ToElement()->Attribute("sound_channel"));
            if(sound_channel!=-1)
                sound_channel+=sound_channel_base;
        }

        int damage=0;
        if(bullet_node->ToElement()->Attribute("damage")!=NULL)
        {
            damage=atoi(bullet_node->ToElement()->Attribute("damage"));
        }

        vector<Image*>sprites_temp;
        for(TiXmlNode* sprite_node=bullet_node->FirstChild("Sprite");
                sprite_node!=NULL;
                sprite_node=sprite_node->NextSibling("Sprite"))
        {
            sprites_temp.push_back(Rosalila()->Graphics->getTexture(assets_directory+directory+"sprites/"+sprite_node->ToElement()->Attribute("path")));
        }

        vector<Hitbox*>hitboxes_temp;
        for(TiXmlNode* hitbox_node=bullet_node->FirstChild("Hitbox");
                hitbox_node!=NULL;
                hitbox_node=hitbox_node->NextSibling("Hitbox"))
        {
            int x=0;
            if(hitbox_node->ToElement()->Attribute("x")!=NULL)
            {
                x=atoi(hitbox_node->ToElement()->Attribute("x"));
            }
            int y=0;
            if(hitbox_node->ToElement()->Attribute("y")!=NULL)
            {
                y=atoi(hitbox_node->ToElement()->Attribute("y"));
            }

            int width=atoi(hitbox_node->ToElement()->Attribute("width"));
            int height=atoi(hitbox_node->ToElement()->Attribute("height"));

            int angle=0;
            if(hitbox_node->ToElement()->Attribute("angle")!=NULL)
            {
                angle=atoi(hitbox_node->ToElement()->Attribute("angle"));
            }

            hitboxes_temp.push_back(new Hitbox(x,y,width,height,angle));
        }

        TiXmlNode* onhit_node=bullet_node->FirstChild("OnHit");
        vector<Image*>sprites_onhit_temp;
        if(onhit_node!=NULL)
        {
            for(TiXmlNode* sprite_node=onhit_node->FirstChild("Sprite");
                    sprite_node!=NULL;
                    sprite_node=sprite_node->NextSibling("Sprite"))
            {
                sprites_onhit_temp.push_back(Rosalila()->Graphics->getTexture(assets_directory+directory+"sprites/"+sprite_node->ToElement()->Attribute("path")));
            }
        }

        TiXmlNode* random_sound_node=bullet_node->FirstChild("RandomSound");
        if(random_sound_node!=NULL)
        {
            for(TiXmlNode* sound_node=random_sound_node->FirstChild("Sound");
                    sound_node!=NULL;
                    sound_node=sound_node->NextSibling("Sound"))
            {
                std::string sound=assets_directory+directory+"sounds/"+sound_node->ToElement()->Attribute("path");
                Rosalila()->Sound->addSound("bullet."+node_name+sound_node->ToElement()->Attribute("path"),sound);
                random_sounds.push_back("bullet."+node_name+sound_node->ToElement()->Attribute("path"));
            }
        }

        bullets[node_name]=new Bullet(node_name,sprites_temp,sprites_onhit_temp,hitboxes_temp,random_sounds,randomize_sound_frequency,arpeggio_length,damage,sound_channel);
    }
}

Pattern* Character::loadPatternXML(TiXmlNode* pattern_node)
{
    TiXmlElement* pattern_element = pattern_node->ToElement();
    std::string bullet_name=pattern_element->Attribute("bullet");
    Bullet*bullet=bullets[bullet_name];

    int velocity=0;
    if(pattern_element->Attribute("velocity")!=NULL)
        velocity=atoi(pattern_element->Attribute("velocity"));

    int max_velocity=9999999;
    if(pattern_element->Attribute("max_velocity")!=NULL)
        max_velocity=atoi(pattern_element->Attribute("max_velocity"));

    int acceleration=0;
    if(pattern_element->Attribute("acceleration")!=NULL)
        acceleration=atoi(pattern_element->Attribute("acceleration"));

    int a_frequency=0;
    if(pattern_element->Attribute("a_frequency")!=NULL)
        a_frequency=atoi(pattern_element->Attribute("a_frequency"));

    int angle=0;
    if(pattern_element->Attribute("angle")!=NULL)
        angle=atoi(pattern_element->Attribute("angle"));

    int angle_change=0;
    if(pattern_element->Attribute("angle_change")!=NULL)
        angle_change=atoi(pattern_element->Attribute("angle_change"));

    int stop_ac_at=-1;
    if(pattern_element->Attribute("stop_ac_at")!=NULL)
        stop_ac_at=atoi(pattern_element->Attribute("stop_ac_at"));

    int ac_frequency=0;
    if(pattern_element->Attribute("ac_frequency")!=NULL)
        ac_frequency=atoi(pattern_element->Attribute("ac_frequency"));

    int offset_x=0;
    if(pattern_element->Attribute("offset_x")!=NULL)
        offset_x=atoi(pattern_element->Attribute("offset_x"));

    int offset_y=0;
    if(pattern_element->Attribute("offset_y")!=NULL)
        offset_y=atoi(pattern_element->Attribute("offset_y"));

    int startup=0;
    if(pattern_element->Attribute("startup")!=NULL)
        startup=atoi(pattern_element->Attribute("startup"));

    int cooldown=0;
    if(pattern_element->Attribute("cooldown")!=NULL)
        cooldown=atoi(pattern_element->Attribute("cooldown"));

    int animation_velocity=0;
    if(pattern_element->Attribute("animation_velocity")!=NULL)
        animation_velocity=atoi(pattern_element->Attribute("animation_velocity"));

    double auto_scale=0;
    if(pattern_element->Attribute("auto_scale"))
        auto_scale=atof(pattern_element->Attribute("auto_scale"));

    int duration=-1;
    if(pattern_element->Attribute("duration"))
        duration=atoi(pattern_element->Attribute("duration"));

    int random_angle=0;
    if(pattern_element->Attribute("random_angle"))
        random_angle=atoi(pattern_element->Attribute("random_angle"));

    bool aim_player=false;
    if(pattern_element->Attribute("aim_player"))
        aim_player=strcmp(pattern_element->Attribute("aim_player"),"yes")==0;

    int bullet_rotation=0;
    if(pattern_element->Attribute("bullet_rotation"))
        bullet_rotation=atoi(pattern_element->Attribute("bullet_rotation"));

    int br_change=0;
    if(pattern_element->Attribute("br_change"))
        br_change=atoi(pattern_element->Attribute("br_change"));

    bool independent_br=false;
    if(pattern_element->Attribute("independent_br"))
        independent_br=strcmp(pattern_element->Attribute("independent_br"),"yes")==0;

    bool freeze=false;
    if(pattern_element->Attribute("freeze"))
        freeze=strcmp(pattern_element->Attribute("freeze"),"yes")==0;

    bool homing = false;
    if (pattern_element->Attribute("homing"))
         homing = strcmp(pattern_element->Attribute("homing"), "yes") == 0;

    bool collides_bullets=false;
    if(pattern_element->Attribute("collides_bullets"))
        collides_bullets=strcmp(pattern_element->Attribute("collides_bullets"),"yes")==0;

    bool collides_opponent=true;
    if(pattern_element->Attribute("collides_opponent"))
        collides_opponent=strcmp(pattern_element->Attribute("collides_opponent"),"yes")==0;

    bool undestructable=false;
    if(pattern_element->Attribute("undestructable"))
        undestructable=strcmp(pattern_element->Attribute("undestructable"),"yes")==0;

    //Modifiers
    std::map<int, vector<Modifier*>* >*pattern_modifiers=new std::map<int, vector<Modifier*>* >();

    if(pattern_node->FirstChild("Modifier")!=NULL)
    {
        for(TiXmlNode* pattern_modifier_node=pattern_node->FirstChild("Modifier");
                pattern_modifier_node!=NULL;
                pattern_modifier_node=pattern_modifier_node->NextSibling("Modifier"))
        {
            int at=atoi(pattern_modifier_node->ToElement()->Attribute("at"));
            (*pattern_modifiers)[at]=loadModifierXML(pattern_modifier_node);
        }
    }
    return new Pattern(velocity,max_velocity,acceleration,a_frequency,angle,angle_change,stop_ac_at,ac_frequency,animation_velocity, auto_scale,bullet,offset_x,offset_y,
                                           startup,cooldown,duration,random_angle,aim_player,bullet_rotation,br_change,independent_br,freeze, homing,collides_bullets,collides_opponent,undestructable,pattern_modifiers,&bullets);
}

vector<Modifier*>* Character::loadModifierXML(TiXmlNode* modifier_node)
{
    vector<Modifier*>* temp_modifiers=new vector<Modifier*>();

    if(modifier_node->ToElement()->Attribute("bullet")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("bullet");
        temp_modifiers->push_back(new Modifier("bullet",value));
    }

    if(modifier_node->ToElement()->Attribute("velocity")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("velocity");
        temp_modifiers->push_back(new Modifier("velocity",value));
    }

    if(modifier_node->ToElement()->Attribute("max_velocity")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("max_velocity");
        temp_modifiers->push_back(new Modifier("max_velocity",value));
    }

    if(modifier_node->ToElement()->Attribute("acceleration")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("acceleration");
        temp_modifiers->push_back(new Modifier("acceleration",value));
    }

    if(modifier_node->ToElement()->Attribute("a_frequency")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("a_frequency");
        temp_modifiers->push_back(new Modifier("a_frequency",value));
    }

    if(modifier_node->ToElement()->Attribute("angle")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("angle");
        temp_modifiers->push_back(new Modifier("angle",value));
    }

    if(modifier_node->ToElement()->Attribute("angle_change")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("angle_change");
        temp_modifiers->push_back(new Modifier("angle_change",value));
    }

    if(modifier_node->ToElement()->Attribute("stop_ac_at")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("stop_ac_at");
        temp_modifiers->push_back(new Modifier("stop_ac_at",value));
    }

    if(modifier_node->ToElement()->Attribute("ac_frequency")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("ac_frequency");
        temp_modifiers->push_back(new Modifier("ac_frequency",value));
    }

    if(modifier_node->ToElement()->Attribute("animation_velocity")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("animation_velocity");
        temp_modifiers->push_back(new Modifier("animation_velocity",value));
    }

    if(modifier_node->ToElement()->Attribute("offset_x")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("offset_x");
        temp_modifiers->push_back(new Modifier("offset_x",value));
    }

    if(modifier_node->ToElement()->Attribute("offset_y")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("offset_y");
        temp_modifiers->push_back(new Modifier("offset_y",value));
    }

    if(modifier_node->ToElement()->Attribute("startup")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("startup");
        temp_modifiers->push_back(new Modifier("startup",value));
    }

    if(modifier_node->ToElement()->Attribute("cooldown")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("cooldown");
        temp_modifiers->push_back(new Modifier("cooldown",value));
    }

    if(modifier_node->ToElement()->Attribute("duration")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("duration");
        temp_modifiers->push_back(new Modifier("duration",value));
    }

    if(modifier_node->ToElement()->Attribute("random_angle")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("random_angle");
        temp_modifiers->push_back(new Modifier("random_angle",value));
    }

    if(modifier_node->ToElement()->Attribute("aim_player")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("aim_player");
        temp_modifiers->push_back(new Modifier("aim_player",value));
    }

    if(modifier_node->ToElement()->Attribute("bullet_rotation")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("bullet_rotation");
        temp_modifiers->push_back(new Modifier("bullet_rotation",value));
    }

    if(modifier_node->ToElement()->Attribute("br_change")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("br_change");
        temp_modifiers->push_back(new Modifier("br_change",value));
    }

    if(modifier_node->ToElement()->Attribute("independent_br")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("independent_br");
        temp_modifiers->push_back(new Modifier("independent_br",value));
    }
    if(modifier_node->ToElement()->Attribute("freeze")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("freeze");
        temp_modifiers->push_back(new Modifier("freeze",value));
    }

    if(modifier_node->ToElement()->Attribute("homing")!=NULL)
    {
        std::string value=modifier_node->ToElement()->Attribute("homing");
        temp_modifiers->push_back(new Modifier("homing",value));
    }

    return temp_modifiers;
}

void Character::loadPatternsXML()
{
//Loading file
    std::string pattern_path=assets_directory+directory+"patterns.xml";
    TiXmlDocument doc_pattern_t(pattern_path.c_str());
    doc_pattern_t.LoadFile();
    TiXmlDocument *doc_pattern;
    doc_pattern=&doc_pattern_t;
    TiXmlNode *patterns_file=doc_pattern->FirstChild("PatternsFile");

    //Loading patterns
    for(TiXmlNode* pattern_type=patterns_file->FirstChild("Type");
            pattern_type!=NULL;
            pattern_type=pattern_type->NextSibling("Type"))
    {
        std::string type_name=pattern_type->ToElement()->Attribute("name");
        std::vector<Pattern*> patterns;
        for(TiXmlNode* pattern_node=pattern_type->FirstChild("Pattern");
                pattern_node!=NULL;
                pattern_node=pattern_node->NextSibling("Pattern"))
        {
            //Pattern ready, now push
            patterns.push_back(loadPatternXML(pattern_node));
        }
        for(TiXmlNode* repeat_node=pattern_type->FirstChild("Repeat");
                repeat_node!=NULL;
                repeat_node=repeat_node->NextSibling("Repeat"))
        {

            for(TiXmlNode* pattern_node=repeat_node->FirstChild("Pattern");
                    pattern_node!=NULL;
                    pattern_node=pattern_node->NextSibling("Pattern"))
            {
                TiXmlElement* repeat_element = repeat_node->ToElement();
                int amount = atoi(repeat_element->Attribute("amount"));

                for(int i=0;i<amount;i++)
                {
                    Pattern* p = loadPatternXML(pattern_node);

                    if(repeat_element->Attribute("velocity")!=NULL)
                        p->velocity+=atoi(repeat_element->Attribute("velocity"))*i;

                    if(repeat_element->Attribute("max_velocity")!=NULL)
                        p->max_velocity+=atoi(repeat_element->Attribute("max_velocity"))*i;

                    if(repeat_element->Attribute("acceleration")!=NULL)
                        p->acceleration+=atoi(repeat_element->Attribute("acceleration"))*i;

                    if(repeat_element->Attribute("a_frequency")!=NULL)
                        p->a_frequency+=atoi(repeat_element->Attribute("a_frequency"))*i;

                    if(repeat_element->Attribute("angle")!=NULL)
                        p->angle+=atoi(repeat_element->Attribute("angle"))*i;

                    if(repeat_element->Attribute("angle_change")!=NULL)
                        p->angle_change+=atoi(repeat_element->Attribute("angle_change"))*i;

                    if(repeat_element->Attribute("stop_ac_at")!=NULL)
                        p->stop_ac_at+=atoi(repeat_element->Attribute("stop_ac_at"))*i;

                    if(repeat_element->Attribute("ac_frequency")!=NULL)
                        p->ac_frequency+=atoi(repeat_element->Attribute("ac_frequency"))*i;

                    if(repeat_element->Attribute("offset_x")!=NULL)
                        p->offset_x+=atoi(repeat_element->Attribute("offset_x"))*i;

                    if(repeat_element->Attribute("offset_y")!=NULL)
                        p->offset_y+=atoi(repeat_element->Attribute("offset_y"))*i;

                    if(repeat_element->Attribute("startup")!=NULL)
                        p->startup+=atoi(repeat_element->Attribute("startup"))*i;

                    if(repeat_element->Attribute("cooldown")!=NULL)
                        p->cooldown+=atoi(repeat_element->Attribute("cooldown"))*i;

                    if(repeat_element->Attribute("animation_velocity")!=NULL)
                        p->animation_velocity+=atoi(repeat_element->Attribute("animation_velocity"))*i;

                    if(repeat_element->Attribute("duration"))
                        p->duration+=atoi(repeat_element->Attribute("duration"))*i;

                    if(repeat_element->Attribute("random_angle"))
                        p->random_angle+=atoi(repeat_element->Attribute("random_angle"))*i;

                    if(repeat_element->Attribute("bullet_rotation"))
                        p->bullet_rotation+=atoi(repeat_element->Attribute("bullet_rotation"))*i;

                    if(repeat_element->Attribute("br_change"))
                        p->br_change+=atoi(repeat_element->Attribute("br_change"))*i;

                    patterns.push_back(p);
                }
            }
        }
        type[type_name]=patterns;
    }
}

void Character::logic(int stage_velocity)
{
    animationControl();
    spellControl(stage_velocity);
    if(current_color_effect_r<255)
        current_color_effect_r++;
    if(current_color_effect_g<255)
        current_color_effect_g++;
    if(current_color_effect_b<255)
        current_color_effect_b++;
    if(current_color_effect_a<255)
        current_color_effect_a++;
}

void Character::animationControl()
{
    if(orientation=="destroyed")
        visible=false;
    if(animation_iteration>=animation_velocity)
    {
        current_sprite++;
        if(current_sprite>=(int)sprites[orientation].size())
        {
            current_sprite=0;
        }
        animation_iteration=0;
    }
    if(getIterateSlowdownFlag())
        animation_iteration++;
}

void Character::spellControl(int stage_velocity)
{
    std::vector<Pattern*> patterns=type[current_type];
    for(int i=0;i<(int)patterns.size();i++)
    {
        if(shooting && this->hp!=0)
        {
            patterns[i]->updateStateShouting();
            if(patterns[i]->isReady())
            {
                patterns[i]->bullet->playSound();
                this->addActivePattern(patterns[i]);
            }
        }else
        {
            patterns[i]->updateStateNotShouting();
        }
    }

    for (std::list<Pattern*>::iterator pattern = active_patterns->begin(); pattern != active_patterns->end(); pattern++)
        ((Pattern*)*pattern)->logic(stage_velocity);
}

void Character::bottomRender()
{
    if(shake_time>0)
    {
        shake_time--;
        if(shake_time==0)
        {
            current_screen_shake_x = 0;
            current_screen_shake_y = 0;
        }else
        {
            current_screen_shake_x = (rand()*10000)%shake_magnitude;
            current_screen_shake_y = (rand()*10000)%shake_magnitude;
        }
    }

    int transparency_divider=1;
    if(getGameOver())
        transparency_divider=8;

    Rosalila()->Graphics->draw2DImage
    (   sprites[orientation][current_sprite],
        sprites[orientation][current_sprite]->getWidth(),sprites[orientation][current_sprite]->getHeight(),
        this->x-sprites[orientation][current_sprite]->getWidth()/2+current_screen_shake_x,
        this->y-sprites[orientation][current_sprite]->getHeight()/2+current_screen_shake_y,
        1.0,
        0.0,
        false,
        0,0,
        Color(current_color_effect_r,current_color_effect_g,current_color_effect_b,current_color_effect_a/transparency_divider),
        0,0,
        true,
        FlatShadow(flat_shadow_texture,2,60,0,700,-500,
                   shadow_align_points_left,shadow_align_points_right,shadow_align_points_top,
                   inbetween_shadow_align_points_left,inbetween_shadow_align_points_right,inbetween_shadow_align_points_top));

    if(Rosalila()->Receiver->isKeyDown(SDLK_h))
    {
        for(int i=0;i<(int)hitboxes.size();i++)
        {
            Rosalila()->Graphics->drawRectangle(hitboxes[i]->getX()+x,
                                   hitboxes[i]->getY()+y,
                                   hitboxes[i]->getWidth(),hitboxes[i]->getHeight(),
                                   hitboxes[i]->angle,100,0,0,100,true);
        }
    }
}

void Character::topRender()
{
    for (std::list<Pattern*>::iterator pattern = active_patterns->begin(); pattern != active_patterns->end(); pattern++)
        ((Pattern*)*pattern)->render();
}

void Character::setOrientation(string orientation)
{
    this->orientation=orientation;
    this->current_sprite=0;
}

bool Character::collides(Hitbox hitbox,int hitbox_x,int hitbox_y,float hitbox_angle)
{
    if(!visible)
        return false;
    for(int i=0;i<(int)hitboxes.size();i++)
        if(hitboxes[i]->getPlacedHitbox(this->x,this->y).collides(hitbox))
            return true;
    return false;
}

void Character::hit(int damage)
{
    this->hp-=damage;
    if(hp<0)
        hp=0;
}

void Character::addActivePattern(Pattern* pattern)
{
    Pattern* pattern_temp=new Pattern(pattern,this->x,this->y);
    float angle=pattern_temp->angle;
    angle+=pattern_temp->getRandomAngle();
    pattern_temp->angle=angle;

    active_patterns->push_back(pattern_temp);
}

void Character::shakeScreen(int shake_magnitude, int shake_time)
{
    this->shake_magnitude=shake_magnitude;
    this->shake_time=shake_time;
}

void Character::deleteActivePatterns()
{
    std::list<Pattern*>::iterator i = active_patterns->begin();
    while (i != active_patterns->end())
    {
        Pattern*p=(Pattern*)*i;
        p->hit(sound_channel_base+1,true);
        i++;
    }
}

Character::~Character()
{
    for(int i=0;i<(int)hitboxes.size();i++)
    {
        delete hitboxes[i];
    }
}
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

//SDL
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>

#include "Input/Input.h"
#include "Painter/Painter.h"
#include "RosalilaSound/RosalilaSound.h"
#include "STGMenu/STGMenu.h"
#include "Utility/Utility.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

    clearLog();
    //Creadas abierto
    Receiver* receiver=new Receiver();
    Input*inputa=new Input();
    Input*inputb=new Input();
    inputa->cargarDesdeXML(1,receiver);
    inputb->cargarDesdeXML(2,receiver);

    Painter*painter=new Painter();

    painter->video(painter);

//painter->update.start();
//painter->fps.start();


    Sound*sonido = new Sound();
    Menu* menu=new Menu(painter,receiver,sonido,(char*)"menu/main_menu.svg");
    sonido->playMusic(menu->music_path);

    menu->loopMenu();
    return 0;
}

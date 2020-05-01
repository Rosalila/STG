#include "Menus/TextInput.h"

string textInput(string label)
{
  Image *background = rosalila()->graphics->getImage(std::string(assets_directory) + "menu/white_background.png");
  background->color_filter.red = 0;
  background->color_filter.green = 0;
  background->color_filter.blue = 0;
  background->width = rosalila()->graphics->screen_width;
  background->height = rosalila()->graphics->screen_height;

  string input = "";
  SDL_StartTextInput();
  SDL_Event event;
  bool input_finished = false;

  while (!input_finished)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        input_finished = true;
      }
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_RETURN && event.type == SDL_KEYDOWN)
        {
          input_finished = true;
        }
        else if (event.key.keysym.sym == SDLK_BACKSPACE && input.length() > 0)
        {
          input = input.substr(0, input.size() - 1);
        }
      }
      else if (event.type == SDL_TEXTINPUT)
      {
        if (!((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
        {
          input += event.text.text;
        }
      }
      else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
      {
        input = SDL_GetClipboardText();
      }
    }

    rosalila()->graphics->drawImage(background, 0, 0);

    rosalila()->graphics->drawText(label, 0, rosalila()->graphics->screen_height / 2 - 200, true, false);
    rosalila()->graphics->drawText(input, 0, 0, true, true);

    rosalila()->graphics->updateScreen();
  }
  SDL_StopTextInput();
  return input;
}

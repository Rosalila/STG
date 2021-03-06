#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include "Rosalila/Rosalila.h"
#include "Stage/LayerFrame.h"
#include "Character/Modifier.h"
using namespace std;

class Player;

class Layer
{
public:
  vector<LayerFrame *> layer_frames;

  Color color;

  std::string type;

  int frame_duration;
  int depth_effect_x;
  int depth_effect_y;
  double x;
  double y;
  double velocity;
  double angle;
  double angle_change;
  int separation_x;

  bool blend_effect;

  int current_frame;
  int time_elapsed;

  double frame;

  // Bounds
  bool is_bounds_active;
  int bounds_x;
  int bounds_y;
  int bounds_width;
  int bounds_height;

  // Obstacle
  int obstacle_hit_points;

  map<int, vector<Modifier *>> *modifiers;

  Layer(vector<LayerFrame *> layer_frames,
        std::map<int, vector<Modifier *>> *modifiers,
        std::string type,
        int frame_duration,
        int depth_effect_x, int depth_effect_y,
        double x, double y,
        double velocity,
        double angle,
        double angle_change,
        int separation_x,
        bool is_bounds_active,
        int bounds_x,
        int bounds_y,
        int bounds_width,
        int bounds_height,
        bool blend_effect,
        int obstacle_hit_points,
        Color color);
  
  bool playerIsInBounds(Player* player);
  bool pointIsInBounds(int x, int y);
  bool playerIsInObstacle(Player* player);
  void modifiersControl();
  void logic(Player* player);

  ~Layer();
};

#endif

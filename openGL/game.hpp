//
//  game.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef game_hpp
#define game_hpp

// Library Includes
#include <irrKlang.h>

// System Includes
#include <vector>

// Local Includes
#include "ball.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "post_processor.hpp"
#include "powerup.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"

enum class GameState {
  ACTIVE,
  MENU,
  WIN
};

enum class Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
public:
  Game(const unsigned int width, const unsigned int height);
  ~Game();
  
  void init();
  void process_input(const float delta_time);
  void update(const float delta_time);
  void render();

  void set_key(const unsigned int key, const bool flag);

private:
  void calculate_collisions();
  bool check_collision(const GameObject& one, const GameObject& two);
  Collision check_collision(const Ball& ball, const GameObject& rect);
  Direction determine_vector_direction(glm::vec2 vector);
  void reset_level();
  void reset_player();
  bool should_spawn_powerup(const unsigned int chance);
  void spawn_powerups(GameObject& rect);
  void update_powerups(const float delta_time);
  void activate_powerup(PowerUp& powerup);
  bool is_other_powerup_active(std::string type);
  
  GameState state_;
  bool keys_[1024];
  unsigned int width_;
  unsigned int height_;
  float shake_time_ = 0.0f;
  
  std::vector<GameLevel> levels_;
  unsigned int level_;
  
  std::vector<PowerUp> powerups_;
  
  GameObject *player_;
  Ball *ball_;
  PostProcessor *effects_;
  SpriteRenderer *renderer_;
  irrklang::ISoundEngine *sound_engine_;
  
  // Constants
  const glm::vec2 PLAYER_SIZE = glm::vec2(100.0f, 20.0f);
  const float PLAYER_VELOCITY = 500.0f;
  const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(100.0f, -350.0f);
  const float BALL_RADIUS = 12.5f;
  
};

#endif /* game_hpp */

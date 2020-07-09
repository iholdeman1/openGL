//
//  game.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef game_hpp
#define game_hpp

enum class GameState {
  ACTIVE,
  MENU,
  WIN
};

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
  GameState state_;
  bool keys_[1024];
  unsigned int width_;
  unsigned int height_;
  
};

#endif /* game_hpp */

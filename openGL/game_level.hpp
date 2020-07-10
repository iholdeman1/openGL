//
//  game_level.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef game_level_hpp
#define game_level_hpp

// System Includes
#include <vector>

// Local Includes
#include "game_object.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"

class GameLevel {
public:
  GameLevel();
  
  void load(const char *file, unsigned int level_width, unsigned int level_height);
  void draw(SpriteRenderer& renderer);
  bool is_complete();
  
private:
  void init(std::vector<std::vector<unsigned int>> tile_data,
            unsigned int level_width, unsigned int level_height);
  
  std::vector<GameObject> bricks_;
};

#endif /* game_level_hpp */

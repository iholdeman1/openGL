//
//  ball.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef ball_hpp
#define ball_hpp

// Local Includes
#include "glm/glm.hpp"
#include "game_object.hpp"
#include "texture.hpp"


// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class Ball : public GameObject {
public:
  Ball();
  Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
  
  glm::vec2 move(const float delta_time, const unsigned int window_width);
  void reset(const glm::vec2 position, const glm::vec2 velocity);
  void unstick();
  
  bool is_stuck() const { return stuck_; };
  float get_radius() const { return radius_; };

private:
  float radius_;
  bool stuck_;
};

#endif /* ball_hpp */

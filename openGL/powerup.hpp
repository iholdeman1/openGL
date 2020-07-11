//
//  powerup.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef powerup_hpp
#define powerup_hpp

// System Includes
#include <string>

// Local Includes
#include "glm/glm.hpp"
#include "game_object.hpp"

const glm::vec2 SIZE = glm::vec2(60.0f, 20.0f);
const glm::vec2 VELOCITY = glm::vec2(0.0f, 150.0f);

class PowerUp : public GameObject {
public:
  PowerUp(std::string type, glm::vec3 color, float duration,
          glm::vec2 position, Texture2D texture);
  
  void decrease_duration(const float time);
  void set_activated(const bool flag);
  
  std::string get_type() const { return type_; };
  float get_duration() const { return duration_; };
  bool is_activated() const { return activated_; };
  
private:
  std::string type_;
  float duration_;
  bool activated_;
};

#endif /* powerup_hpp */

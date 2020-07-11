//
//  powerup.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "powerup.hpp"

PowerUp::PowerUp(std::string type, glm::vec3 color, float duration,
                 glm::vec2 position, Texture2D texture)
: GameObject(position, SIZE, texture, color, VELOCITY),
  type_(type), duration_(duration), activated_(false) {
  
}

void PowerUp::decrease_duration(const float time) {
  duration_ -= time;
}

void PowerUp::set_activated(const bool flag) {
  activated_ = flag;
}

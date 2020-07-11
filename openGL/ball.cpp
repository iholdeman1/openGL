//
//  ball.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "ball.hpp"

Ball::Ball() : GameObject(), radius_(12.5f), stuck_(true), sticky_(false), pass_through_(false) {
  
}

Ball::Ball(glm::vec2 position, float radius, glm::vec2 velocity, Texture2D sprite)
  : GameObject(position, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity),
    radius_(radius),
    stuck_(true),
    sticky_(false),
    pass_through_(false) {
  
}

glm::vec2 Ball::move(const float delta_time, const unsigned int window_width) {
  if (!stuck_) {
    // Move the ball
    position_ += velocity_ * delta_time;
    
    // Then check if outside window bounds. If so, reverse velocity and restore at correct position
    if (position_.x <= 0.0f) {
      velocity_.x = -velocity_.x;
      position_.x = 0.0f;
    }
    else if (position_.x + size_.x >= window_width) {
      velocity_.x = -velocity_.x;
      position_.x = window_width - size_.x;
    }
    
    if (position_.y <= 0.0f) {
      velocity_.y = -velocity_.y;
      position_.y = 0.0f;
    }
  }
  
  return position_;
}

void Ball::reverse_velocity(const bool reverse_x, const bool reverse_y) {
  if (reverse_x) {
    velocity_.x = -velocity_.x;
  }
  
  if (reverse_y) {
    velocity_.y = -velocity_.y;
  }
}

void Ball::set_velocity(const float x, const float y) {
  velocity_.x = x;
  velocity_.y = y;
}

void Ball::reset(const glm::vec2 position, const glm::vec2 velocity) {
  position_ = position;
  velocity_ = velocity;
  stuck_ = true;
  sticky_ = false;
  pass_through_ = false;
}

void Ball::set_stuck(const bool flag) {
  stuck_ = flag;
}

void Ball::set_sticky(const bool flag) {
  sticky_ = flag;
}

void Ball::set_pass_through(const bool flag) {
  pass_through_ = flag;
}

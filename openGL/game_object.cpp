//
//  game_object.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "game_object.hpp"

GameObject::GameObject()
  : position_(0.0f, 0.0f), size_(1.0f, 1.0f), velocity_(0.0f), color_(1.0f),
    rotation_(0.0f), sprite_(), is_solid_(false), destroyed_(false) {
      
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
  : position_(pos), size_(size), velocity_(velocity), color_(color), rotation_(0.0f),
    sprite_(sprite), is_solid_(false), destroyed_(false) {
      
}

void GameObject::draw(SpriteRenderer& renderer) {
  renderer.draw_sprite(sprite_, position_, size_, rotation_, color_);
}

void GameObject::set_is_solid(const bool flag) {
	is_solid_ = flag;
}

void GameObject::set_is_destroyed(const bool flag) {
  destroyed_ = flag;
}

void GameObject::move_object(const float x, const float y) {
  position_.x += x;
  position_.y += y;
}

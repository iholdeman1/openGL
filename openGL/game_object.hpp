//
//  game_object.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef game_object_hpp
#define game_object_hpp

// Local Includes
#include "glm/glm.hpp"
#include "sprite_renderer.hpp"
#include "texture.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject {
public:
  GameObject();
  GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
  
  virtual void draw(SpriteRenderer &renderer);
  void set_is_solid(const bool flag);
  void set_is_destroyed(const bool flag);
  void move_object(const float x, const float y);
  void set_position(const float x, const float y);
  void set_size(const glm::vec2 size);

  bool is_destroyed() const { return destroyed_; };
  bool is_solid() const { return is_solid_; };
  glm::vec2 get_position() const { return position_; };
  glm::vec2 get_size() const { return size_; };
  glm::vec2 get_velocity() const { return velocity_; };

protected:
  glm::vec2 position_;
  glm::vec2 size_;
  glm::vec2 velocity_;
  glm::vec3 color_;
  float rotation_;
  bool is_solid_;
  bool destroyed_;
  Texture2D sprite_;
};

#endif /* game_object_hpp */

//
//  sprite_renderer.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef sprite_renderer_hpp
#define sprite_renderer_hpp

// Local Includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.hpp"
#include "texture.hpp"


class SpriteRenderer {
public:
  SpriteRenderer(const Shader& shader);
  ~SpriteRenderer();
  
  void draw_sprite(const Texture2D& texture, glm::vec2 position,
                   glm::vec2 size = glm::vec2(10.0f, 10.0f),
                   float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
private:
  void init_render_data();
  
  Shader shader_;
  unsigned int quad_vao_;
};

#endif /* sprite_renderer_hpp */

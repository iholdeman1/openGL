//
//  sprite_renderer.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "sprite_renderer.hpp"

#include <iostream>

SpriteRenderer::SpriteRenderer(const Shader& shader) {
  shader_ = shader;
  init_render_data();
}

SpriteRenderer::~SpriteRenderer() {
  glDeleteVertexArrays(1, &quad_vao_);
}

void SpriteRenderer::draw_sprite(const Texture2D& texture, glm::vec2 position,
                                 glm::vec2 size, float rotate, glm::vec3 color) {

  shader_.use();

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

  model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
  model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

  model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

  shader_.set_matrix4("model", model);

  // Render textured quad
  shader_.set_vector3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);

  texture.bind();

  glBindVertexArray(quad_vao_);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);

}

void SpriteRenderer::init_render_data() {
  unsigned int VBO;
  float vertices[] = {
    // Position // Texture
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
  };
  
  glGenVertexArrays(1, &quad_vao_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(quad_vao_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

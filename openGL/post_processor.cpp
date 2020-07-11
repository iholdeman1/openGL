//
//  post_processor.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "post_processor.hpp"

// System Includes
#include <iostream>

PostProcessor::PostProcessor(Shader shader, const unsigned int width, const unsigned int height)
: post_processing_shader_(shader), texture_(), width_(width*2), height_(height*2),
  confuse_(false), chaos_(false), shake_(false) {
  
  // Initialize renderbuffer/framebuffer object
  glGenFramebuffers(1, &MSFBO);
  glGenFramebuffers(1, &FBO);
  glGenRenderbuffers(1, &RBO);
  
    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
  glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width_, height_); // allocate storage for render buffer object
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO); // attach MS render buffer object to framebuffer
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO\n";
  }
  
    // Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  texture_.generate(width_, height_, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_.get_id(), 0); // attach texture to framebuffer as its color attachment
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO\n";
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
    // Initialize render data and uniforms
  init_render_data();
  // post_processing_shader_.use();
  post_processing_shader_.set_integer("scene", 0, true);
  
  float offset = 1.0f / 300.0f;
  float offsets[9][2] = {
    { -offset,  offset  },  // top-left
    {  0.0f,    offset  },  // top-center
    {  offset,  offset  },  // top-right
    { -offset,  0.0f    },  // center-left
    {  0.0f,    0.0f    },  // center-center
    {  offset,  0.0f    },  // center - right
    { -offset, -offset  },  // bottom-left
    {  0.0f,   -offset  },  // bottom-center
    {  offset, -offset  }   // bottom-right
  };
  glUniform2fv(glGetUniformLocation(post_processing_shader_.get_id(), "offsets"), 9, (float*)offsets);
  
  int edge_kernel[9] = {
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
  };
  glUniform1iv(glGetUniformLocation(post_processing_shader_.get_id(), "edge_kernel"), 9, edge_kernel);
  
  float blur_kernel[9] = {
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
    2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
    1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
  };
  glUniform1fv(glGetUniformLocation(post_processing_shader_.get_id(), "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::begin_render() {
  glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::end_render() {
  // Now resolve multisampled color-buffer into intermediate FBO to store to texture
  glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::render(const float time) {
  // Set uniforms/options
  post_processing_shader_.use();
  post_processing_shader_.set_float("time", time);
  post_processing_shader_.set_integer("confuse", confuse_);
  post_processing_shader_.set_integer("chaos", chaos_);
  post_processing_shader_.set_integer("shake", shake_);
  
  // Render textured quad
  glActiveTexture(GL_TEXTURE0);
  texture_.bind();
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void PostProcessor::init_render_data() {
  // Configure VAO/VBO
  unsigned int VBO;
  float vertices[] = {
      // Position   // Texture
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 1.0f,

      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f
  };
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void PostProcessor::set_shake(const bool flag) {
  shake_ = flag;
}

void PostProcessor::set_confuse(const bool flag) {
	confuse_ = flag;
}

void PostProcessor::set_chaos(const bool flag) {
	chaos_ = flag;
}

//
//  post_processor.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef post_processor_hpp
#define post_processor_hpp

// Local Includes
#include "glm/glm.hpp"
#include "shader.hpp"
#include "sprite_renderer.hpp"
#include "texture.hpp"


// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or
// Shake boolean.
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor {
public:
  PostProcessor(Shader shader, const unsigned int width, const unsigned int height);
  
  void begin_render();
  void end_render();
  void render(const float time);
  
  void set_shake(const bool flag);

private:
  void init_render_data();
  
  unsigned int MSFBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
  unsigned int FBO;
  unsigned int RBO; // RBO is used for multisampled color buffer
  unsigned int VAO;
  
  Shader post_processing_shader_;
  Texture2D texture_;
  unsigned int width_;
  unsigned int height_;
  bool confuse_;
  bool chaos_;
  bool shake_;
};

#endif /* post_processor_hpp */

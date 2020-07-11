//
//  text_renderer.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/11/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef text_renderer_hpp
#define text_renderer_hpp

// System Includes
#include <map>

// Local Includes
#include "glm/glm.hpp"
#include "resource_manager.hpp"
#include "shader.hpp"
#include "texture.hpp"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int texture_id; // ID handle of the glyph texture
  glm::ivec2 size;      // size of glyph
  glm::ivec2 bearing;   // offset from baseline to left/top of glyph
  long advance;   // horizontal offset to advance to next glyph
};

// A renderer class for rendering text displayed by a font loaded using the
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer {
public:
  TextRenderer(unsigned int width, unsigned int height);
  void load(std::string font, unsigned int font_size);
  void render_text(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

private:
  std::map<char, Character> characters_;

  unsigned int VAO;
  unsigned int VBO;
};

#endif /* text_renderer_hpp */

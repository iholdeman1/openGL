//
//  text_renderer.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/11/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "text_renderer.hpp"

// Library Includes
#include <ft2build.h>
#include FT_FREETYPE_H

// System Includes
#include <iostream>

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {
	// Load and configure shader
  ResourceManager::load_shader("text_2d.vert", "text_2d.frag", nullptr, "text");
  ResourceManager::get_shader("text").use();
  ResourceManager::get_shader("text").set_matrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
  ResourceManager::get_shader("text").set_integer("text", 0);
  
  // Configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TextRenderer::load(std::string font, unsigned int font_size) {
	// First clear the previously loaded characters_
  characters_.clear();
  
  // Then initialize and load the FreeType library
  FT_Library ft;    
  if (FT_Init_FreeType(&ft)) { // All functions return a value different than 0 whenever an error occurred
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library\n";
  }
  
  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font\n";
  }
  
  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, font_size);
  
  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
  
  // Then for the first 128 ASCII characters, pre-load/compile their characters and store them
  for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
  {
    // Load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph\n";
      continue;
    }

    // Generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
	    GL_TEXTURE_2D,
	    0,
	    GL_RED,
	    face->glyph->bitmap.width,
	    face->glyph->bitmap.rows,
	    0,
	    GL_RED,
	    GL_UNSIGNED_BYTE,
	    face->glyph->bitmap.buffer
	  );
    
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
    // Now store character for later use
    Character character = {
      texture,
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      face->glyph->advance.x
    };
    characters_.insert(std::pair<char, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void TextRenderer::render_text(std::string text, float x, float y, float scale, glm::vec3 color) {
  // Activate corresponding render state	
  ResourceManager::get_shader("text").use();
  ResourceManager::get_shader("text").set_vector3f("textColor", color);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // Iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = characters_[*c];

    const float xpos = x + ch.bearing.x * scale;
    const float ypos = y + (characters_['H'].bearing.y - ch.bearing.y) * scale;

    const float w = ch.size.x * scale;
    const float h = ch.size.y * scale;
    
    // Update VBO for each character
    float vertices[6][4] = {
      { xpos,     ypos + h,   0.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 0.0f },
      { xpos,     ypos,       0.0f, 0.0f },

      { xpos,     ypos + h,   0.0f, 1.0f },
      { xpos + w, ypos + h,   1.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 0.0f }
     };

    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.texture_id);
    
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Now advance cursors for next glyph
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
  }

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

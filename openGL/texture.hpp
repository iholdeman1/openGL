//
//  texture.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef texture_hpp
#define texture_hpp

// Library Includes
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

class Texture2D {
public:
  Texture2D();
  
  void generate(const unsigned int width, const unsigned int height, unsigned char *data);
  void bind() const;

  void set_alpha_format();

  unsigned int& get_id_ref() { return id_; };
  
private:
  // Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
  unsigned int id_;
  
  // Texture image dimensions
  unsigned int width_;
  unsigned int height_;
  
  // Texture Format
  unsigned int internal_format_; // Format of texture object
  unsigned int image_format_; // Format of loaded image
  
  // Texture configuration
  unsigned int wrap_s_; // Wrapping mode on S axis
  unsigned int wrap_t_; // Wrapping mode on T axis
  unsigned int filter_min_; // Filtering mode if texture pixels < screen pixels
  unsigned int filter_max_; // Filtering mode if texture pixels > screen pixels
};

#endif /* texture_hpp */

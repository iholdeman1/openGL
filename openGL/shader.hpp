//
//  shader.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef shader_hpp
#define shader_hpp

// Library Includes
#include <OpenGL/gl3.h>

// System Includes
#include <string>

// Local Incudes
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
  Shader();
  
  void use();
  void compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource);
  
  void set_float(const char *name, const float value);
  void set_integer(const char *name, const int value);
  void set_integer(const char *name, const int value, const bool use_shader);
  void set_vector2f(const char *name, const float x, const float y);
  void set_vector2f(const char *name, const glm::vec2 &value);
  void set_vector3f(const char *name, const float x, const float y, const float z);
  void set_vector3f(const char *name, const glm::vec3 &value);
  void set_vector4f(const char *name, const float x, const float y, const float z, const float w);
  void set_vector4f(const char *name, const glm::vec4 &value);
  void set_matrix4(const char *name, const glm::mat4 &matrix);

  unsigned int get_id() const { return id_; };
  
private:
  void check_compile_errors(const unsigned int shader, std::string type);
  
  unsigned int id_;
};

#endif /* shader_hpp */

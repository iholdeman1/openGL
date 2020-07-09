//
//  shader.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "shader.hpp"

// System Includes
#include <iostream>

Shader::Shader() {
  
};

Shader::~Shader() {
  glDeleteProgram(id_);
};

void Shader::use() {
  glUseProgram(id_);
};

void Shader::compile(const char *vertex_source,
                     const char *fragment_source,
                     const char *geometry_source) {
  // Vertex shader
  const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);
  check_compile_errors(vertex_shader, "VERTEX");
  
  // Fragment Shader
  const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);
  check_compile_errors(fragment_shader, "FRAGMENT");
  
  // If geometry shader source code is given, also compile geometry shader
  unsigned int geometry_shader;
  if (geometry_source != nullptr) {
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, &geometry_source, NULL);
    glCompileShader(geometry_shader);
    check_compile_errors(geometry_shader, "GEOMETRY");
  }
  
  // Shader program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex_shader);
  glAttachShader(id_, fragment_shader);
  if (geometry_source != nullptr) {
    glAttachShader(id_, geometry_shader);
  }
  glLinkProgram(id_);
  check_compile_errors(id_, "PROGRAM");
  
  // Delete the shaders as they're linked into our program now and no longer necessery
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  if (geometry_source != nullptr) {
    glDeleteShader(geometry_shader);
  }
};

void Shader::set_integer(const char *name, const int value) {
  glUniform1i(glGetUniformLocation(id_, name), value);
};

void Shader::set_float(const char *name, const float value) {
  glUniform1f(glGetUniformLocation(id_, name), value);
};

void Shader::set_vector2f(const char *name, const float x, const float y) {
  glUniform2f(glGetUniformLocation(id_, name), x, y);
};

void Shader::set_vector2f(const char *name, const glm::vec2 &value) {
  glUniform2f(glGetUniformLocation(id_, name), value.x, value.y);
};

void Shader::set_vector3f(const char *name, const float x, const float y, const float z) {
  glUniform3f(glGetUniformLocation(id_, name), x, y, z);
};

void Shader::set_vector3f(const char *name, const glm::vec3 &value) {
  glUniform3f(glGetUniformLocation(id_, name), value.x, value.y, value.z);
};

void Shader::set_vector4f(const char *name, const float x, const float y, const float z, const float w) {
  glUniform4f(glGetUniformLocation(id_, name), x, y, z, w);
};

void Shader::set_vector4f(const char *name, const glm::vec4 &value) {
  glUniform4f(glGetUniformLocation(id_, name), value.x, value.y, value.z, value.w);
};

void Shader::set_matrix4(const char *name, const glm::mat4 &matrix) {
  glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, false, glm::value_ptr(matrix));
};


void Shader::check_compile_errors(const unsigned int shader, const std::string type) {
  int success;
  char infoLog[1024];
  
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
  else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
}

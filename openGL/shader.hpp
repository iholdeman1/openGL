//
//  shader.hpp
//  openGL
//
//  Created by Ian Holdeman on 6/29/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef shader_h
#define shader_h

// System Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Local Includes
#include "glm/glm.hpp"

class Shader {

public:
  // Ctor
  Shader(const char *vertexPath, const char *fragmentPath) {
    // Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
      // Open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      
      // Read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      
      // Close file handlers
      vShaderFile.close();
      fShaderFile.close();
      
      // Convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile shaders
    
    // Vertex shader
    const unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");
    
    // Fragment Shader
    const unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");
    
    // Shader Program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    check_compile_errors(id_, "PROGRAM");
    
    // Delete the shaders after linking
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  
  // Dtor
  ~Shader() {
    glDeleteProgram(id_);
  }

  void use() {
    glUseProgram(id_);
  }
  
  unsigned int get_id() const {
    return id_;
  }
  
  // TODO: might be useful to make sure these are relevant
  void set_bool(const std::string name, const bool value) {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
  }
  
  void set_int(const std::string name, const int value) {
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
  }
  
  void set_float(const std::string name, const float value) {
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
  }
  
  void set_vec2(const std::string &name, const glm::vec2 &value) const {
      glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }
  
  void set_vec2(const std::string &name, float x, float y) const {
      glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
  }
  
  void set_vec3(const std::string &name, const glm::vec3 &value) const {
      glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }
  
  void set_vec3(const std::string &name, float x, float y, float z) const {
      glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
  }
  
  void set_vec4(const std::string &name, const glm::vec4 &value) const {
      glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
  }
  
  void set_vec4(const std::string &name, float x, float y, float z, float w) const {
      glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
  }
  
  void set_mat2(const std::string &name, const glm::mat2 &mat) const {
      glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }

  void set_mat3(const std::string &name, const glm::mat3 &mat) const {
      glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  
  void set_mat4(const std::string &name, const glm::mat4 &mat) const {
      glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }

private:
  void check_compile_errors(const unsigned int shader, const std::string type) {
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
  
  unsigned int id_;
  
};

#endif /* shader_h */

//
//  resource_manager.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "resource_manager.hpp"

// System Includes
#include <iostream>
#include <sstream>
#include <fstream>

// Local Includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture2D> ResourceManager::textures;

Shader ResourceManager::load_shader(const char *vertex_shader_file, const char *fragment_shader_file, 
																		const char *geometry_shader_file, std::string name) {
  shaders[name] = load_shader_from_file(vertex_shader_file, fragment_shader_file, geometry_shader_file);
  return shaders[name];
}

Shader ResourceManager::get_shader(std::string name) {
  return shaders[name];
}

Texture2D ResourceManager::load_texture(const char *file, bool alpha, std::string name) {
  textures[name] = load_texture_from_file(file, alpha);
  return textures[name];
}

Texture2D ResourceManager::get_texture(std::string name) {
  return textures[name];
}

void ResourceManager::clear() {
  // Delete all shaders
  for (auto iter : shaders) {
    glDeleteProgram(iter.second.get_id());
  }
  
  // Delete all textures
  for (auto iter : textures) {
    glDeleteTextures(1, &iter.second.get_id_ref());
  }
}

Shader ResourceManager::load_shader_from_file(const char *vertex_shader_file, 
																							const char *fragment_shader_file, 
																							const char *geometry_shader_file) {
  // 1. Retrieve the vertex/fragment source code from file path
  std::string vertex_code;
  std::string fragment_code;
  std::string geometry_code;
  try {
    // Open files
    std::ifstream vertexShaderFile(vertex_shader_file);
    std::ifstream fragmentShaderFile(fragment_shader_file);
    std::stringstream vShaderStream, fShaderStream;
    
    // Read file's buffer contents into streams
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();
    
    // Close file handlers
    vertexShaderFile.close();
    fragmentShaderFile.close();
    
    // Convert stream into string
    vertex_code = vShaderStream.str();
    fragment_code = fShaderStream.str();
    
    // If geometry shader path is present, also load a geometry shader
    if (geometry_shader_file != nullptr) {
      std::ifstream geometryShaderFile(geometry_shader_file);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometry_code = gShaderStream.str();
    }
  }
  catch (std::exception e) {
    std::cout << "ERROR::SHADER: Failed to read shader files\n";
  }

  // 2. Now create shader object from source code
  Shader shader;
  shader.compile(vertex_code.c_str(), fragment_code.c_str(), geometry_shader_file != nullptr ? geometry_code.c_str() : nullptr);
  return shader;
}

Texture2D ResourceManager::load_texture_from_file(const char *file, bool alpha) {
  // Create texture object
  Texture2D texture;
  if (alpha) {
    texture.set_alpha_format();
  }
  
  // Load image
  int width, height, nrChannels;
  unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);
  
  // Now generate texture
  texture.generate(width, height, data);
  
  // Free image data
  stbi_image_free(data);

  return texture;
}

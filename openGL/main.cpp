//
//  main.cpp
//  openGL
//
//  Created by Ian Holdeman on 12/7/19.
//  Copyright © 2019 Marvin LLC. All rights reserved.
//

// Library Includes
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

// System Includes
#include <iostream>
#include <math.h>

// Local Includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Globals
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// Kill function
void kill_glfw() {
  glfwTerminate();
}

// Main function
int main(int argc, const char *argv[]) {
  
  // Callback City
  const auto error_callback = [](int error, const char *description)
  {
    std::cerr << "Error: " << description << "\n";
  };
  
  const auto frame_buffer_size_callback = [](GLFWwindow *window, const int width, const int height)
  {
    glViewport(0, 0, width, height);
  };
  
  const auto key_callback = [](GLFWwindow *window,
                               const int key,
                               const int scancode,
                               const int action,
                               const int mods)
  {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  };
  
  // Initialize GLFW
  if (!glfwInit()) {
    std::cout << "GLFW failed to initialize! Quitting program...\n";
    return -1;
  }
  
  // Setup windows
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Window", nullptr, nullptr);
  
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window\n";
    kill_glfw();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  
  // Set Callbacks
  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(window, key_callback);
  
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  
  // Setup shader class
  Shader shader("shader.vert", "shader.frag");
  
  // Array of vertices
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };
  
  glm::vec3 cube_locations[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };
  
//  unsigned int indices[] = {  // note that we start from 0!
//      0, 1, 3,   // first triangle
//      1, 2, 3    // second triangle
//  };
  
  // Setup structures
  GLuint VAO, VBO; //, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // Color attribute
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
//  glEnableVertexAttribArray(1);
  // Texture coords attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Handle textures
  unsigned int texture_one, texture_two;
  
  // First texture
  glGenTextures(1, &texture_one);
  glBindTexture(GL_TEXTURE_2D, texture_one);
  
  // Set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // Load and generate the texture
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true); // Tell stb_image.h to flip loaded texture's on the y-axis
  unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
  
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    std::cerr << "Failed to load texture_one\n";
  }
  
  stbi_image_free(data);
  
  // Second texture
  glGenTextures(1, &texture_two);
  glBindTexture(GL_TEXTURE_2D, texture_two);

  // Set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Load and generate the texture
  data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    std::cerr << "Failed to load texture_two\n";
  }

  stbi_image_free(data);
  
  shader.use(); // Don't forget to activate the shader before setting uniforms!
  glUniform1i(glGetUniformLocation(shader.get_id(), "texture_one"), 0); // set it manually
  glUniform1i(glGetUniformLocation(shader.get_id(), "texture_two"), 1); // set it manually
//  shader.set_int("texture_one", 0); // or with shader class
//  shader.set_int("texture_two", 1); // or with shader class
  
//  uncomment this call to draw in wireframe polygons.
//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_one);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_two);
    
    // Be sure to activate the shader
    shader.use();
    
    // Transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    
    model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) /
                                  static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
    
    // Retrieve the matrix uniform locations
    unsigned int model_location = glGetUniformLocation(shader.get_id(), "model");
    // Pass them to the shaders (3 different ways)
    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
    // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    shader.set_mat4("projection", projection);
    shader.set_mat4("view", view);
    
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    
    for (uint32_t i = 0; i < 10; i++) {
      // Calculate the model matrix for each object and pass it to shader before drawing
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cube_locations[i]);
      model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
      const float angle = 20.0f * i;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      shader.set_mat4("model", model);
      
      glDrawArrays(GL_TRIANGLES, 0, 36); // -- w/o EBO/only want triangle
      // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // with EBO -- want rectangle
    }

    // glBindVertexArray(0); // no need to unbind it every time
    
    glfwSwapBuffers(window);
  }
  
  // Clean up
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  shader.~Shader();
  
  // Kill program
  glfwDestroyWindow(window);
  kill_glfw();
  return 0;
}

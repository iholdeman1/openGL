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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "game.hpp"
#include "resource_manager.hpp"

// Globals
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// Lambda Graveyard
void frame_buffer_size_callback(GLFWwindow *window, const int width, const int height);
void key_callback(GLFWwindow *window, const int key, const int scancode,
                  const int action, const int mods);

Game breakout(WINDOW_WIDTH, WINDOW_HEIGHT);

// Kill function
void kill_glfw() {
  glfwTerminate();
}

// Main function
int main(int argc, const char *argv[]) {
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
  
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Breakout", nullptr, nullptr);
  
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window\n";
    kill_glfw();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  
  // Set Callbacks
  glfwSetKeyCallback(window, key_callback);
  
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

  // Enable stuff
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // Initialize game
  breakout.init();
  
  // Time variables
  float delta_time = 0.0f;
  float last_frame = 0.0f;
  
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    
    // Timing logic
    const float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    
    glfwPollEvents();
    
    breakout.process_input(delta_time);
    breakout.update(delta_time);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    breakout.render();
    
    glfwSwapBuffers(window);
  }
  
  // Kill program
  glfwDestroyWindow(window);
  kill_glfw();
  return 0;
}

// Implementation of dead callbacks
void key_callback(GLFWwindow *window, const int key, const int scancode,
                  const int action, const int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      breakout.set_key(key, true);
    }
    else if (action == GLFW_RELEASE) {
      breakout.set_key(key, false);
    }
  }
}

void frame_buffer_size_callback(GLFWwindow *window, const int width, const int height) {
  glViewport(0, 0, width, height);
}

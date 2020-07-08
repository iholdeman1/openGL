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
#include "camera.hpp"
#include "stb_image.h"
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Globals
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// Timing variables
float delta_time = 0.0f;
float last_frame = 0.0f;

// Camera variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float last_x = 800.0f / 2.0;
float last_y = 600.0 / 2.0;
bool first_mouse = true;

// Lighting Variables
glm::vec3 light_position(1.2f, 1.0f, 2.0f);

// Lambda Graveyard
void key_callback(GLFWwindow *window, const int key, const int scancode,
                  const int action, const int mods);

void mouse_callback(GLFWwindow *window, const double x_position, const double y_position);

void scroll_callback(GLFWwindow *window, const double x_offset, const double y_offset);

// Kill function
void kill_glfw() {
  glfwTerminate();
}

// Main function
int main(int argc, const char *argv[]) {
  
  // Callback City
  const auto error_callback = [](int error, const char *description) {
    std::cerr << "Error: " << description << "\n";
  };
  
  const auto frame_buffer_size_callback = [](GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
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
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  
  // Tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  // Setup shader class
  Shader shader("shader.vert", "shader.frag");
  Shader lighting_shader("light_shader.vert", "light_shader.frag");
  
  // Array of vertices
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);

  GLuint light_vao;
  glGenVertexArrays(1, &light_vao);
  glBindVertexArray(light_vao);

  // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  shader.use(); // Don't forget to activate the shader before setting uniforms!
  
//  uncomment this call to draw in wireframe polygons.
//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    
    // Timing logic
    const float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    
    glfwPollEvents();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Be sure to activate the shader
    shader.use();
    shader.set_vec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.set_vec3("lightPos", light_position);
    shader.set_vec3("viewPos", camera.get_position());
    
    // Transformations
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.get_zoom()), static_cast<float>(WINDOW_WIDTH) /
                                  static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
    
    // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    shader.set_mat4("projection", projection);
    
    // Camera/view transformation
    glm::mat4 view = camera.get_view_matrix();
    shader.set_mat4("view", view);
    
    glm::mat4 model = glm::mat4(1.0f);
    shader.set_mat4("model", model);
    
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Also draw the light object
    lighting_shader.use();
    lighting_shader.set_mat4("projection", projection);
    lighting_shader.set_mat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, light_position);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    lighting_shader.set_mat4("model", model);

    glBindVertexArray(light_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

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

// Implementation of dead callbacks
void key_callback(GLFWwindow *window, const int key, const int scancode,
                  const int action, const int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    camera.process_keyboard(FORWARD, delta_time);
  }
  else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    camera.process_keyboard(LEFT, delta_time);
  }
  else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    camera.process_keyboard(BACKWARD, delta_time);
  }
  else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
    camera.process_keyboard(RIGHT, delta_time);
  }

}

void mouse_callback(GLFWwindow *window, const double x_position, const double y_position) {
  if (first_mouse)
  {
    last_x = x_position;
    last_y = y_position;
    first_mouse = false;
  }

  float x_offset = x_position - last_x;
  float y_offset = last_y - y_position;
  
  last_x = x_position;
  last_y = y_position;

  camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, const double x_offset, const double y_offset) {
  camera.process_mouse_scroll(y_offset);
}

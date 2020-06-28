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

// Globals
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// Vertex Shader
const char *vertex_shader_code = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Fragment Shader
const char *fragment_shader_code = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

// Kill function
void kill_glfw() {
  glfwTerminate();
}

// Check shader compilation
bool check_shader_compilation(const GLuint shader, const std::string shader_type) {
  int success;
  char log[512];
  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cerr << "Error: " << shader_type << " shader failed to compile!\n" << log << "\n";
    return false;
  }
  
  return true;
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
  
  // Set the vertex shader
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
  glCompileShader(vertex_shader);
  if (!check_shader_compilation(vertex_shader, "vertex")) {
    return -1;
  }
  
  // Set the fragment shader
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
  glCompileShader(fragment_shader);
  if (!check_shader_compilation(fragment_shader, "fragment")) {
    return -1;
  }
  
  // Set the shader program
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  
  int success;
  char log[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, nullptr, log);
    std::cerr << "Error: shader program failed to link!\n" << log << "\n";
    return -1;
  }
  
  // Delete shaders now that they're linked
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  // Array of vertices
  float vertices[] = {
       0.5f,  0.5f, 0.0f,  // top right
       0.5f, -0.5f, 0.0f,  // bottom right
      -0.5f, -0.5f, 0.0f,  // bottom left
      -0.5f,  0.5f, 0.0f   // top left
  };
  
  unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,   // first triangle
      1, 2, 3    // second triangle
  };
  
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  
  // uncomment this call to draw in wireframe polygons.
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    
    glClearColor(0.7f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shader_program);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // glDrawArrays(GL_TRIANGLES, 0, 3); -- w/o EBO/only want triangle
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // with EBO -- want rectangle
    // glBindVertexArray(0); // no need to unbind it every time
    
    glfwSwapBuffers(window);
  }
  
  // Clean up
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader_program);
  
  // Kill program
  glfwDestroyWindow(window);
  kill_glfw();
  return 0;
}

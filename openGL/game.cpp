//
//  game.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "game.hpp"

#include <iostream>

Game::Game(const unsigned int width, const unsigned int height)
: state_(GameState::MENU), width_(width), height_(height) {

}

Game::~Game() {
  delete renderer_;
}

void Game::init() {
  
  ResourceManager::load_shader("shader.vert", "shader.frag", nullptr, "sprite");
  
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
                                    static_cast<float>(height_), 0.0f, -1.0f, 1.0f);
  
  ResourceManager::get_shader("sprite").use();
  
  ResourceManager::get_shader("sprite").set_integer("image", 0);
  
  ResourceManager::get_shader("sprite").set_matrix4("projection", projection);
  
  renderer_ = new SpriteRenderer(ResourceManager::get_shader("sprite"));
  
  ResourceManager::load_texture("awesomeface.png", true, "face");
}

void Game::process_input(const float delta_time) {

}

void Game::update(const float delta_time) {
  
}

void Game::render() {
  renderer_->draw_sprite(ResourceManager::get_texture("face"), glm::vec2(200.0f, 200.0f),
                         glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::set_key(const unsigned int key, const bool flag) {
  keys_[key] = flag;
}

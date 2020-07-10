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
: state_(GameState::ACTIVE), width_(width), height_(height) {

}

Game::~Game() {
  delete renderer_;
  delete player_;
}

void Game::init() {
  
  // Set up the shader
  ResourceManager::load_shader("shader.vert", "shader.frag", nullptr, "sprite");
  
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
                                    static_cast<float>(height_), 0.0f, -1.0f, 1.0f);
  
  ResourceManager::get_shader("sprite").use();
  ResourceManager::get_shader("sprite").set_integer("image", 0);
  ResourceManager::get_shader("sprite").set_matrix4("projection", projection);
  
  // Create the renderer
  renderer_ = new SpriteRenderer(ResourceManager::get_shader("sprite"));
  
  // Load textures
  ResourceManager::load_texture("background.png", false, "background");
  ResourceManager::load_texture("awesomeface.png", true, "face");
  ResourceManager::load_texture("block.png", false, "block");
  ResourceManager::load_texture("block_solid.png", false, "block_solid");
  ResourceManager::load_texture("paddle.png", true, "paddle");

  // Load levels
  GameLevel one;
  GameLevel two;
  GameLevel three;
  GameLevel four;

  one.load("one.lvl", width_, height_ / 2);
  two.load("two.lvl", width_, height_ / 2);
  three.load("three.lvl", width_, height_ / 2);
  four.load("four.lvl", width_, height_ / 2);

  levels_.push_back(one);
  levels_.push_back(two);
  levels_.push_back(three);
  levels_.push_back(four);

  level = 0;
  
  // Set up player
  glm::vec2 player_position = glm::vec2(width_ / 2.0f - PLAYER_SIZE.x / 2.0f, height_ - PLAYER_SIZE.y);
  player_ = new GameObject(player_position, PLAYER_SIZE, ResourceManager::get_texture("paddle"));
}

void Game::process_input(const float delta_time) {
  if (state_ == GameState::ACTIVE) {
    const float velocity = PLAYER_VELOCITY * delta_time;
    const glm::vec2 current_player_position = player_->get_position();
    const glm::vec2 current_player_size = player_->get_size();

    if (keys_[GLFW_KEY_A] && current_player_position.x >= 0.0f) {
      player_->move_object(-velocity, 0.0f);
    }
    if (keys_[GLFW_KEY_D] && current_player_position.x <= width_ - current_player_size.x) {
      player_->move_object(velocity, 0.0f);
    }
  }
}

void Game::update(const float delta_time) {
  
}

void Game::render() {
	if (state_ == GameState::ACTIVE) {
		renderer_->draw_sprite(ResourceManager::get_texture("background"), glm::vec2(0.0f, 0.0f),
                           glm::vec2(width_, height_), 0.0f);

		levels_[level].draw(*renderer_);
    player_->draw(*renderer_);
	}
}

void Game::set_key(const unsigned int key, const bool flag) {
  keys_[key] = flag;
}

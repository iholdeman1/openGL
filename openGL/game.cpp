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

  level_ = 0;
  
  // Set up the player
  glm::vec2 player_position = glm::vec2(width_ / 2.0f - PLAYER_SIZE.x / 2.0f, height_ - PLAYER_SIZE.y);
  player_ = new GameObject(player_position, PLAYER_SIZE, ResourceManager::get_texture("paddle"));
  
  // Set up the ball
  glm::vec2 ball_position = player_position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
  ball_ = new Ball(ball_position, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::get_texture("face"));
}

void Game::process_input(const float delta_time) {
  if (state_ == GameState::ACTIVE) {
    const float velocity = PLAYER_VELOCITY * delta_time;
    const glm::vec2 current_player_position = player_->get_position();
    const glm::vec2 current_player_size = player_->get_size();

    if (keys_[GLFW_KEY_A] && current_player_position.x >= 0.0f) {
      player_->move_object(-velocity, 0.0f);
      if (ball_->is_stuck()) {
        ball_->move_object(-velocity, 0.0);
      }
    }
    if (keys_[GLFW_KEY_D] && current_player_position.x <= width_ - current_player_size.x) {
      player_->move_object(velocity, 0.0f);
      if (ball_->is_stuck()) {
        ball_->move_object(velocity, 0.0);
      }
    }
    if (keys_[GLFW_KEY_SPACE]) {
      ball_->unstick();
    }
  }
}

void Game::update(const float delta_time) {
  ball_->move(delta_time, width_);
  calculate_collisions();
}

void Game::render() {
	if (state_ == GameState::ACTIVE) {
		renderer_->draw_sprite(ResourceManager::get_texture("background"), glm::vec2(0.0f, 0.0f),
                           glm::vec2(width_, height_), 0.0f);

		levels_[level_].draw(*renderer_);
    player_->draw(*renderer_);
    ball_->draw(*renderer_);
	}
}

void Game::set_key(const unsigned int key, const bool flag) {
  keys_[key] = flag;
}

void Game::calculate_collisions() {
  for (GameObject& rect : levels_[level_].get_bricks()) {
    if (!rect.is_destroyed() && check_collision(*ball_, rect) && !rect.is_solid()) {
      rect.set_is_destroyed(true);
    }
  }
}

bool Game::check_collision(const Ball& ball, const GameObject& rect) {
  // Get member vars for each object
  const float ball_radius = ball.get_radius();
  const glm::vec2 ball_position = ball.get_position();
  const glm::vec2 rect_position = rect.get_position();
  const glm::vec2 rect_size = rect.get_size();
  
  // Get ball's center
  glm::vec2 center(ball_position + ball_radius);
  
  // Calculate AABB info (center, half-extents)
  glm::vec2 aabb_half_extents(rect_size.x / 2.0f, rect_size.y / 2.0f);
  glm::vec2 aabb_center(
    rect_position.x + aabb_half_extents.x,
    rect_position.y + aabb_half_extents.y
  );
  
  // Get difference vector between both centers
  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  
  // Add clamped value to AABB_center and we get the value of box closest to circle
  glm::vec2 closest = aabb_center + clamped;
  
  // Retrieve vector between center circle and closest point AABB and check if length <= radius
  difference = closest - center;
  
  return glm::length(difference) < ball_radius;
}

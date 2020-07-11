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
  delete ball_;
  delete effects_;
}

void Game::init() {
  
  // Set up the shader
  ResourceManager::load_shader("shader.vert", "shader.frag", nullptr, "sprite");
  ResourceManager::load_shader("post_processing.vert", "post_processing.frag", nullptr, "postprocessing");
  
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width_),
                                    static_cast<float>(height_), 0.0f, -1.0f, 1.0f);
  
  ResourceManager::get_shader("sprite").use();
  ResourceManager::get_shader("sprite").set_integer("image", 0);
  ResourceManager::get_shader("sprite").set_matrix4("projection", projection);
  
  // Create the renderers
  renderer_ = new SpriteRenderer(ResourceManager::get_shader("sprite"));
  effects_ = new PostProcessor(ResourceManager::get_shader("postprocessing"), width_, height_);
  
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
  // Move ball
  ball_->move(delta_time, width_);
  
  // Collisions
  calculate_collisions();
  
  // Reduce shake time
  if (shake_time_ > 0.0f) {
    shake_time_ -= delta_time;
    if (shake_time_ <= 0.0f) {
      effects_->set_shake(false);
    }
  }
  
  // Check loss condition
  if (ball_->get_position().y >= height_) {
    reset_level();
    reset_player();
  }
}

void Game::render() {
	if (state_ == GameState::ACTIVE) {
    // Begin rendering to postprocessing framebuffer
    effects_->begin_render();
    
    renderer_->draw_sprite(ResourceManager::get_texture("background"), glm::vec2(0.0f, 0.0f),
                           glm::vec2(width_, height_), 0.0f);

    levels_[level_].draw(*renderer_);
    player_->draw(*renderer_);
    ball_->draw(*renderer_);
    
    // End rendering to postprocessing framebuffer
    effects_->end_render();
    
    // Render postprocessing quad
    effects_->render(glfwGetTime());
	}
}

void Game::set_key(const unsigned int key, const bool flag) {
  keys_[key] = flag;
}

void Game::calculate_collisions() {
  for (GameObject& rect : levels_[level_].get_bricks()) {
    if (!rect.is_destroyed()) {
      Collision collision = check_collision(*ball_, rect);
      if (std::get<0>(collision)) {
        if (!rect.is_solid()) {
          rect.set_is_destroyed(true);
        }
        else {
          shake_time_ = 0.05f;
          effects_->set_shake(true);
        }
          
        // Collision resolution
        Direction direction = std::get<1>(collision);
        glm::vec2 result_vector = std::get<2>(collision);
        
        if (direction == Direction::LEFT || direction == Direction::RIGHT) {
          ball_->reverse_velocity(true, false);
          
          const float overlap = ball_->get_radius() - std::abs(result_vector.x);
          if (direction == Direction::LEFT) {
            ball_->move_object(overlap, 0.0f);
          }
          else {
            ball_->move_object(-overlap, 0.0f);
          }
        }
        else { // (direction == Direction::UP || direction == Direction::DOWN)
          ball_->reverse_velocity(false, true);
          
          const float overlap = ball_->get_radius() - std::abs(result_vector.y);
          if (direction == Direction::UP) {
            ball_->move_object(0.0f, -overlap);
          }
          else {
            ball_->move_object(0.0f, overlap);
          }
        }
      }
    }
  }
  
  // Check collision with player
  Collision collision = check_collision(*ball_, *player_);
  
  if (!ball_->is_stuck() && std::get<0>(collision)) {
    const float half_player_size = player_->get_size().x / 2.0f;
    const float player_center = player_->get_position().x + half_player_size;
    const float distance = ball_->get_position().x + ball_->get_radius() - player_center;
    const float percentage = distance / half_player_size;
    const float strength = 2.0f;
    
    const glm::vec2 previous_velocity = ball_->get_velocity();
    const glm::vec2 temp_velocity = glm::vec2(INITIAL_BALL_VELOCITY.x * percentage * strength, previous_velocity.y);
    const glm::vec2 new_velocity = glm::normalize(temp_velocity) * glm::length(previous_velocity);
    ball_->set_velocity(new_velocity.x, -1.0 * std::abs(new_velocity.y));
  }
}

Collision Game::check_collision(const Ball& ball, const GameObject& rect) {
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
  
  if (glm::length(difference) < ball_radius) {
    return std::make_tuple(true, determine_vector_direction(difference), difference);
  }
  else {
    return std::make_tuple(false, Direction::UP, glm::vec2(0.0f, 0.0f));
  }
}

Direction Game::determine_vector_direction(glm::vec2 vector) {
  glm::vec2 compass[] = {
    glm::vec2(0.0f, 1.0f), // up
    glm::vec2(0.0f, -1.0f), // down
    glm::vec2(-1.0f, 0.0f), // left
    glm::vec2(1.0f, 0.0f) // right
  };
  
  float maximum = 0.0f;
  unsigned int best_guess = -1;
  
  for (unsigned int i = 0; i < 4; i++) {
    const float dot_product = glm::dot(glm::normalize(vector), compass[i]);
    if (dot_product > maximum) {
      maximum = dot_product;
      best_guess = i;
    }
  }
  return static_cast<Direction>(best_guess);
}

void Game::reset_level() {
  switch (level_) {
    case 0:
      levels_[level_].load("one.lvl", width_, height_ / 2);
      break;
    case 1:
      levels_[level_].load("two.lvl", width_, height_ / 2);
      break;
    case 2:
      levels_[level_].load("three.lvl", width_, height_ / 2);
      break;
    case 3:
      levels_[level_].load("four.lvl", width_, height_ / 2);
      break;
    default:
      break;
  }
}

void Game::reset_player() {
  player_->set_size(PLAYER_SIZE);
  
  const glm::vec2 new_position = glm::vec2(width_ / 2.0f - PLAYER_SIZE.x / 2.0f, height_ - PLAYER_SIZE.y);
  player_->set_position(new_position.x, new_position.y);
  
  const float ball_radius = ball_->get_radius();
  ball_->reset(new_position + glm::vec2(PLAYER_SIZE.x / 2.0f - ball_radius, -(ball_radius * 2.0f)),
               INITIAL_BALL_VELOCITY);
}

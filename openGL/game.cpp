//
//  game.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "game.hpp"

// System Includes
#include <iostream>

Game::Game(const unsigned int width, const unsigned int height)
: state_(GameState::ACTIVE), width_(width), height_(height) {

}

Game::~Game() {
  delete renderer_;
  delete player_;
  delete ball_;
  delete effects_;
  sound_engine_->drop();
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
  ResourceManager::load_texture("powerup_speed.png", true, "powerup_speed");
  ResourceManager::load_texture("powerup_sticky.png", true, "powerup_sticky");
  ResourceManager::load_texture("powerup_increase.png", true, "powerup_increase");
  ResourceManager::load_texture("powerup_confuse.png", true, "powerup_confuse");
  ResourceManager::load_texture("powerup_chaos.png", true, "powerup_chaos");
  ResourceManager::load_texture("powerup_passthrough.png", true, "powerup_passthrough");

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
  
  // Set up the sound engine
  sound_engine_ = irrklang::createIrrKlangDevice();
  sound_engine_->play2D("breakout.mp3", true);
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
      ball_->set_stuck(false);
    }
  }
}

void Game::update(const float delta_time) {
  // Move ball
  ball_->move(delta_time, width_);
  
  // Collisions
  calculate_collisions();

  // Update powerups
  update_powerups(delta_time);
  
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

    for (PowerUp& powerup : powerups_) {
    	if (!powerup.is_destroyed()) {
    		powerup.draw(*renderer_);
    	}
    }

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
          spawn_powerups(rect);
          sound_engine_->play2D("bleep.mp3", false);
        }
        else {
          shake_time_ = 0.05f;
          effects_->set_shake(true);
          sound_engine_->play2D("solid.wav", false);
        }
          
        // Collision resolution
        Direction direction = std::get<1>(collision);
        glm::vec2 result_vector = std::get<2>(collision);
        
        if (!(ball_->can_pass_through() && !rect.is_solid())) { // Don't do collision resolution on non-solid bricks if pass-through is activated
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
  }

  // Check collision on powerups
  for (PowerUp& powerup : powerups_) {
  	if (!powerup.is_destroyed()) {
  		// First check if powerup passed bottom edge, if so: keep as inactive and destroy
      if (powerup.get_position().y >= height_) {
        powerup.set_is_destroyed(true);
      }

      if (check_collision(*player_, powerup)) {	// Collided with player, now activate powerup
        activate_powerup(powerup);
        powerup.set_is_destroyed(true);
        powerup.set_activated(true);
        sound_engine_->play2D("powerup.wav", false);
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

    // If Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
    ball_->set_stuck(ball_->is_sticky());
    
    sound_engine_->play2D("bleep.wav", false);
  }
}

bool Game::check_collision(const GameObject& one, const GameObject& two) { // AABB - AABB collision
  // Get positions and sizes
	const glm::vec2 position_one = one.get_position();
	const glm::vec2 size_one = one.get_size();
	const glm::vec2 position_two = two.get_position();
	const glm::vec2 size_two = two.get_size();

  // Collision x-axis
  const bool collision_x = position_one.x + size_one.x >= position_two.x &&
      										 position_two.x + size_two.x >= position_one.x;
  
  // Collision y-axis
  const bool collision_y = position_one.y + size_one.y >= position_two.y &&
      										 position_two.y + size_two.y >= position_one.y;
  
  // Collision only if on both axes
  return collision_x && collision_y;
}

Collision Game::check_collision(const Ball& ball, const GameObject& rect) { // AABB - Circle collision
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
  
  effects_->set_chaos(false);
  effects_->set_confuse(false);
  
  ball_->set_sticky(false);
  ball_->set_pass_through(false);
  
  player_->set_color(glm::vec3(1.0f));
  ball_->set_color(glm::vec3(1.0f));
}

bool Game::should_spawn_powerup(const unsigned int chance) {
  unsigned int random = rand() % chance;
  return random == 0;
}

void Game::spawn_powerups(GameObject& rect) {
  if (should_spawn_powerup(75)) { // 1 in 75 chance
      powerups_.push_back(
          PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, rect.get_position(), ResourceManager::get_texture("powerup_speed")));
  }
  if (should_spawn_powerup(75)) {
      powerups_.push_back(
          PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, rect.get_position(), ResourceManager::get_texture("powerup_sticky")));
  }
  if (should_spawn_powerup(75)) {
      powerups_.push_back(
          PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, rect.get_position(), ResourceManager::get_texture("powerup_passthrough")));
  }
  if (should_spawn_powerup(75)) {
      powerups_.push_back(
          PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, rect.get_position(), ResourceManager::get_texture("powerup_increase")));
  }
  if (should_spawn_powerup(15)) { // Negative powerups should spawn more often
      powerups_.push_back(
          PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, rect.get_position(), ResourceManager::get_texture("powerup_confuse")));
  }
  if (should_spawn_powerup(15)) {
      powerups_.push_back(
          PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, rect.get_position(), ResourceManager::get_texture("powerup_chaos")));
  }
}

void Game::update_powerups(const float delta_time) {
  for (PowerUp& powerup : powerups_) {
    if (!powerup.is_destroyed()) {
      const glm::vec2 new_distance = powerup.get_velocity() * delta_time;
      powerup.move_object(new_distance.x, new_distance.y);
    }
    
    if (powerup.is_activated()) {
      powerup.decrease_duration(delta_time);
      
      if (powerup.get_duration() <= 0.0f) {
        // De-activate
        powerup.set_activated(false);
        
        // Remove effects
        if (powerup.get_type() == "sticky") {
          if (!is_other_powerup_active("sticky")) { // Only reset if no other PowerUp of type sticky is active
            ball_->set_sticky(false);
            player_->set_color(glm::vec3(1.0f));
          }
        }
        else if (powerup.get_type() == "pass-through") {
          if (!is_other_powerup_active("pass-through")) { // Only reset if no other PowerUp of type pass-through is active
            ball_->set_pass_through(false);
            ball_->set_color(glm::vec3(1.0f));
          }
        }
        else if (powerup.get_type() == "confuse") {
          if (!is_other_powerup_active("confuse")) {  // Only reset if no other PowerUp of type confuse is active
            effects_->set_confuse(false);
          }
        }
        else if (powerup.get_type() == "chaos") {
          if (!is_other_powerup_active("chaos")) { // Only reset if no other PowerUp of type chaos is active
            effects_->set_chaos(false);
          }
        }
      }
    }
  }
  
  // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
  // Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
//  powerups_.erase(std::remove_if(powerups_.begin(), powerups_.end(),
//      [](const PowerUp& powerup) { return powerup.is_destroyed() && !powerup.is_activated(); }
//  ), powerups_.end());
}

void Game::activate_powerup(PowerUp& powerup) {
  if (powerup.get_type() == "speed") {
  	const glm::vec2 new_velocity = ball_->get_velocity() * 1.2f;
    ball_->set_velocity(new_velocity.x, new_velocity.y);
  }
  else if (powerup.get_type() == "sticky") {
    ball_->set_sticky(true);
    player_->set_color(glm::vec3(1.0f, 0.5f, 1.0f));
  }
  else if (powerup.get_type() == "pass-through") {
    ball_->set_pass_through(true);
    ball_->set_color(glm::vec3(1.0f, 0.5f, 0.5f));
  }
  else if (powerup.get_type() == "pad-size-increase") {
    glm::vec2 old_size = player_->get_size();
    glm::vec2 new_size = glm::vec2(old_size.x + 50, old_size.y);
    player_->set_size(new_size);
  }
  else if (powerup.get_type() == "confuse") {
    if (!effects_->is_chaotic()) {
      effects_->set_confuse(true); // Only activate if chaos wasn't already active
    }
  }
  else if (powerup.get_type() == "chaos") {
    if (!effects_->is_confused()) {
      effects_->set_chaos(true);
    }
  }
}

bool Game::is_other_powerup_active(std::string type) {
  // Check if another PowerUp of the same type is still active
  // in which case we don't disable its effect (yet)
  for (const PowerUp& powerup : powerups_) {
    if (powerup.is_activated() && powerup.get_type() == type) {
      return true;
    }
  }
  return false;
}

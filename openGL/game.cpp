//
//  game.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/9/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "game.hpp"

Game::Game(const unsigned int width, const unsigned int height)
: state_(GameState::MENU), width_(width), height_(height) {
  
}

Game::~Game() {
  
}

void Game::init() {
  
}

void Game::process_input(const float delta_time) {

}

void Game::update(const float delta_time) {
  
}

void Game::render() {
  
}

void Game::set_key(const unsigned int key, const bool flag) {
  keys_[key] = flag;
}

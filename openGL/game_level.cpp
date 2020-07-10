//
//  game_level.cpp
//  openGL
//
//  Created by Ian Holdeman on 7/10/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#include "game_level.hpp"

// System Includes
#include <fstream>
#include <sstream>

GameLevel::GameLevel() {
  
}

void GameLevel::load(const char *file, unsigned int level_width, unsigned int level_height) {
  // Clear old data
  bricks_.clear();
  
  // Load from file
  unsigned int tile_code;
  std::string line;
  std::ifstream fstream(file);
  std::vector<std::vector<unsigned int>> tile_data;
  if (fstream) {
    while (std::getline(fstream, line)) { // Read each line from level file
      std::istringstream sstream(line);
      std::vector<unsigned int> row;
      while (sstream >> tile_code) { // Read each word separated by spaces
        row.push_back(tile_code);
      }
      tile_data.push_back(row);
    }
    if (tile_data.size() > 0) {
      init(tile_data, level_width, level_height);
    }
  }
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tile_data,
                unsigned int level_width, unsigned int level_height) {
  // Calculate dimensions
  unsigned int height = tile_data.size();
  unsigned int width  = tile_data[0].size();
  const float unit_width = static_cast<float>(level_width / width);
  const float unit_height = static_cast<float>(level_height / height);
  // initialize level tiles based on tile_data
  for (unsigned int y = 0; y < height; y++) {
    for (unsigned int x = 0; x < width; x++) {
      // Check block type from level data (2D level array)
      if (tile_data[y][x] == 1) { // Solid
        glm::vec2 pos(unit_width * x, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        GameObject obj(pos, size,
                       ResourceManager::get_texture("block_solid"),
                       glm::vec3(0.8f, 0.8f, 0.7f));
        obj.set_is_solid(true);
        bricks_.push_back(obj);
      }
      else if (tile_data[y][x] > 1) {
        glm::vec3 color = glm::vec3(1.0f); // original: white
        switch (tile_data[y][x]) {
          case 2:
            color = glm::vec3(0.2f, 0.6f, 1.0f);
            break;
          case 3:
            color = glm::vec3(0.0f, 0.7f, 0.0f);
            break;
          case 4:
            color = glm::vec3(0.8f, 0.8f, 0.4f);
            break;
          case 5:
            color = glm::vec3(1.0f, 0.5f, 0.0f);
            break;
          default:
            break;
        }

        glm::vec2 pos(unit_width * x, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        bricks_.push_back(GameObject(pos, size, ResourceManager::get_texture("block"), color));
      }
    }
  }
}

void GameLevel::draw(SpriteRenderer& renderer) {
  for (GameObject& tile : bricks_) {
    if (!tile.is_destroyed()) {
      tile.draw(renderer);
    }
  }
}

bool GameLevel::is_complete() {
  for (const GameObject& tile : bricks_) {
    if (!tile.is_solid() && !tile.is_destroyed()) {
      return false;
    }
  }
  return true;
}

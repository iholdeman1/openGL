//
//  camera.hpp
//  openGL
//
//  Created by Ian Holdeman on 7/5/20.
//  Copyright © 2020 Marvin LLC. All rights reserved.
//

#ifndef camera_h
#define camera_h

// System includes
#include <vector>

// Local includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH =  0.0f;
const float SPEED =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {

public:
  // Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
   float yaw = YAW, float pitch = PITCH)
  : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
    movement_speed_(SPEED),
    mouse_sensitivity_(SENSITIVITY),
    zoom_(ZOOM) {
      position_ = position;
      world_up_ = up;
      yaw_ = yaw;
      pitch_ = pitch;
      update_camera_vectors();
  }

  // Constructor with scalar values
  Camera(const float pos_x, const float pos_y, const float pos_z, const float up_x, const float up_y,
   const float up_z, const float yaw, const float pitch)
  : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
    movement_speed_(SPEED),
    mouse_sensitivity_(SENSITIVITY),
    zoom_(ZOOM) {
      position_ = glm::vec3(pos_x, pos_y, pos_z);
      world_up_ = glm::vec3(up_x, up_y, up_z);
      yaw_ = yaw;
      pitch_ = pitch;
      update_camera_vectors();
  }

  // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
  glm::mat4 get_view_matrix() {
    return glm::lookAt(position_, position_ + front_, up_);
  }

  float get_zoom() const {
    return zoom_;
  }
  
  glm::vec3 get_position() {
    return position_;
  }

  // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void process_keyboard(Camera_Movement direction, const float delta_time) {
    const float velocity = movement_speed_ * delta_time;
    
    if (direction == FORWARD) {
      position_ += front_ * velocity;
    }
    else if (direction == BACKWARD) {
      position_ -= front_ * velocity;
    }
    else if (direction == LEFT) {
      position_ -= right_ * velocity;
    }
    else if (direction == RIGHT) {
      position_ += right_ * velocity;
    }
  }

  // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true) {
    x_offset *= mouse_sensitivity_;
    y_offset *= mouse_sensitivity_;

    yaw_ += x_offset;
    pitch_ += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
      if (pitch_ > 89.0f) {
        pitch_ = 89.0f;
      }
      else if (pitch_ < -89.0f) {
        pitch_ = -89.0f;
      }
    }

    // Update front, right and up vectors using the updated euler angles
    update_camera_vectors();
  }

  // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void process_mouse_scroll(float y_offset)
  {
    zoom_ -= y_offset;

    if (zoom_ < 1.0f) {
      zoom_ = 1.0f;
    }
    else if (zoom_ > 45.0f) {
      zoom_ = 45.0f;
    }
  }

private:
  // Calculates the new front vector from the Camera's (updated) Euler Angles
  void update_camera_vectors() {
    // Calculate the new front vector
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    new_front.y = sin(glm::radians(pitch_));
    new_front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(new_front);

    // Re-calculate the right and up vector
    right_ = glm::normalize(glm::cross(front_, world_up_));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up_ = glm::normalize(glm::cross(right_, front_));
  }

  // Camera attributes
  glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 front_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 right_ = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 world_up_ = glm::vec3(0.0f, 0.0f, 0.0f);

  // Euler angles
  float yaw_ = 0.0f;
  float pitch_ = 0.0f;

  // Camera options
  float movement_speed_ = 0.0f;
  float mouse_sensitivity_ = 0.0f;
  float zoom_ = 0.0f;
};

#endif /* camera_h */

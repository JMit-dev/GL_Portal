#include "app/Camera.h"
#include "app/Input.h"

void Camera::update(float /*dt*/) {
  // --- VERY BASIC ---  (camera remains static for now)
  // Later: read Input::state() for WASD / mouse look and update
  // `position`, `yaw`, `pitch`.

  // view matrix
  glm::vec3 front{cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                  sin(glm::radians(pitch)),
                  sin(glm::radians(yaw)) * cos(glm::radians(pitch))};
  viewMat = glm::lookAt(position, position + glm::normalize(front),
                        glm::vec3(0.f, 1.f, 0.f));

  // projection
  projMat = glm::perspective(fov, aspect, zNear, zFar);
}

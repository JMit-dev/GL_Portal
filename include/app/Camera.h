#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(float fovDeg, float aspect, float near, float far)
      : fov(glm::radians(fovDeg)), aspect(aspect), zNear(near), zFar(far) {}

  void setAspect(float a) { aspect = a; }
  void update(float dt);

  const glm::mat4 &view() const { return viewMat; }
  const glm::mat4 &proj() const { return projMat; }
  const glm::vec3 &pos() const { return position; }

private:
  glm::vec3 position{0.f, 0.f, 3.f};
  float yaw{-90.f};
  float pitch{0.f};

  float fov, aspect, zNear, zFar;
  glm::mat4 viewMat{1.f}, projMat{1.f};
};

#endif

/// STOP. You should not modify this file unless you KNOW what you are doing.

#ifndef GLSHAPE_H
#define GLSHAPE_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/Renderable.h"

class Shader;

/// Generic Shape object that manages the OpenGL context for a shape.
/// All shapes directly interacting with the OpenGL context
/// should public-inherit this class.
class GLShape {
public:
  GLShape() = delete;
  GLShape(const GLShape &) = delete;
  GLShape &operator=(const GLShape &) = delete;

  virtual ~GLShape() noexcept = 0;

protected:
  explicit GLShape(Shader *shader);

  GLShape(GLShape &&) noexcept;
  GLShape &operator=(GLShape &&) noexcept;

  Shader *pShader{nullptr};

  GLuint vao{0U};
  GLuint vbo{0U};

public:
  Shader *shader() const // read‑only accessor
  {
    return pShader;
  }
};

#endif // GLSHAPE_H

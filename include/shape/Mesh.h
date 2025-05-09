#ifndef SHAPE_MESH_H
#define SHAPE_MESH_H

#include "shape/GLShape.h"
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 tex;
};

/// A single draw‑call chunk.  Manages its own VAO/VBO via GLShape.
class Mesh : public GLShape, public Renderable {
public:
  Mesh(Shader *shader, std::vector<Vertex> vertices,
       std::vector<unsigned> indices);

  void render() override;

private:
  std::vector<Vertex> verts;
  std::vector<unsigned> idx;
};
#endif

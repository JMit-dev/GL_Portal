#include "shape/Mesh.h"
#include "util/Shader.h"

Mesh::Mesh(Shader *sh, std::vector<Vertex> v, std::vector<unsigned> i)
    : GLShape(sh), verts(std::move(v)), idx(std::move(i)) {
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(),
               GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned),
               idx.data(), GL_STATIC_DRAW);

  // layout
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Mesh::render() {
  pShader->use();
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, idx.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

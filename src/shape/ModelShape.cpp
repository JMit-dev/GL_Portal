#include "shape/ModelShape.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <stdexcept>

ModelShape::ModelShape(Shader *sh, const std::string &path, const glm::mat4 &m)
    : modelMat(m), shader(sh) {
  Assimp::Importer imp;
  const aiScene *sc =
      imp.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                             aiProcess_CalcTangentSpace);
  if (!sc || sc->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !sc->mRootNode)
    throw std::runtime_error("Assimp: " + std::string(imp.GetErrorString()));

  loadNode(sc->mRootNode, sc, sh);
}

void ModelShape::loadNode(const aiNode *node, const aiScene *scene,
                          Shader *sh) {
  for (unsigned i = 0; i < node->mNumMeshes; i++) {
    const aiMesh *m = scene->mMeshes[node->mMeshes[i]];
    std::vector<Vertex> v(m->mNumVertices);
    for (unsigned k = 0; k < m->mNumVertices; k++) {
      v[k].pos = {m->mVertices[k].x, m->mVertices[k].y, m->mVertices[k].z};
      v[k].normal = {m->mNormals[k].x, m->mNormals[k].y, m->mNormals[k].z};
      if (m->mTextureCoords[0])
        v[k].tex = {m->mTextureCoords[0][k].x, m->mTextureCoords[0][k].y};
    }
    std::vector<unsigned> ind;
    for (unsigned f = 0; f < m->mNumFaces; ++f) {
      auto &face = m->mFaces[f];
      ind.insert(ind.end(), face.mIndices, face.mIndices + face.mNumIndices);
    }
    meshes.emplace_back(
        std::make_unique<Mesh>(sh, std::move(v), std::move(ind)));
  }
  for (unsigned c = 0; c < node->mNumChildren; ++c)
    loadNode(node->mChildren[c], scene, sh);
}

void ModelShape::setViewProj(const glm::mat4 &v, const glm::mat4 &p,
                             const glm::vec3 &e) {
  view = v;
  proj = p;
  eye = e;
}

void ModelShape::render() {
  if (meshes.empty())
    return;

  shader->use();
  shader->setMat4("model", modelMat);
  shader->setMat4("view", view);
  shader->setMat4("proj", proj);
  shader->setVec3("viewPos", eye);

  for (auto &m : meshes)
    m->render();
}

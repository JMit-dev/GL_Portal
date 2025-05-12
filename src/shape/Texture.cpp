#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "shape/Texture.h"
#include <algorithm>
#include <iostream> // optional: for error/debug prints
#include <stb_image.h>
#include <stdexcept>

float Texture2D::s_maxAniso = 1.0f; // initialised in first ctor
float Texture2D::s_aniso = 1.0f;

float Texture2D::anisotropy() { return s_aniso; }

void Texture2D::setAnisotropy(float v) {
  s_aniso = std::clamp(v, 1.0f, s_maxAniso);
}

Texture2D::Texture2D(std::string path) : file(std::move(path)) {
  if (s_maxAniso == 1.0f)
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &s_maxAniso);
}

void Texture2D::upload() const {
  int w, h, n;
  stbi_uc *data = stbi_load(file.c_str(), &w, &h, &n, 0);
  if (!data)
    throw std::runtime_error("Texture load failed: " + file);

  GLenum fmt = (n == 3) ? GL_RGB : GL_RGBA;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if (clampWrap) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  stbi_image_free(data);
}

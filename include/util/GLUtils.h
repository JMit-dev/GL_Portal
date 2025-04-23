#ifndef GLUTILS_H
#define GLUTILS_H
#include <glad/glad.h>
#include <string>

inline void GLCheckError(const std::string &where = "") {
#ifdef _DEBUG
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "[GL] Error 0x%04X at %s\n", err, where.c_str());
  }
#endif
}

#endif

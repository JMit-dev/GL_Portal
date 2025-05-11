#ifndef UTIL_TEXTURE_H
#define UTIL_TEXTURE_H
#include <glad/glad.h>
#include <string>
#include <utility>

class Texture2D {
public:
public:
  static float anisotropy();        // 1 … max (read‑only)
  static void setAnisotropy(float); // clamped setter
  static float maxAnisotropy() { return s_maxAniso; }

  explicit Texture2D(std::string path);

  void bind(int unit = 0) const {
    if (id == 0)
      upload();
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);

    /* apply current global value every bind (cheap, driver caches) */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, s_aniso);
  }

  ~Texture2D() {
    if (id)
      glDeleteTextures(1, &id);
  }

  Texture2D(const Texture2D &) = delete;
  Texture2D &operator=(const Texture2D &) = delete;
  Texture2D(Texture2D &&rhs) noexcept { id = std::exchange(rhs.id, 0); }
  Texture2D &operator=(Texture2D &&rhs) noexcept {
    if (this != &rhs) {
      if (id)
        glDeleteTextures(1, &id);
      id = std::exchange(rhs.id, 0);
    }
    return *this;
  }

private:
  void upload() const; // ← implementation below

  mutable GLuint id = 0;
  std::string file;

  static float s_maxAniso;
  static float s_aniso;
};

#endif

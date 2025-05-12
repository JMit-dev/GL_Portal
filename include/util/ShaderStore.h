#ifndef SHADER_STORE_H
#define SHADER_STORE_H
#include "util/Shader.h"
#include <memory>

class ShaderStore {
public:
  static ShaderStore &inst() {
    static ShaderStore s;
    return s;
  }

  Shader *phong() { ///< returns compiled phong shader (lazy)

    if (!phongShader) {
      phongShader = std::make_unique<Shader>("src/shader/phong.vert.glsl",
                                             "src/shader/phong.frag.glsl");
    }
    return phongShader.get();
  }

  Shader *textured() {
    if (!texturedShader)
      texturedShader = std::make_unique<Shader>(
          "src/shader/textured.vert.glsl", "src/shader/textured.frag.glsl");
    return texturedShader.get();
  }

  Shader *flatWhite() {
    if (!flatShader)
      flatShader = std::make_unique<Shader>("src/shader/flat.vert.glsl",
                                            "src/shader/flat.frag.glsl");
    return flatShader.get();
  }

private:
  ShaderStore() = default;
  std::unique_ptr<Shader> phongShader;
  std::unique_ptr<Shader> texturedShader;
  std::unique_ptr<Shader> flatShader;
};
#endif

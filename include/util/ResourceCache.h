#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H
#include "shape/Texture.h"
#include <map>
#include <memory>
#include <string>

class ResourceCache {
public:
  static ResourceCache &inst() {
    static ResourceCache c; // created *after* main begins
    return c;
  }

  /// returns a *shared* texture; loads it the first time only
  std::shared_ptr<Texture2D> texture(const std::string &path,
                                     bool clamp = false) {
    std::string key = clamp ? path + "#clamp" : path;

    auto it = texPool.find(key);
    if (it != texPool.end())
      return it->second;

    auto tex = std::make_shared<Texture2D>(path, clamp);
    texPool.emplace(key, tex);
    return tex;
  }

private:
  std::map<std::string, std::shared_ptr<Texture2D>> texPool;
};

#endif

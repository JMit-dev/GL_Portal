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
  std::shared_ptr<Texture2D> texture(const std::string &path) {
    auto it = texPool.find(path);
    if (it != texPool.end())
      return it->second;

    // first request -> create *after* GL context exists
    auto tex = std::make_shared<Texture2D>(path);
    texPool.emplace(path, tex);
    return tex;
  }

private:
  std::map<std::string, std::shared_ptr<Texture2D>> texPool;
};

#endif

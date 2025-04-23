#ifndef PORTAL_H
#define PORTAL_H

#include <glm/glm.hpp>
#include <memory>

class Renderable;
class Cell;

class Portal {
public:
  Portal(std::shared_ptr<Renderable> surface, Cell *destination,
         const glm::mat4 &transformToDst)
      : surface(std::move(surface)), destCell(destination),
        toDest(transformToDst) {}

  const Renderable &getSurface() const { return *surface; };
  const glm::mat4 &transform() const { return toDest; }
  Cell *destination() { return destCell; }

private:
  std::shared_ptr<Renderable> surface;
  Cell *destCell{nullptr};
  glm::mat4 toDest{1.f};
};

#endif

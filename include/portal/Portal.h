#ifndef PORTAL_H
#define PORTAL_H

#include "portal/Cell.h"
#include "shape/Renderable.h"
#include <glm/glm.hpp>
#include <memory>

class Portal {
public:
  Portal(std::shared_ptr<Renderable> surface, Cell *destination,
         const glm::mat4 &transformToDst)
      : surface(std::move(surface)), destCell(destination),
        toDest(transformToDst) {}

  Renderable &getSurface() { return *surface; }
  const Renderable &getSurface() const { return *surface; }
  Cell *destination() const { return destCell; }
  const glm::mat4 &transform() const { return toDest; }

  void setDestinationPortal(Portal *p) { returnPortal = p; }
  Portal *getDestinationPortal() const { return returnPortal; }
  bool getFlipView() const { return _flipView; }
  void setFlipView(bool b) { _flipView = b; }

private:
  std::shared_ptr<Renderable> surface;
  Cell *destCell{nullptr};
  glm::mat4 toDest{1.f};

  Portal *returnPortal{nullptr};
  bool _flipView = false;
};

#endif

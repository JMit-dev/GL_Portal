#ifndef CELL_H
#define CELL_H

#include <memory>
#include <vector>

class Renderable;
class Portal;

class Cell {
public:
  explicit Cell(std::vector<std::shared_ptr<Renderable>> geometry = {})
      : geometry(std::move(geometry)) {}

  void addPortal(std::shared_ptr<Portal> p) {
    portals.emplace_back(std::move(p));
  }
  const std::vector<std::shared_ptr<Portal>> &getPortals() const {
    return portals;
  }
  const std::vector<std::shared_ptr<Renderable>> &getGeometry() const {
    return geometry;
  }
  std::vector<std::shared_ptr<Renderable>> &getGeometry() { return geometry; }

private:
  std::vector<std::shared_ptr<Renderable>> geometry;
  std::vector<std::shared_ptr<Portal>> portals;
};

#endif

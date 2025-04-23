#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

class Cell;

class Scene {
public:
  Scene() = default;

  Cell *createCell();
  Cell *viewpointCell() const { return viewpoint; }
  void setViewpoint(Cell *c) { viewpoint = c; }

  const std::vector<std::unique_ptr<Cell>> &cells() const { return cellsVec; }

private:
  std::vector<std::unique_ptr<Cell>> cellsVec;
  Cell *viewpoint{nullptr};
};

#endif

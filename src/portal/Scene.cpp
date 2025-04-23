#include "portal/Scene.h"
#include "portal/Cell.h"

Cell *Scene::createCell() {
  cellsVec.emplace_back(std::make_unique<Cell>());
  return cellsVec.back().get();
}

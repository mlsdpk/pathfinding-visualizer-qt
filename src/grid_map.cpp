#include "grid_map.h"

#include <math.h>

#include <QDebug>
#include <algorithm>
#include <iostream>
#include <random>

GridMap::GridMap(int gridSize, QGraphicsScene *scene)
    : QObject(),
      gridSize_{gridSize},
      grids_(new QList<Grid *>()),
      scene_{scene},
      isStartConfigButtonChecked_{true},
      placeConfigMode_{false},
      editMapMode_{false} {}

GridMap::~GridMap() { delete grids_; }

void GridMap::setGridSize(int size) { gridSize_ = size; }

void GridMap::setPlaceConfigMode(bool b) { placeConfigMode_ = b; }

void GridMap::setEditingMapMode(bool b) { editMapMode_ = b; }

void GridMap::setStartConfigRadioButton(bool b) {
  isStartConfigButtonChecked_ = b;
}

void GridMap::setNumberOfGrids(int n) { numberOfGrids_ = n; }

int GridMap::getGridSize() const { return gridSize_; }

int GridMap::getNumberOfGrids() const { return numberOfGrids_; }

QList<int> GridMap::getFreeSpaceIdxs() const { return freeSpaceIdxs_; }

const QList<Grid *> *GridMap::getGridsList() const { return grids_; }

Grid *GridMap::getGoalGrid() const { return goalGrid_; }

Grid *GridMap::getStartGrid() const { return startGrid_; }

void GridMap::generateObstacles(int n) {
  int randomIdx;
  int obstCount = 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  while (obstCount < n) {
    // randomly generate index
    std::uniform_int_distribution<> dis(
        0, std::distance(freeSpaceIdxs_.begin(), freeSpaceIdxs_.end()) - 1);
    auto it = freeSpaceIdxs_.begin();
    std::advance(it, dis(gen));
    randomIdx = *it;

    // make the indexed grid as obstacle
    grids_->at(randomIdx)->setOccupied();

    // remove index from freespace indexes list
    freeSpaceIdxs_.removeOne(randomIdx);

    // increment obstacle count
    obstCount++;
  }
}

void GridMap::clearObstacles() {
  for (int i = 0; i < grids_->size(); i++) {
    if (grids_->at(i)->isOccupied()) {
      grids_->at(i)->setFree();
      freeSpaceIdxs_.append(i);
    }
  }
}

void GridMap::renderGrids(int grid_size) {
  setGridSize(grid_size);

  // clear the grids
  grids_->clear();

  // clear free space indexes
  freeSpaceIdxs_.clear();

  // clear start and goal grid ptrs
  startGrid_ = nullptr;
  goalGrid_ = nullptr;

  // clear the scene (also delete the objects)
  scene_->clear();

  // get grid size from slider value
  int v = gridSize_;
  v = (int)(ceil(v / 5.0) * 5);
  int no_of_row_grids_ = (int)(ceil(500.0 / v));

  // setting total number of grids in the map
  setNumberOfGrids(no_of_row_grids_ * no_of_row_grids_);

  int freeSpaceIdx = 0;
  for (int col = 0; col < no_of_row_grids_; col++) {
    for (int row = 0; row < no_of_row_grids_; row++) {
      // create grid obj
      Grid *g = new Grid(this);
      // set pos and size
      g->setRect(0, 0, v, v);
      g->setPos(0 + row * v, 0 + col * v);
      grids_->append(g);
      scene_->addItem(g);
      freeSpaceIdxs_.append(freeSpaceIdx);
      freeSpaceIdx++;
    }
  }

  // find neighbour indexes for each grid
  for (int col = 0; col < no_of_row_grids_; col++) {
    for (int row = 0; row < no_of_row_grids_; row++) {
      int node_idx = no_of_row_grids_ * col + row;
      if (row > 0)
        grids_->at(node_idx)->setNeighbourIdx(col * no_of_row_grids_ +
                                              (row - 1));
      if (row < (no_of_row_grids_ - 1))
        grids_->at(node_idx)->setNeighbourIdx(col * no_of_row_grids_ +
                                              (row + 1));
      if (col > 0)
        grids_->at(node_idx)->setNeighbourIdx((col - 1) * no_of_row_grids_ +
                                              row);
      if (col < (no_of_row_grids_ - 1))
        grids_->at(node_idx)->setNeighbourIdx((col + 1) * no_of_row_grids_ +
                                              row);
    }
  }

  // default start (top-left) and goal (bottom-right) configurations
  int startIdx = 0;
  int goalIdx =
      no_of_row_grids_ * (no_of_row_grids_ - 1) + (no_of_row_grids_ - 1);
  startGrid_ = grids_->at(startIdx);
  startGrid_->setStart();
  goalGrid_ = grids_->at(goalIdx);
  goalGrid_->setGoal();

  freeSpaceIdxs_.removeOne(startIdx);
  freeSpaceIdxs_.removeOne(goalIdx);
}

void GridMap::isGridClicked(Grid *grid) {
  // find the index of the grid
  int idx = grids_->indexOf(grid);

  // add/remove obstacles
  if (editMapMode_) {
    // if either place at start or goal, just return
    if (grid == startGrid_ || grid == goalGrid_) return;

    if (grid->isOccupied()) {
      grid->setFree();
      // free space now
      // append this grid index into free space indexes
      freeSpaceIdxs_.append(idx);
    } else {
      grid->setOccupied();
      // otherwise, remove
      freeSpaceIdxs_.removeOne(idx);
    }
  }
  // set start & goal configs
  else if (placeConfigMode_) {
    // if place at obst grid, just return
    if (grid->isOccupied()) return;

    // if either place at start or goal, just return
    if (grid == startGrid_ || grid == goalGrid_) return;

    // if start config radio button is checked
    if (isStartConfigButtonChecked_) {
      // make previous start grid to be empty
      if (startGrid_) {
        startGrid_->setFree();
        freeSpaceIdxs_.append(grids_->indexOf(startGrid_));
      }
      // make this grid to be start grid
      grid->setStart();
      freeSpaceIdxs_.removeOne(idx);
      startGrid_ = grid;
    } else {
      // make previous goal grid to be empty
      if (goalGrid_) {
        goalGrid_->setFree();
        freeSpaceIdxs_.append(grids_->indexOf(goalGrid_));
      }
      // make this grid to be goal grid
      grid->setGoal();
      freeSpaceIdxs_.removeOne(idx);
      goalGrid_ = grid;
    }
  }
}

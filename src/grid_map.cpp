#include "grid_map.h"

#include <math.h>

#include <QDebug>
#include <iostream>

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

int GridMap::getGridSize() const { return gridSize_; }

const QList<Grid *> *GridMap::getGridsList() const { return grids_; }

Grid *GridMap::getGoalGrid() const { return goalGrid_; }

Grid *GridMap::getStartGrid() const { return startGrid_; }

void GridMap::renderGrids(int grid_size) {
  setGridSize(grid_size);

  // clear the grids
  grids_->clear();

  // clear start and goal grid ptrs
  startGrid_ = nullptr;
  goalGrid_ = nullptr;

  // clear the scene (also delete the objects)
  scene_->clear();

  // get grid size from slider value
  int v = gridSize_;
  v = (int)(ceil(v / 5.0) * 5);
  int no_of_row_grids_ = (int)(ceil(500.0 / v));
  for (int col = 0; col < no_of_row_grids_; col++) {
    for (int row = 0; row < no_of_row_grids_; row++) {
      // create grid obj
      Grid *g = new Grid(this);
      // set pos and size
      g->setRect(0, 0, v, v);
      g->setPos(0 + row * v, 0 + col * v);
      grids_->append(g);
      scene_->addItem(g);
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
  startGrid_ = grids_->at(0);
  startGrid_->setStart();
  goalGrid_ = grids_->at(no_of_row_grids_ * (no_of_row_grids_ - 1) +
                         (no_of_row_grids_ - 1));
  goalGrid_->setGoal();
}

void GridMap::isGridClicked(Grid *grid) {
  // add/remove obstacles
  if (editMapMode_) {
    // if either place at start or goal, just return
    if (grid == startGrid_ || grid == goalGrid_) return;

    if (grid->isOccupied())
      grid->setFree();
    else
      grid->setOccupied();
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
      if (startGrid_) startGrid_->setFree();
      // make this grid to be start grid
      grid->setStart();
      startGrid_ = grid;
    } else {
      // make previous goal grid to be empty
      if (goalGrid_) goalGrid_->setFree();
      // make this grid to be goal grid
      grid->setGoal();
      goalGrid_ = grid;
    }
  }
}

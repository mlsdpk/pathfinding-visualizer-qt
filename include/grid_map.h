#pragma once

#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <QList>
#include <QObject>

#include "grid.h"

class GridMap : public QObject {
  Q_OBJECT
 public:
  /** \brief Default Constructor. */
  GridMap() = default;

  /** \brief Parameterized Constructor. */
  GridMap(int gridSize, QGraphicsScene *scene);

  /** \brief Destructor. */
  ~GridMap();

  /** \brief Set the size of each grid in the map. */
  void setGridSize(int size);

  /** \brief Allow changing the start and goal configurations
   *   by enabling the place configuration mode. */
  void setPlaceConfigMode(bool b);

  /** \brief Allow to add/remove obstacles in the map
   *  by enabling the editing map mode. */
  void setEditingMapMode(bool b);

  /** \brief Whether start config radio button is selected or not. */
  void setStartConfigRadioButton(bool b);

  /** \brief Set total number of grids in the map. */
  void setNumberOfGrids(int n);

  /** \brief Get the current grid size of the map. */
  int getGridSize() const;

  /** \brief Get total number of grids in the map. */
  int getNumberOfGrids() const;

  /** \brief Get free space indexes in the map. */
  QList<int> getFreeSpaceIdxs() const;

  /** \brief Get all the grids in the map as const list pointer. */
  const QList<Grid *> *getGridsList() const;

  /** \brief Get the pointer to goal grid in the map. */
  Grid *getGoalGrid() const;

  /** \brief Get the pointer to start grid in the map. */
  Grid *getStartGrid() const;

  /** \brief Randomly generate n number of obstacles in the map. */
  void generateObstacles(int n);

  /** \brief Clear all obstacles in the map. */
  void clearObstacles();

 public slots:
  /** \brief Slot function for rendering grids. */
  void renderGrids(int grid_size);

  /** \brief Slot function for manipulating the state of the grid
   *   when each grid in the map sends a clicked signal. */
  void isGridClicked(Grid *grid);

 private:
  /** \brief Grid size of the map. */
  int gridSize_;

  /** \brief Total number of grids the map. */
  int numberOfGrids_;

  /** \brief Pointer to start grid in the map. */
  Grid *startGrid_;

  /** \brief Pointer to goal grid in the map. */
  Grid *goalGrid_;

  /** \brief Pointer to list of all the grids in the map. */
  QList<Grid *> *grids_;

  /** \brief List of free space indexes in the map. */
  QList<int> freeSpaceIdxs_;

  /** \brief Qt Graphics scene object. */
  QGraphicsScene *scene_;

  /** \brief Flag for indicating whether start config radio
   *  button is checked or not. */
  bool isStartConfigButtonChecked_;

  /** \brief Flag for indicating whether place config mode or not. */
  bool placeConfigMode_;

  /** \brief Flag for indicating whether editing map mode or not. */
  bool editMapMode_;
};

#pragma once

#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QObject>
#include <iostream>

/** \brief Grid class containing different states of the grid
 *  (occupied, free, start and goal). This class is inherited
 *  from QObject and QGraphicsRectItem mainly for both purposes
 *  of rendering stuffs and signal/slots. */
class Grid : public QObject, public QGraphicsRectItem {
  Q_OBJECT
 public:
  /** \brief Constructor. */
  Grid(QObject *parent = nullptr);

  /** \brief Destructor. */
  ~Grid();

  /** \brief Getter for checking the grid is occupied or not */
  bool isOccupied() const;

  /** \brief Getter for checking the grid is
   *  start configuration or not */
  bool isStart() const;

  /** \brief Getter for checking the grid is
   *  goal configuration or not */
  bool isGoal() const;

  /** \brief Get neighbour grids' indexes  */
  QList<int> getNeighboursIdxs() const;

  // setters
  /** \brief Set grid as occupied */
  void setOccupied();

  /** \brief Set grid as free */
  void setFree();

  /** \brief Set grid as start configuration */
  void setStart();

  /** \brief Set grid as goal configuration */
  void setGoal();

  /** \brief Passing neighbour index into this function
   *  adds the index into neighboursIdxs_ list of the grid */
  void setNeighbourIdx(int idx);

  /** \brief Setting the cureent mode is map editing mode or not.
   *  This is used in combination with mousePressEvent for manipulating
   *  the state of the grid. */
  void setObstEditMode(bool b);

  /** \brief Setting the cureent mode is place configuration mode or not.
   *  This is used in combination with mousePressEvent for manipulating
   *  the state of the grid. */
  void setPlaceConfigMode(bool b);

  /** \brief Set freespace graphics effect to this grid. */
  void setFreeSpaceGraphicsEffect();

  /** \brief Set visited graphics effect to this grid. */
  void setVisitedGraphicsEffect();

  /** \brief Set path color to this grid. */
  void setPathColor();

 private:
  /** \brief Flag indicating whether grid is start configuration or not */
  bool isStart_;

  /** \brief Flag indicating whether grid is goal configuration or not */
  bool isGoal_;

  /** \brief Flag indicating whether grid is occupied or not */
  bool isOccupied_;

  /** \brief List object for neighbours' indexes of grid */
  QList<int> neighboursIdxs_;

 protected:
  /** \brief Mouse press event listener */
  void mousePressEvent(QGraphicsSceneMouseEvent *event);

 signals:
  /** \brief Signal for sending a grid is clicked containing a message of
   * pointer to clicked grid. */
  void mouseClicked(Grid *);
};

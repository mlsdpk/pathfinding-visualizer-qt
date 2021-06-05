#pragma once

#include <QList>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsObject>

#include "grid.h"

class GridMap: public QObject {
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

    /** \brief Get the current grid size of the map. */
    int getGridSize() const;

    /** \brief Get all the grids in the map as const list pointer. */
    const QList<Grid*>* getGridsList() const;

    /** \brief Get the pointer to goal grid in the map. */
    Grid* getGoalGrid() const;

    /** \brief Get the pointer to start grid in the map. */
    Grid* getStartGrid() const;

public slots:
    /** \brief Slot function for rendering grids. */
    void renderGrids(int grid_size);

    /** \brief Slot function for manipulating the state of the grid
     *   when each grid in the map sends a clicked signal. */
    void isGridClicked(Grid* grid);

private:
    /** \brief Grid size of the map. */
    int gridSize_;

    /** \brief Pointer to start grid in the map. */
    Grid *startGrid_;

    /** \brief Pointer to goal grid in the map. */
    Grid *goalGrid_;

    /** \brief Pointer to list of all the grids in the map. */
    QList<Grid*> *grids_;

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

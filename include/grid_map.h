#pragma once

#include <QList>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsObject>

#include "grid.h"

class GridMap: public QObject {
    Q_OBJECT
private:
    int gridSize_;
    QList<Grid*> *grids_;
    QGraphicsScene *scene_;
    bool isStartConfigButtonChecked_;
    bool placeConfigMode_;
    bool editMapMode_;
    Grid *startGrid_;
    Grid *goalGrid_;
public:
    GridMap(int gridSize, QGraphicsScene *scene);
    ~GridMap();

    void set_gridSize(int size);
    void setPlaceConfigMode(bool b);
    void setEditingMapMode(bool b);
    void setStartConfigRadioButton(bool b);

    int get_gridSize();
    const QList<Grid*>* getGridsList() const;
    Grid* getGoalGrid() const;
    Grid* getStartGrid() const;

public slots:
    void renderGrids(int grid_size);
    void isGridClicked(Grid* grid);
};

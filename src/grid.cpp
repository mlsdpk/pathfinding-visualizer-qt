#include "grid.h"
#include <iostream>
#include <QBrush>

Grid::Grid(QObject *parent)
  :  QObject(parent),
     isOccupied_{false},
     isStart_{false},
     isGoal_{false} {
    setRect(0, 0, 100, 100);
    connect(this, SIGNAL(mouseClicked(Grid*)), parent, SLOT(isGridClicked(Grid*)));
}

Grid::~Grid() {}

bool Grid::isOccupied() const {
    return isOccupied_;
}

bool Grid::isStart() const {
    return isStart_;
}

bool Grid::isGoal() const {
    return isGoal_;
}

QList<int> Grid::getNeighboursIdxs() const {
    return neighboursIdxs_;
}

void Grid::setOccupied() {
    isOccupied_ = true;
    isStart_ = false;
    isGoal_ = false;

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::black);
    setBrush(brush);
}

void Grid::setFree() {
    isOccupied_ = false;
    isStart_ = false;
    isGoal_ = false;

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::white);
    setBrush(brush);
}

void Grid::setStart() {
    isStart_ = true;
    isGoal_ = false;
    isOccupied_ = false;

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::green);
    setBrush(brush);
}

void Grid::setGoal() {
    isStart_ = false;
    isGoal_ = true;
    isOccupied_ = false;

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::red);
    setBrush(brush);
}

void Grid::setNeighbourIdx(int idx) {
    neighboursIdxs_.append(idx);
}

void Grid::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // emit the signal to parent
    emit mouseClicked(this);
    QGraphicsItem::mousePressEvent(event);
}

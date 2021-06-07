#include "grid.h"

Grid::Grid(QObject *parent)
    : QObject(parent), isOccupied_{false}, isStart_{false}, isGoal_{false} {
  setRect(0, 0, 100, 100);

  // create shadoweffect obj
  setFreeSpaceGraphicsEffect();

  connect(this, SIGNAL(mouseClicked(Grid *)), parent,
          SLOT(isGridClicked(Grid *)));
}

Grid::~Grid() {}

bool Grid::isOccupied() const { return isOccupied_; }

bool Grid::isStart() const { return isStart_; }

bool Grid::isGoal() const { return isGoal_; }

QList<int> Grid::getNeighboursIdxs() const { return neighboursIdxs_; }

void Grid::setOccupied() {
  isOccupied_ = true;
  isStart_ = false;
  isGoal_ = false;

  setBrush(QBrush(QColor(0, 0, 0, 255), Qt::SolidPattern));
}

void Grid::setFree() {
  isOccupied_ = false;
  isStart_ = false;
  isGoal_ = false;

  setBrush(QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
}

void Grid::setStart() {
  isStart_ = true;
  isGoal_ = false;
  isOccupied_ = false;

  setBrush(QBrush(QColor(51, 255, 51, 245), Qt::SolidPattern));
}

void Grid::setGoal() {
  isStart_ = false;
  isGoal_ = true;
  isOccupied_ = false;

  setBrush(QBrush(QColor(255, 51, 51, 245), Qt::SolidPattern));
}

void Grid::setNeighbourIdx(int idx) { neighboursIdxs_.append(idx); }

void Grid::setFreeSpaceGraphicsEffect() {
  QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
  shadowEffect->setColor(QColor(160, 160, 160, 245));
  shadowEffect->setOffset(1, 1);
  shadowEffect->setBlurRadius(5);
  setGraphicsEffect(shadowEffect);
  setScale(0.8);
  setOpacity(0.8);
  setBrush(QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
}

void Grid::setVisitedGraphicsEffect() {
  setBrush(QBrush(QColor(224, 224, 224, 245), Qt::SolidPattern));
  setOpacity(1);

  // create shadoweffect obj
  QGraphicsDropShadowEffect *dps = new QGraphicsDropShadowEffect(this);
  dps->setColor(QColor(40, 40, 40, 245));
  dps->setOffset(1, 1);
  dps->setBlurRadius(10);
  setGraphicsEffect(dps);
}

void Grid::setPathColor() {
  setBrush(QBrush(QColor(160, 160, 160, 200), Qt::SolidPattern));
  setOpacity(1);
}

void Grid::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  // emit the signal to parent
  emit mouseClicked(this);
  QGraphicsItem::mousePressEvent(event);
}

#include "grid.h"

Grid::Grid(QObject *parent)
    : QObject(parent), is_occupied_{false}, is_start_{false}, is_goal_{false} {
  setRect(0, 0, 100, 100);

  // create shadoweffect obj
  setFreeSpaceGraphicsEffect();

  connect(this, SIGNAL(mouseClicked(Grid *)), parent,
          SLOT(isGridClicked(Grid *)));
}

Grid::~Grid() {}

bool Grid::isOccupied() const { return is_occupied_; }

bool Grid::isStart() const { return is_start_; }

bool Grid::isGoal() const { return is_goal_; }

QList<int> Grid::getNeighboursIdxs() const { return neighbours_idxs_; }

void Grid::setOccupied() {
  is_occupied_ = true;
  is_start_ = false;
  is_goal_ = false;

  setBrush(QBrush(QColor(0, 0, 0, 255), Qt::SolidPattern));
}

void Grid::setFree() {
  is_occupied_ = false;
  is_start_ = false;
  is_goal_ = false;

  setBrush(QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
}

void Grid::setStart() {
  is_start_ = true;
  is_goal_ = false;
  is_occupied_ = false;

  setBrush(QBrush(QColor(51, 255, 51, 245), Qt::SolidPattern));
}

void Grid::setGoal() {
  is_start_ = false;
  is_goal_ = true;
  is_occupied_ = false;

  setBrush(QBrush(QColor(255, 51, 51, 245), Qt::SolidPattern));
}

void Grid::setNeighbourIdx(int idx) { neighbours_idxs_.append(idx); }

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

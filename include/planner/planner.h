#pragma once

#include <QObject>
#include <unordered_map>

#include "grid_map.h"

namespace planner {

struct Vertex {
  bool isObstacle{false};
  bool isVisited{false};
  bool isFrontier{false};
  QPointF pos;
  Vertex* parent;
  QList<Vertex*> neighbours;
};

class Planner : public QObject {
  Q_OBJECT
 public:
  Planner();
  virtual ~Planner();
  virtual void onInit() = 0;
  virtual void search() = 0;

 protected:
  bool initialized_;
  bool reseted_;
  bool done_;
  bool finished_;
  QList<Vertex*> vertices_;
  QList<int> visitedVerticesIdxOrder_;
  std::unordered_map<int, int> childParentIdxs_;

 signals:
  void finished();
  void sendVisitedVerticesIdxOrder(const QList<int>* visitedVerticesIdxOrder);
  void sendChildParentIdxs(const std::unordered_map<int, int>* childParentIdxs);

 private slots:
  virtual void plan(const GridMap* gm) = 0;
  virtual void receiveResetStatus();
  virtual void receiveFinishStatus();
  virtual void process();
};

}  // namespace planner

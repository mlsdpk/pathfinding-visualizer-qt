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
  double gValue;
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
  QList<int> visited_vertices_idx_order_;
  std::unordered_map<int, int> child_parent_idxs_;
  std::chrono::system_clock::time_point init_time_;

 signals:
  void finished();
  void sendVisitedVerticesIdxOrder(
      const QList<int>* visited_vertices_idx_order);
  void sendChildParentIdxs(
      const std::unordered_map<int, int>* child_parent_idxs);

 private slots:
  virtual void plan(const GridMap* gm) = 0;
  virtual void receiveResetStatus();
  virtual void receiveFinishStatus();
  virtual void process();
};

}  // namespace planner

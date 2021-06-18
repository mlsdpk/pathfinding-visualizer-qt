#pragma once

#include <queue>

#include "planner/planner.h"

namespace planner {
namespace search {
class ASTAR : public Planner {
  // custom A* vertex struct
  struct Vertex {
    bool isObstacle{false};
    bool isVisited{false};
    QPointF pos;
    Vertex* parent;
    QList<Vertex*> neighbours;
    double gValue;

    // - key to store in the priority queue
    //   <f-value, g-value>
    // - uses lexicographical ordering for minimum vertex selection
    std::pair<double, double> key;
  };

  // custom function for returning minimum distance node
  // to be used in priority queue
  struct MinimumDistanceASTAR {
    // operator overloading
    bool operator()(const Vertex* v1, const Vertex* v2) const {
      // std::pair support lexicographical comparison
      // which is convenient for us to implement the comparison
      // without writing additional code
      return v1->key > v2->key;
    }
  };

 public:
  ASTAR();
  ~ASTAR();

  double L1_Distance(const Vertex* v1, const Vertex* v2);
  double L2_Distance(const Vertex* v1, const Vertex* v2);

 private:
  Vertex* start_vertex_;
  Vertex* goal_vertex_;
  std::priority_queue<Vertex*, std::vector<Vertex*>, MinimumDistanceASTAR>
      frontier_;
  QList<Vertex*> vertices_;

  void onInit() override;
  void search() override;

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

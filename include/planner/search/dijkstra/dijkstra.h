#pragma once

#include <queue>

#include "planner/planner.h"

namespace planner {
namespace search {
class DIJKSTRA : public Planner {
  // custom function for returning minimum distance node
  // to be used in priority queue
  // Note:
  // - Since we are using priority queue, bfs is faster than dijkstra.
  // - However, dijkstra can work with weighted edges.
  struct MinimumDistanceDIJKSTRA {
    // operator overloading
    bool operator()(const Vertex* n1, const Vertex* n2) const {
      return n1->gValue > n2->gValue;
    }
  };

 public:
  DIJKSTRA();
  ~DIJKSTRA();

  double L1_Distance(const Vertex* v1, const Vertex* v2);

 private:
  Vertex* start_vertex_;
  Vertex* goal_vertex_;
  std::priority_queue<Vertex*, std::vector<Vertex*>, MinimumDistanceDIJKSTRA>
      frontier_;

  void onInit() override;
  void search() override;

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

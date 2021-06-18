#pragma once

#include <queue>

#include "planner/planner.h"

namespace planner {
namespace search {
class BFS : public Planner {
 public:
  BFS();
  ~BFS();

 private:
  Vertex* start_vertex_;
  Vertex* goal_vertex_;
  std::queue<Vertex*> frontier_;

  void onInit() override;
  void search() override;

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

#pragma once

#include <stack>

#include "planner/planner.h"

namespace planner {
namespace search {
class DFS : public Planner {
 public:
  DFS();
  ~DFS();

 private:
  void onInit() override;
  void search() override;

  Vertex* start_vertex_;
  Vertex* goal_vertex_;
  std::stack<Vertex*> frontier_;

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

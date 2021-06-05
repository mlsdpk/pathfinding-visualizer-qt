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

  Vertex* startVertex_;
  Vertex* goalVertex_;
  std::stack<Vertex*> frontier_;

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

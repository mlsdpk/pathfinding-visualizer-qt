#pragma once

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

 private slots:
  void plan(const GridMap* gm) override;
};
}  // namespace search
}  // namespace planner

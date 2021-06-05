#include "planner/search/dfs/dfs.h"

#include <iostream>

planner::search::DFS::DFS() : Planner() {
  std::cout << "DFS created: " << this << std::endl;
}

planner::search::DFS::~DFS() {
  std::cout << "DFS deleted: " << this << std::endl;
}

void planner::search::DFS::onInit() { std::cout << "DFS onInit!" << std::endl; }

void planner::search::DFS::plan(const GridMap *gm) {
  // add some initializations here

  initialized_ = true;
  std::cout << "DFS initialized!" << std::endl;
}

void planner::search::DFS::search() {}

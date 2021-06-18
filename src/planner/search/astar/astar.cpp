#include "planner/search/astar/astar.h"

#include <cmath>
#include <iostream>

planner::search::ASTAR::ASTAR() : Planner() {
  std::cout << "ASTAR created: " << this << std::endl;
}

planner::search::ASTAR::~ASTAR() {
  std::cout << "ASTAR deleted: " << this << std::endl;
}

double planner::search::ASTAR::L1_Distance(const Vertex* v1, const Vertex* v2) {
  return fabs(v1->pos.x() - v2->pos.x()) + fabs(v1->pos.y() - v2->pos.y());
}

double planner::search::ASTAR::L2_Distance(const Vertex* v1, const Vertex* v2) {
  return sqrt(pow(v1->pos.x() - v2->pos.x(), 2) +
              pow(v1->pos.y() - v2->pos.y(), 2));
}

void planner::search::ASTAR::onInit() {
  std::cout << "ASTAR onInit!" << std::endl;
}

void planner::search::ASTAR::plan(const GridMap* gm) {
  // add some initializations here

  // clear and delete the vertices
  while (!vertices_.isEmpty()) delete vertices_.takeFirst();

  visited_vertices_idx_order_.clear();
  child_parent_idxs_.clear();

  // create graph with vertices using grid map
  for (auto g : *(gm->getGridsList())) {
    Vertex* vertex = new Vertex();
    vertex->isObstacle = g->isOccupied();
    vertex->pos = g->pos();
    vertex->gValue = std::numeric_limits<double>::infinity();
    vertices_.append(vertex);

    if (g->isStart())
      start_vertex_ = vertex;
    else if (g->isGoal())
      goal_vertex_ = vertex;
  }

  // find number of vertex in each row/col
  int rc_len = (int)std::sqrt(vertices_.size());

  // find neighbours of each grid
  for (int col = 0; col < rc_len; col++) {
    for (int row = 0; row < rc_len; row++) {
      int node_idx = rc_len * col + row;
      if (row > 0)
        vertices_.at(node_idx)->neighbours.append(
            vertices_.at(col * rc_len + (row - 1)));
      if (row < (rc_len - 1))
        vertices_.at(node_idx)->neighbours.append(
            vertices_.at(col * rc_len + (row + 1)));
      if (col > 0)
        vertices_.at(node_idx)->neighbours.append(
            vertices_.at((col - 1) * rc_len + row));
      if (col < (rc_len - 1))
        vertices_.at(node_idx)->neighbours.append(
            vertices_.at((col + 1) * rc_len + row));
    }
  }

  // initialize A* by clearing frontier
  while (!frontier_.empty()) {
    frontier_.pop();
  }

  // add start vertex into frontier
  start_vertex_->isVisited = true;
  start_vertex_->gValue = 0.0;
  start_vertex_->key = std::make_pair(L2_Distance(start_vertex_, goal_vertex_),
                                      start_vertex_->gValue);
  frontier_.push(start_vertex_);

  initialized_ = true;
  std::cout << "A* initialized!" << std::endl;

  // get current time
  init_time_ = std::chrono::system_clock::now();
}

void planner::search::ASTAR::search() {
  if (!frontier_.empty()) {
    Vertex* v = frontier_.top();
    visited_vertices_idx_order_.append(vertices_.indexOf(v));
    frontier_.pop();

    if (v == goal_vertex_) done_ = true;

    for (auto nb_ptr : v->neighbours) {
      if (!nb_ptr->isVisited && !nb_ptr->isObstacle) {
        // calculate new g value
        // cost to come to current visiting neighbour
        double dist = v->gValue + L1_Distance(v, nb_ptr);

        // choose parent
        if (dist < nb_ptr->gValue) {
          nb_ptr->parent = v;
          nb_ptr->gValue = dist;
          nb_ptr->isVisited = true;

          // calculate key
          nb_ptr->key =
              std::make_pair(nb_ptr->gValue + L2_Distance(nb_ptr, goal_vertex_),
                             nb_ptr->gValue);
          frontier_.push(nb_ptr);
          child_parent_idxs_[vertices_.indexOf(nb_ptr)] = vertices_.indexOf(v);
        }
      }
    }

  } else {
    done_ = true;
  }
}

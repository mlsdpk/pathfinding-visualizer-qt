#include "planner/search/bfs/bfs.h"
#include <iostream>
#include <cmath>

planner::search::BFS::BFS(): Planner() {
    std::cout << "BFS created: " << this << std::endl;
}

planner::search::BFS::~BFS() {
    std::cout << "BFS deleted: " << this << std::endl;
}

void planner::search::BFS::onInit() {
    std::cout << "BFS onInit!" << std::endl;
}

void planner::search::BFS::plan(const GridMap *gm) {
    // add some initializations here

    // clear and delete the vertices
    while (!vertices_.isEmpty())
        delete vertices_.takeFirst();

    visitedVerticesIdxOrder_.clear();
    childParentIdxs_.clear();

    // create graph with vertices using grid map
    for (auto g: *(gm->getGridsList())) {
        Vertex* vertex = new Vertex();
        vertex->isObstacle = g->isOccupied();
        vertex->pos = g->pos();
        vertices_.append(vertex);

        if (g->isStart())
            startVertex_ = vertex;
        else if (g->isGoal())
            goalVertex_ = vertex;
    }

    // find number of vertex in each row/col
    int rc_len = (int)std::sqrt(vertices_.size());

    // find neighbours of each grid
    for (int col=0; col<rc_len; col++) {
        for (int row=0; row<rc_len; row++) {
            int node_idx = rc_len * col + row;
            if (row > 0)
                vertices_.at(node_idx)->neighbours.append(vertices_.at(col * rc_len + (row - 1)));
            if (row < (rc_len - 1))
                vertices_.at(node_idx)->neighbours.append(vertices_.at(col * rc_len + (row + 1)));
            if (col > 0)
                vertices_.at(node_idx)->neighbours.append(vertices_.at((col - 1) * rc_len + row));
            if (col < (rc_len - 1))
                vertices_.at(node_idx)->neighbours.append(vertices_.at((col + 1) * rc_len + row));
        }
    }

    // initialize bfs by clearing frontier
    while (!frontier_.empty()) {
        frontier_.pop();
    }

    // add start vertex into frontier
    startVertex_->isVisited = true;
    frontier_.push(startVertex_);

    initialized_ = true;
    std::cout << "BFS initialized!" << std::endl;
}

void planner::search::BFS::search() {
    if (!frontier_.empty()) {
        Vertex* v = frontier_.front();
        v->isFrontier = false;
        frontier_.pop();

        if (v == goalVertex_)
            done_ = true;

        for (auto nb_ptr: v->neighbours) {
            if (!nb_ptr->isVisited && !nb_ptr->isObstacle) {
                nb_ptr->parent = v;
                nb_ptr->isVisited = true;
                nb_ptr->isFrontier = true;
                frontier_.push(nb_ptr);
                visitedVerticesIdxOrder_.append(vertices_.indexOf(nb_ptr));
                childParentIdxs_[vertices_.indexOf(nb_ptr)] = vertices_.indexOf(v);
            }
        }

    } else {
        done_ = true;
    }
}

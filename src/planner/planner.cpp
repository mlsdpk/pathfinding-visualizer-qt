#include "planner/planner.h"
#include <iostream>

planner::Planner::Planner() {
    std::cout << "Planner created" << std::endl;
    initialized_ = false;
    done_ = false;
    reseted_ = false;
    finished_ = false;
}

planner::Planner::~Planner() {
    // clear and delete the vertices
    while (!vertices_.isEmpty())
        delete vertices_.takeFirst();
}

void planner::Planner::receiveResetStatus() {
    reseted_ = true;
}

void planner::Planner::receiveFinishStatus() {
    finished_ = true;
}

void planner::Planner::process() {
    onInit();
    std::cout << "Algorithm started!" << std::endl;
    while (true) {
        // run the algorithm
        if (initialized_ && !done_) {
            // algorithm function will b here
            search();
            if (done_) {
                std::cout << "Algorithm done!" << std::endl;
                emit sendVisitedVerticesIdxOrder(&visitedVerticesIdxOrder_);
                emit sendChildParentIdxs(&childParentIdxs_);
            }
        }
        if (reseted_) {
            initialized_ = false;
            done_ = false;
            reseted_ = false;
            std::cout << "Algorithm resetted!" << std::endl;
        }
        if (finished_) {
            std::cout << "Algorithm finished! Exitting..." << std::endl;
            break;
        }
    }
    emit finished();
}

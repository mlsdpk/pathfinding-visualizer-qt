#include "mainwindow.h"

#include <math.h>

#include <iostream>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      map_editing_mode_{false},
      place_config_mode_{false},
      is_gui_reset_{true} {
  ui_->setupUi(this);

  setFixedSize(717, 574);

  // create scene object and pass it to ui_
  // defaut gui_ parameters for now
  scene_ = new QGraphicsScene(this);
  scene_->setSceneRect(0, 0, 500, 500);
  ui_->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_->graphicsView->setScene(scene_);

  ////////////////////////////////////
  /// buttons inital configurations
  ////////////////////////////////////

  // disable the RESET button
  ui_->resetButton->setEnabled(false);

  // map editing mode pushbutton
  ui_->editMapButton->setDefault(false);

  // obstacle number spinbox
  ui_->obstSpinBox->setEnabled(false);

  // generate obstacle push button
  ui_->generateObstPushButton->setEnabled(false);

  // clear obstacles push button
  ui_->clearObstPushButton->setEnabled(false);

  // place configuration mode pushbutton
  ui_->placeConfigButton->setDefault(false);

  // start config radio button
  ui_->startConfigRadioButton->setChecked(true);
  ui_->startConfigRadioButton->setEnabled(false);

  // goal config radio button
  ui_->goalConfigRadioButton->setEnabled(false);
  ////////////////////////////////////

  ////////////////////////////////
  /// grids and grid map related
  ////////////////////////////////
  // set minimum and maximum values of slider
  // map size - 500x500
  // max grid numbers = 50x50
  // min grid numbers = 2x2
  ui_->gridSlider->setMinimum(10);
  ui_->gridSlider->setMaximum(250);
  ui_->gridSlider->setTickInterval(5);
  ui_->gridSlider->setSingleStep(5);
  ui_->gridSlider->setEnabled(false);

  // setting default grid size (50px)
  int grid_size = 50;
  ui_->gridSlider->setValue(grid_size);

  grid_map_ = new GridMap(grid_size, scene_);
  grid_map_->renderGrids(grid_size);
  connect(ui_->gridSlider, SIGNAL(valueChanged(int)), grid_map_,
          SLOT(renderGrids(int)));
  ////////////////////////////////

  ////////////////////////////////
  /// obstacle number spinbox
  ////////////////////////////////
  // set initial minimum and maximum values for spinbox
  ui_->obstSpinBox->setMinimum(0);
  ui_->obstSpinBox->setMaximum(grid_map_->getFreeSpaceIdxs().size());

  ////////////////////////////////
  /// algorithm combobox
  ////////////////////////////////
  ui_->comboBox->addItem("BFS");
  ui_->comboBox->addItem("DFS");
  ui_->comboBox->addItem("DIJKSTRA");
  ui_->comboBox->addItem("A*");
  ////////////////////////////////

  ////////////////////////////////
  /// visualization related
  ////////////////////////////////
  connect(&visualization_timer_, SIGNAL(timeout()), this, SLOT(visualize()));
  visualization_current_idx_ = 0;

  ui_->animationSpeedSlider->setRange(1, 100);
  ui_->animationSpeedSlider->setValue(50);

  visualization_timer_.setInterval(ui_->animationSpeedSlider->value());

  // animation checkbox
  ui_->animationCheckBox->setChecked(true);
  ////////////////////////////////
}

// destructor
MainWindow::~MainWindow() {
  delete ui_;
  delete grid_map_;
  emit sendFinishStatus();
  planner_thread_->wait();
}

// map editing button slot
void MainWindow::on_editMapButton_pressed() {
  ui_->editMapButton->setDefault(!map_editing_mode_);
  map_editing_mode_ = !map_editing_mode_;

  // enable/disable other gui_ stuffs
  // grid size slider
  ui_->gridSlider->setEnabled(map_editing_mode_);
  // place config push button
  ui_->placeConfigButton->setEnabled(!map_editing_mode_);
  // RUN push button
  ui_->runButton->setEnabled(!map_editing_mode_);
  // animation checkbox
  ui_->animationCheckBox->setEnabled(!map_editing_mode_);
  // animation speed slider
  ui_->animationSpeedSlider->setEnabled(!map_editing_mode_);
  // obstacle number spinbox
  ui_->obstSpinBox->setEnabled(map_editing_mode_);
  // generate obstacle push button
  ui_->generateObstPushButton->setEnabled(map_editing_mode_);
  // clear obstacle push button
  ui_->clearObstPushButton->setEnabled(map_editing_mode_);

  ui_->obstSpinBox->setMaximum(grid_map_->getFreeSpaceIdxs().size());

  // pass edit mode to grid map
  grid_map_->setEditingMapMode(map_editing_mode_);
}

void MainWindow::on_gridSlider_sliderReleased() {
  ui_->obstSpinBox->setMaximum(grid_map_->getFreeSpaceIdxs().size());
}

void MainWindow::on_generateObstPushButton_clicked() {
  grid_map_->generateObstacles(ui_->obstSpinBox->value());
  ui_->obstSpinBox->setMaximum(grid_map_->getFreeSpaceIdxs().size());
}

void MainWindow::on_clearObstPushButton_clicked() {
  grid_map_->clearObstacles();
  ui_->obstSpinBox->setMaximum(grid_map_->getFreeSpaceIdxs().size());
}

// place configuration mode slot
void MainWindow::on_placeConfigButton_pressed() {
  ui_->placeConfigButton->setDefault(!place_config_mode_);
  place_config_mode_ = !place_config_mode_;

  // enable/disable gui_ stuffs
  // start and goal radio buttons
  ui_->startConfigRadioButton->setEnabled(place_config_mode_);
  ui_->goalConfigRadioButton->setEnabled(place_config_mode_);

  if (is_gui_reset_) {
    // map editing button
    ui_->editMapButton->setEnabled(!place_config_mode_);
    // RUN push button
    ui_->runButton->setEnabled(!place_config_mode_);
    // animation checkbox
    ui_->animationCheckBox->setEnabled(!place_config_mode_);
    // animation speed slider
    ui_->animationSpeedSlider->setEnabled(!place_config_mode_);
    // make start radio button always selected first
    ui_->startConfigRadioButton->setChecked(true);
    grid_map_->setStartConfigRadioButton(true);
  } else {
    // only allow to change goal vertex if planner is not reset yet
    ui_->startConfigRadioButton->setEnabled(false);
    ui_->startConfigRadioButton->setChecked(false);
    ui_->goalConfigRadioButton->setChecked(true);
    grid_map_->setStartConfigRadioButton(false);
  }

  grid_map_->setPlaceConfigMode(place_config_mode_);
}

void MainWindow::on_startConfigRadioButton_pressed() {
  grid_map_->setStartConfigRadioButton(true);
}

void MainWindow::on_goalConfigRadioButton_pressed() {
  grid_map_->setStartConfigRadioButton(false);
}

// this slot is called when the algorithm finshes and sends a signal
// of visited vertices in order
void MainWindow::receiveVisitedVerticesIdxOrder(
    const QList<int> *visited_vertices_idx_order) {
  for (auto idx : *visited_vertices_idx_order) {
    visited_vertices_idx_order_.append(idx);
  }
}

// this slot is called when the algorithm finishes and sends a signal
// of (child, parent) pairs hash map of vertices
void MainWindow::receiveChildParentIdxs(
    const std::unordered_map<int, int> *child_parent_idxs) {
  child_parent_idxs_ = *child_parent_idxs;

  // if animation is not allowed
  // we visualize the visited vertices and path without using timer
  if (!animate_planning_) {
    // render visited grids
    for (auto idx : visited_vertices_idx_order_) {
      if (idx != start_key_idx_ && idx != goal_key_idx_) {
        grid_map_->getGridsList()->at(idx)->setVisitedGraphicsEffect();
      }
    }
    // render path
    while (child_parent_idxs_[current_path_key_idx_] != start_key_idx_) {
      // find the parent vertex of current key (child)
      current_path_key_idx_ = child_parent_idxs_[current_path_key_idx_];
      grid_map_->getGridsList()->at(current_path_key_idx_)->setPathColor();
    }
  }
}

// slot timer function for visualization
void MainWindow::visualize() {
  // if we have idxs for visualization
  // start the visualization
  if (visited_vertices_idx_order_.size() > 0) {
    if (animate_planning_) {
      // show all visited vertices
      if (visualization_current_idx_ <=
          visited_vertices_idx_order_.size() - 1) {
        // skip the start and goal vertex
        if (visited_vertices_idx_order_.at(visualization_current_idx_) !=
                goal_key_idx_ &&
            visited_vertices_idx_order_.at(visualization_current_idx_) !=
                start_key_idx_) {
          grid_map_->getGridsList()
              ->at(visited_vertices_idx_order_.at(visualization_current_idx_))
              ->setVisitedGraphicsEffect();
        }

        visualization_current_idx_++;
      }
      // if done, visualize solution path until it reaches start vertex
      else if (child_parent_idxs_[current_path_key_idx_] != start_key_idx_) {
        // find the parent vertex of current key (child)
        current_path_key_idx_ = child_parent_idxs_[current_path_key_idx_];
        grid_map_->getGridsList()->at(current_path_key_idx_)->setPathColor();
      }
    }
  }
}

// main slot function for changing the algorithm
// all the necessary things effected by the algorithm changes should be added
// here including the signal and slots related to algorithms
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) {
  // send a signal to algorithm to stop execution
  emit sendFinishStatus();

  // create new thread obj
  planner_thread_ = new QThread(this);

  // choose planners
  if (arg1 == "BFS") {
    planner_.reset(new planner::search::BFS());
  } else if (arg1 == "DFS") {
    planner_.reset(new planner::search::DFS());
  } else if (arg1 == "DIJKSTRA") {
    planner_.reset(new planner::search::DIJKSTRA());
  } else if (arg1 == "A*") {
    planner_.reset(new planner::search::ASTAR());
  }

  // for RUN & RESET buttons
  connect(this, SIGNAL(sendGridMapInformation(const GridMap *)), planner_.get(),
          SLOT(plan(const GridMap *)), Qt::DirectConnection);
  connect(this, SIGNAL(sendResetStatus()), planner_.get(),
          SLOT(receiveResetStatus()), Qt::DirectConnection);

  // main function loop of the planner thread
  connect(planner_thread_, SIGNAL(started()), planner_.get(), SLOT(process()));

  // for qui_tting stuffs
  connect(this, SIGNAL(sendFinishStatus()), planner_.get(),
          SLOT(receiveFinishStatus()), Qt::DirectConnection);
  connect(planner_.get(), SIGNAL(finished()), planner_thread_, SLOT(quit()),
          Qt::DirectConnection);
  connect(planner_thread_, SIGNAL(finished()), planner_thread_,
          SLOT(deleteLater()));

  // for visualization stuffs
  connect(planner_.get(),
          SIGNAL(sendVisitedVerticesIdxOrder(const QList<int> *)), this,
          SLOT(receiveVisitedVerticesIdxOrder(const QList<int> *)),
          Qt::DirectConnection);
  connect(planner_.get(),
          SIGNAL(sendChildParentIdxs(const std::unordered_map<int, int> *)),
          this,
          SLOT(receiveChildParentIdxs(const std::unordered_map<int, int> *)));

  // move the current planner onto the thread
  // and initialize the algorithm
  planner_.get()->moveToThread(planner_thread_);
  planner_thread_->start();
}

// RUN pushbutton clicked slot
void MainWindow::on_runButton_clicked() {
  // emit a signal to algorithm slot containing gridmap information
  emit sendGridMapInformation(grid_map_);

  // get the start and goal vertex indexes
  goal_key_idx_ = grid_map_->getGridsList()->indexOf(grid_map_->getGoalGrid());
  start_key_idx_ =
      grid_map_->getGridsList()->indexOf(grid_map_->getStartGrid());
  current_path_key_idx_ = goal_key_idx_;

  // disable the RUN button
  ui_->runButton->setEnabled(false);

  // enable the RESET button
  ui_->resetButton->setEnabled(true);

  // disable the animation checkbox
  ui_->animationCheckBox->setEnabled(false);

  // disable the map editing button
  ui_->editMapButton->setEnabled(false);

  // animate based on checkbox
  if (ui_->animationCheckBox->isChecked()) {
    visualization_timer_.start();
    animate_planning_ = true;
  } else
    animate_planning_ = false;

  is_gui_reset_ = false;
}

// RESET pushbutton clicked slot
void MainWindow::on_resetButton_clicked() {
  // send reset status to algorithm
  emit sendResetStatus();

  // stop the timer
  visualization_timer_.stop();

  // enable the RUN button
  ui_->runButton->setEnabled(true);

  // disable the RESET button
  ui_->resetButton->setEnabled(false);

  // enable the animation checkbox
  ui_->animationCheckBox->setEnabled(true);

  // enable the map editing button
  ui_->editMapButton->setEnabled(true);

  // make edit map button not selected
  ui_->editMapButton->setDefault(false);
  map_editing_mode_ = false;

  // make place config button not selected
  ui_->placeConfigButton->setDefault(false);
  place_config_mode_ = false;
  // checked start radio button as default
  ui_->startConfigRadioButton->setChecked(true);
  // disable start and goal radio buttons
  ui_->startConfigRadioButton->setEnabled(false);
  ui_->goalConfigRadioButton->setEnabled(false);

  // remove all the rendered path and visited vertices
  for (auto idx : visited_vertices_idx_order_) {
    if (idx != start_key_idx_ && idx != goal_key_idx_)
      grid_map_->getGridsList()->at(idx)->setFreeSpaceGraphicsEffect();
  }

  // clear visualization related stuffs
  visited_vertices_idx_order_.clear();
  child_parent_idxs_.clear();
  visualization_current_idx_ = 0;

  is_gui_reset_ = true;
}

// slot for setting the animation speed
void MainWindow::on_animationSpeedSlider_valueChanged(int value) {
  visualization_timer_.setInterval(value);
}

// slot for enable/disable the animation
void MainWindow::on_animationCheckBox_stateChanged(int arg1) {
  // if checked, enable speed slider
  if (ui_->animationCheckBox->isChecked())
    ui_->animationSpeedSlider->setEnabled(true);
  else
    ui_->animationSpeedSlider->setEnabled(false);
}

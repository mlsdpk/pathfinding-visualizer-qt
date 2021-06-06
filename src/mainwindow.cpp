#include "mainwindow.h"

#include <math.h>

#include <iostream>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mapEditingMode_{false},
      placeConfigMode_{false},
      isGuiReset_{true} {
  ui->setupUi(this);

  // create scene object and pass it to ui
  // defaut gui parameters for now
  scene = new QGraphicsScene(this);
  scene->setSceneRect(0, 0, 500, 500);
  ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->graphicsView->setScene(scene);

  ////////////////////////////////////
  /// buttons inital configurations
  ////////////////////////////////////

  // disable the RESET button
  ui->resetButton->setEnabled(false);

  // map editing mode pushbutton
  ui->editMapButton->setDefault(false);

  // obstacle number spinbox
  ui->obstSpinBox->setEnabled(false);

  // generate obstacle push button
  ui->generateObstPushButton->setEnabled(false);

  // place configuration mode pushbutton
  ui->placeConfigButton->setDefault(false);

  // start config radio button
  ui->startConfigRadioButton->setChecked(true);
  ui->startConfigRadioButton->setEnabled(false);

  // goal config radio button
  ui->goalConfigRadioButton->setEnabled(false);
  ////////////////////////////////////

  ////////////////////////////////
  /// grids and grid map related
  ////////////////////////////////
  // set minimum and maximum values of slider
  // map size - 500x500
  // max grid numbers = 100x100
  // min grid numbers = 2x2
  ui->gridSlider->setMinimum(5);
  ui->gridSlider->setMaximum(250);
  ui->gridSlider->setTickInterval(5);
  ui->gridSlider->setSingleStep(5);
  ui->gridSlider->setEnabled(false);

  // setting default grid size (50px)
  int grid_size = 50;
  ui->gridSlider->setValue(grid_size);

  gridMap = new GridMap(grid_size, scene);
  gridMap->renderGrids(grid_size);
  connect(ui->gridSlider, SIGNAL(valueChanged(int)), gridMap,
          SLOT(renderGrids(int)));
  ////////////////////////////////

  ////////////////////////////////
  /// obstacle number spinbox
  ////////////////////////////////
  // set initial minimum and maximum values for spinbox
  ui->obstSpinBox->setMinimum(0);
  ui->obstSpinBox->setMaximum(gridMap->getFreeSpaceIdxs().size());

  ////////////////////////////////
  /// algorithm combobox
  ////////////////////////////////
  ui->comboBox->addItem("BFS");
  ui->comboBox->addItem("DFS");
  // ui->comboBox->addItem("DIJKSTRA");
  // ui->comboBox->addItem("A*");
  ////////////////////////////////

  ////////////////////////////////
  /// visualization related
  ////////////////////////////////
  connect(&visualizationTimer, SIGNAL(timeout()), this, SLOT(visualize()));
  visualizationCurrentIdx_ = 0;

  ui->animationSpeedSlider->setRange(1, 100);
  ui->animationSpeedSlider->setValue(50);

  visualizationTimer.setInterval(ui->animationSpeedSlider->value());

  // animation checkbox
  ui->animationCheckBox->setChecked(true);
  ////////////////////////////////
}

// destructor
MainWindow::~MainWindow() {
  delete ui;
  delete gridMap;
  emit sendFinishStatus();
  plannerThread_->wait();
}

// map editing button slot
void MainWindow::on_editMapButton_pressed() {
  ui->editMapButton->setDefault(!mapEditingMode_);
  mapEditingMode_ = !mapEditingMode_;

  // enable/disable other gui stuffs
  // grid size slider
  ui->gridSlider->setEnabled(mapEditingMode_);
  // place config push button
  ui->placeConfigButton->setEnabled(!mapEditingMode_);
  // RUN push button
  ui->runButton->setEnabled(!mapEditingMode_);
  // animation checkbox
  ui->animationCheckBox->setEnabled(!mapEditingMode_);
  // animation speed slider
  ui->animationSpeedSlider->setEnabled(!mapEditingMode_);
  // obstacle number spinbox
  ui->obstSpinBox->setEnabled(mapEditingMode_);
  // generate obstacle push button
  ui->generateObstPushButton->setEnabled(mapEditingMode_);

  ui->obstSpinBox->setMaximum(gridMap->getFreeSpaceIdxs().size());

  // pass edit mode to grid map
  gridMap->setEditingMapMode(mapEditingMode_);
}

void MainWindow::on_gridSlider_sliderReleased() {
  ui->obstSpinBox->setMaximum(gridMap->getFreeSpaceIdxs().size());
}

void MainWindow::on_generateObstPushButton_clicked() {
  gridMap->generateObstacles(ui->obstSpinBox->value());
  ui->obstSpinBox->setMaximum(gridMap->getFreeSpaceIdxs().size());
}

// place configuration mode slot
void MainWindow::on_placeConfigButton_pressed() {
  ui->placeConfigButton->setDefault(!placeConfigMode_);
  placeConfigMode_ = !placeConfigMode_;

  // enable/disable gui stuffs
  // start and goal radio buttons
  ui->startConfigRadioButton->setEnabled(placeConfigMode_);
  ui->goalConfigRadioButton->setEnabled(placeConfigMode_);

  if (isGuiReset_) {
    // map editing button
    ui->editMapButton->setEnabled(!placeConfigMode_);
    // RUN push button
    ui->runButton->setEnabled(!placeConfigMode_);
    // animation checkbox
    ui->animationCheckBox->setEnabled(!placeConfigMode_);
    // animation speed slider
    ui->animationSpeedSlider->setEnabled(!placeConfigMode_);
    // make start radio button always selected first
    ui->startConfigRadioButton->setChecked(true);
    gridMap->setStartConfigRadioButton(true);
  } else {
    // only allow to change goal vertex if planner is not reset yet
    ui->startConfigRadioButton->setEnabled(false);
    ui->startConfigRadioButton->setChecked(false);
    ui->goalConfigRadioButton->setChecked(true);
    gridMap->setStartConfigRadioButton(false);
  }

  gridMap->setPlaceConfigMode(placeConfigMode_);
}

void MainWindow::on_startConfigRadioButton_pressed() {
  gridMap->setStartConfigRadioButton(true);
}

void MainWindow::on_goalConfigRadioButton_pressed() {
  gridMap->setStartConfigRadioButton(false);
}

// this slot is called when the algorithm finshes and sends a signal
// of visited vertices in order
void MainWindow::receiveVisitedVerticesIdxOrder(
    const QList<int> *visitedVerticesIdxOrder) {
  for (auto idx : *visitedVerticesIdxOrder) {
    visitedVerticesIdxOrder_.append(idx);
  }
}

// this slot is called when the algorithm finishes and sends a signal
// of (child, parent) pairs hash map of vertices
void MainWindow::receiveChildParentIdxs(
    const std::unordered_map<int, int> *childParentIdxs) {
  childParentIdxs_ = *childParentIdxs;

  // if animation is not allowed
  // we visualize the visited vertices and path without using timer
  if (!animatePlanning_) {
    // render visited grids
    for (auto idx : visitedVerticesIdxOrder_) {
      if (idx != goalKeyIdx_)
        gridMap->getGridsList()->at(idx)->setBrush(
            QBrush(Qt::blue, Qt::SolidPattern));
    }
    // render path
    while (childParentIdxs_[currentPathKeyIdx_] != startKeyIdx_) {
      // find the parent vertex of current key (child)
      currentPathKeyIdx_ = childParentIdxs_[currentPathKeyIdx_];
      gridMap->getGridsList()
          ->at(currentPathKeyIdx_)
          ->setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    }
  }
}

// slot timer function for visualization
void MainWindow::visualize() {
  // if we have idxs for visualization
  // start the visualization
  if (visitedVerticesIdxOrder_.size() > 0) {
    if (animatePlanning_) {
      // show all visited vertices
      if (visualizationCurrentIdx_ <= visitedVerticesIdxOrder_.size() - 1) {
        // skip the goal vertex
        if (visitedVerticesIdxOrder_.at(visualizationCurrentIdx_) !=
            goalKeyIdx_)
          gridMap->getGridsList()
              ->at(visitedVerticesIdxOrder_.at(visualizationCurrentIdx_))
              ->setBrush(QBrush(Qt::blue, Qt::SolidPattern));
        visualizationCurrentIdx_++;
      }
      // if done, visualize solution path until it reaches start vertex
      else if (childParentIdxs_[currentPathKeyIdx_] != startKeyIdx_) {
        // find the parent vertex of current key (child)
        currentPathKeyIdx_ = childParentIdxs_[currentPathKeyIdx_];
        gridMap->getGridsList()
            ->at(currentPathKeyIdx_)
            ->setBrush(QBrush(Qt::gray, Qt::SolidPattern));
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
  plannerThread_ = new QThread(this);

  // choose planners
  if (arg1 == "BFS") {
    planner_.reset(new planner::search::BFS());
  } else if (arg1 == "DFS") {
    planner_.reset(new planner::search::DFS());
  }

  // for RUN & RESET buttons
  connect(this, SIGNAL(sendGridMapInformation(const GridMap *)), planner_.get(),
          SLOT(plan(const GridMap *)), Qt::DirectConnection);
  connect(this, SIGNAL(sendResetStatus()), planner_.get(),
          SLOT(receiveResetStatus()), Qt::DirectConnection);

  // main function loop of the planner thread
  connect(plannerThread_, SIGNAL(started()), planner_.get(), SLOT(process()));

  // for quitting stuffs
  connect(this, SIGNAL(sendFinishStatus()), planner_.get(),
          SLOT(receiveFinishStatus()), Qt::DirectConnection);
  connect(planner_.get(), SIGNAL(finished()), plannerThread_, SLOT(quit()),
          Qt::DirectConnection);
  connect(plannerThread_, SIGNAL(finished()), plannerThread_,
          SLOT(deleteLater()));

  // for visualization stuffs
  connect(planner_.get(),
          SIGNAL(sendVisitedVerticesIdxOrder(const QList<int> *)), this,
          SLOT(receiveVisitedVerticesIdxOrder(const QList<int> *)),
          Qt::DirectConnection);
  connect(planner_.get(),
          SIGNAL(sendChildParentIdxs(const std::unordered_map<int, int> *)),
          this,
          SLOT(receiveChildParentIdxs(const std::unordered_map<int, int> *)),
          Qt::DirectConnection);

  // move the current planner onto the thread
  // and initialize the algorithm
  planner_.get()->moveToThread(plannerThread_);
  plannerThread_->start();
}

// RUN pushbutton clicked slot
void MainWindow::on_runButton_clicked() {
  // emit a signal to algorithm slot containing gridmap information
  emit sendGridMapInformation(gridMap);

  // get the start and goal vertex indexes
  goalKeyIdx_ = gridMap->getGridsList()->indexOf(gridMap->getGoalGrid());
  startKeyIdx_ = gridMap->getGridsList()->indexOf(gridMap->getStartGrid());
  currentPathKeyIdx_ = goalKeyIdx_;

  // disable the RUN button
  ui->runButton->setEnabled(false);

  // enable the RESET button
  ui->resetButton->setEnabled(true);

  // disable the animation checkbox
  ui->animationCheckBox->setEnabled(false);

  // disable the map editing button
  ui->editMapButton->setEnabled(false);

  // animate based on checkbox
  if (ui->animationCheckBox->isChecked()) {
    visualizationTimer.start();
    animatePlanning_ = true;
  } else
    animatePlanning_ = false;

  isGuiReset_ = false;
}

// RESET pushbutton clicked slot
void MainWindow::on_resetButton_clicked() {
  // send reset status to algorithm
  emit sendResetStatus();

  // stop the timer
  visualizationTimer.stop();

  // enable the RUN button
  ui->runButton->setEnabled(true);

  // disable the RESET button
  ui->resetButton->setEnabled(false);

  // enable the animation checkbox
  ui->animationCheckBox->setEnabled(true);

  // enable the map editing button
  ui->editMapButton->setEnabled(true);

  // make edit map button not selected
  ui->editMapButton->setDefault(false);
  mapEditingMode_ = false;

  // make place config button not selected
  ui->placeConfigButton->setDefault(false);
  placeConfigMode_ = false;
  // checked start radio button as default
  ui->startConfigRadioButton->setChecked(true);
  // disable start and goal radio buttons
  ui->startConfigRadioButton->setEnabled(false);
  ui->goalConfigRadioButton->setEnabled(false);

  // remove all the rendered path and visited vertices
  for (auto idx : visitedVerticesIdxOrder_) {
    if (idx != goalKeyIdx_)
      gridMap->getGridsList()->at(idx)->setBrush(
          QBrush(Qt::white, Qt::SolidPattern));
  }

  // clear visualization related stuffs
  visitedVerticesIdxOrder_.clear();
  childParentIdxs_.clear();
  visualizationCurrentIdx_ = 0;

  isGuiReset_ = true;
}

// slot for setting the animation speed
void MainWindow::on_animationSpeedSlider_valueChanged(int value) {
  visualizationTimer.setInterval(value);
}

// slot for enable/disable the animation
void MainWindow::on_animationCheckBox_stateChanged(int arg1) {
  // if checked, enable speed slider
  if (ui->animationCheckBox->isChecked())
    ui->animationSpeedSlider->setEnabled(true);
  else
    ui->animationSpeedSlider->setEnabled(false);
}

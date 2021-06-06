#pragma once

#include <QDebug>
#include <QGraphicsScene>
#include <QList>
#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <chrono>
#include <memory>
#include <stack>
#include <thread>
#include <unordered_map>

#include "grid.h"
#include "grid_map.h"
#include "planner/planner.h"
#include "planner/search/bfs/bfs.h"
#include "planner/search/dfs/dfs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /** \brief Constructor. */
  MainWindow(QWidget *parent = nullptr);

  /** \brief Destructor. */
  ~MainWindow();

 private slots:

  /** \brief GUI slot functions. */
  void on_gridSlider_sliderReleased();
  void on_generateObstPushButton_clicked();
  void on_clearObstPushButton_clicked();
  void on_animationCheckBox_stateChanged(int arg1);
  void on_animationSpeedSlider_valueChanged(int value);
  void on_comboBox_currentTextChanged(const QString &arg1);
  void on_runButton_clicked();
  void on_resetButton_clicked();
  void on_editMapButton_pressed();
  void on_placeConfigButton_pressed();
  void on_startConfigRadioButton_pressed();
  void on_goalConfigRadioButton_pressed();

  /** \brief This slot function is called when the planner finishes
   *  the search and sends a signal containing list of vertices' indexes in
   *  order. */
  void receiveVisitedVerticesIdxOrder(
      const QList<int> *visitedVerticesIdxOrder);

  /** \brief This slot function is called when the planner finishes
   *  the search and sends a signal of hash map containing pairs of child and
   *  parent vertex indexes. */
  void receiveChildParentIdxs(
      const std::unordered_map<int, int> *childParentIdxs);

  /** \brief This slot function is called when a timer sends a timeout signal.
   *  The purpose of the function is for visualizing the animation of planning
   *  process. */
  void visualize();

 signals:
  /** \brief Signal for sending gridmap information to the planner. */
  void sendGridMapInformation(const GridMap *);

  /** \brief Send a reset signal to the planner. */
  void sendResetStatus();

  /** \brief Send a finish signal to the planner. */
  void sendFinishStatus();

 private:
  //////////////////////////////////////////////////////////////////
  /// \brief Private member variables. All the variable names might
  /// be understandable for their used cases
  //////////////////////////////////////////////////////////////////
  Ui::MainWindow *ui;
  QGraphicsScene *scene;
  GridMap *gridMap;
  std::unique_ptr<planner::Planner> planner_;
  QThread *plannerThread_;
  QTimer visualizationTimer;
  QList<int> visitedVerticesIdxOrder_;
  std::unordered_map<int, int> childParentIdxs_;
  int visualizationCurrentIdx_;
  int currentPathKeyIdx_;
  int startKeyIdx_;
  int goalKeyIdx_;
  bool isVisualized_;
  bool animatePlanning_;
  bool mapEditingMode_;
  bool placeConfigMode_;
  bool isGuiReset_;
};

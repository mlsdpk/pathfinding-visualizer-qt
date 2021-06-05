#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <thread>
#include <chrono>
#include <stack>
#include <unordered_map>
#include <memory>
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QMainWindow>
#include <QGraphicsScene>
#include "grid.h"
#include "grid_map.h"
#include "planner/planner.h"
#include "planner/search/bfs/bfs.h"
#include "planner/search/dfs/dfs.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_animationCheckBox_stateChanged(int arg1);

private slots:
    void on_animationSpeedSlider_valueChanged(int value);

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_runButton_clicked();
    void on_resetButton_clicked();

private slots:
    void on_editMapButton_pressed();
    void on_placeConfigButton_pressed();
    void on_startConfigRadioButton_pressed();
    void on_goalConfigRadioButton_pressed();

    void testDestroyed(QObject* obj);

private slots:
    void receiveVisitedVerticesIdxOrder(const QList<int>* visitedVerticesIdxOrder);
    void receiveChildParentIdxs(const std::unordered_map<int, int>* childParentIdxs);
    void visualize();

signals:
    void sendGridMapInformation(const GridMap*);
    void sendResetStatus();
    void sendFinishStatus();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    GridMap *gridMap;
    std::unique_ptr<planner::Planner> planner_;
    QThread* plannerThread_;
    QList<Grid*> visualizeGrids_;

    QTimer visualizationTimer;
    QList<int> visitedVerticesIdxOrder_;
    std::unordered_map<int, int> childParentIdxs_;
    //QList<std::pair<int, int>> visualizationIdxes_;
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
#endif // MAINWINDOW_H

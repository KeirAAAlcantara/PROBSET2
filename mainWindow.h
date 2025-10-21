#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QPointF>
#include <vector>
#include <atomic>
#include <memory>

#include "Canvas.h"
#include "voronoi.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadConfigFile();
    void startSimulation();
    void stopSimulation();
    void updateCanvasDisplay();

private:
    void clearSimulationData();

    Canvas *canvas = nullptr;

    std::vector<std::shared_ptr<std::atomic<float>>> temperatures;
    std::vector<SensorWorker*> workers;
    std::vector<QPointF> positions;
    std::vector<QColor> colors;

    float distanceThreshold = 0.0f;
    std::vector<std::unique_ptr<voronoi>> agents;

    QTimer *guiUpdateTimer = nullptr;
};

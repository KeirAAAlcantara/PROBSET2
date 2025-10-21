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
#include "sensorworker.h"

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

    Canvas *canvas;

    vector<shared_ptr<atomic<float>>> temperatures;
    vector<SensorWorker*> workers;
    vector<QPointF> positions;
    vector<QColor> colors;

    float distanceThreshold = 0.0f;

    QTimer *guiUpdateTimer;
};

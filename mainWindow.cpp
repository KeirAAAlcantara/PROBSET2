#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Sensor Temperature Simulation");
    setFixedSize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // Canvas initialization (pass references)
    canvas = new Canvas(&temperatures, &positions, &colors, this);
    mainLayout->addWidget(canvas, 1);

    // Buttons
    QPushButton *loadButton = new QPushButton("Load Config File", this);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadConfigFile);
    buttonLayout->addWidget(loadButton);

    QPushButton *startButton = new QPushButton("Start Simulation", this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    buttonLayout->addWidget(startButton);

    QPushButton *stopButton = new QPushButton("Stop Simulation", this);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);
    buttonLayout->addWidget(stopButton);

    mainLayout->addLayout(buttonLayout);

    // Timer for periodic GUI updates
    guiUpdateTimer = new QTimer(this);
    connect(guiUpdateTimer, &QTimer::timeout, this, &MainWindow::updateCanvasDisplay);
    guiUpdateTimer->start(50);
}

MainWindow::~MainWindow() {
    stopSimulation();
}

void MainWindow::clearSimulationData() {
    stopSimulation();
    for (auto *worker : workers) {
        delete worker;
    }
    worker.clear();
    agents.clear();
    temperatures.clear();
    positions.clear();
    colors.clear();
    distanceThreshold = 0.0f;
    canvas->update();
}

void MainWindow::loadConfigFile() {
    QString filePath = QCoreApplication::applicationDirPath() + QDir::separator() + "config.txt";

    qInfo() << "Attempting to load config from:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    clearSimulationData();

    // Read threshold
    if (!in.atEnd()) {
        QString line = in.readLine();
        bool ok;
        distanceThreshold = line.toFloat(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Error", "Invalid distance threshold in config file.");
            file.close();
            return;
        }
        qInfo() << "Distance Threshold set to:" << distanceThreshold;
    } else {
        QMessageBox::warning(this, "Error", "Config file is empty or missing threshold.");
        file.close();
        return;
    }

    // Read sensors
    int idCounter = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() >= 3) {
            bool okX, okY, okTemp;
            float x = parts[0].toFloat(&okX);
            float y = parts[1].toFloat(&okY);
            float initialTemp = parts[2].toFloat(&okTemp);

            if (okX && okY && okTemp) {
                positions.push_back(QPointF(x, y));
                temperatures.push_back(std::make_shared<std::atomic<float>>(initialTemp));
                colors.push_back(QColor(
                    QRandomGenerator::global()->bounded(256),
                    QRandomGenerator::global()->bounded(256),
                    QRandomGenerator::global()->bounded(256)
                ));

                agents.push_back(std::make_unique<voronoi>(
                    idCounter++, distanceThreshold, &temperatures, &positions));
            } else {
                qWarning() << "Failed to parse line:" << line;
            }
        } else if (!line.trimmed().isEmpty()) {
            qWarning() << "Malformed line (expected 3 values):" << line;
        }
    }
    file.close();

        // Create one SensorWorker thread per sensor
    for (int i = 0; i < positions.size(); ++i) {
        auto *worker = new SensorWorker(i, &temperatures, &positions, distanceThreshold, this);
        workers.push_back(worker);
    }

    if (positions.empty()) {
        QMessageBox::warning(this, "Warning", "No sensor points loaded from config file.");
    } else {
        QMessageBox::information(this, "Success",
                                 QString::number(positions.size()) + " sensor points loaded.");
        canvas->updateDisplay();
    }
}

void MainWindow::startSimulation()
{
    if (workers.empty()) {
        QMessageBox::warning(this, "Warning", "No sensor data loaded. Please load data first.");
        return;
    }

    for (auto *worker : workers) {
        if (!worker->isRunning()) {
            worker->start();
        }
    }

    qInfo() << "Simulation started with" << workers.size() << "sensor threads.";
}


void MainWindow::stopSimulation()
{
    for (auto *worker : workers) {
        if (worker->isRunning()) {
            worker->stop();
            worker->wait(); // wait for clean thread exit
        }
    }

    qInfo() << "All sensor threads stopped.";
}


void MainWindow::updateCanvasDisplay() {
    canvas->updateDisplay();
}

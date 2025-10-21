#include "MainWindow.h"
// #include "ui_MainWindow.h" // Uncomment if using a .ui file
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator> // For initial random colors if not in config
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow) // Uncomment if using a .ui file
{
    // ui->setupUi(this); // Uncomment if using a .ui file
    setWindowTitle("Sensor Temperature Simulation");
    setFixedSize(800, 600); // Fixed window size

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // Initialize Canvas, passing const pointers as Canvas only reads
    canvas = new Canvas(&temperatures, &positions, &colors, this);
    mainLayout->addWidget(canvas, 1); // Give canvas more space

    // Setup buttons
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

    // Setup GUI update timer
    guiUpdateTimer = new QTimer(this);
    connect(guiUpdateTimer, &QTimer::timeout, this, &MainWindow::updateCanvasDisplay);
    guiUpdateTimer->start(50); // Update GUI every 50ms (20 FPS)
}

MainWindow::~MainWindow()
{
    stopSimulation(); // Ensure all agent threads are stopped
    // delete ui; // Uncomment if using a .ui file
}

void MainWindow::clearSimulationData() {
    stopSimulation(); // Stop any running agents first
    agents.clear(); // This will destroy SensorAgent objects, calling their destructors
    temperatures.clear();
    positions.clear();
    colors.clear();
    distanceThreshold = 0.0f;
    canvas->update(); // Clear the display
}

void MainWindow::loadConfigFile()
{
    QString filePath = QCoreApplication::applicationDirPath() + QDir::separator() + "config.txt";

    qInfo() << "Attempting to load config from:" << filePath;
    /* 
    QString filePath = QFileDialog::getOpenFileName(this, "Open Configuration File", "", "Text Files (*.txt);;All Files (*)");
    if (filePath.isEmpty()) {
        return;
    }
    */

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    clearSimulationData(); // Clear previous data before loading new

    // Read distance threshold from the first line
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

    int id_counter = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() >= 3) { // Expect x, y, initial_temp
            bool okX, okY, okTemp;
            float x = parts[0].toFloat(&okX);
            float y = parts[1].toFloat(&okY);
            float initialTemp = parts[2].toFloat(&okTemp);

            if (okX && okY && okTemp) {
                positions.push_back(QPointF(x, y));
                temperatures.push_back(std::atomic<float>(initialTemp));
                // Assign a random color if not specified in config (or load from config)
                colors.push_back(QColor(QRandomGenerator::global()->bounded(256),
                                        QRandomGenerator::global()->bounded(256),
                                        QRandomGenerator::global()->bounded(256)));

                // Create agent with the loaded threshold
                agents.push_back(std::make_unique<voronoi>(
                    id_counter++, distanceThreshold, &temperatures, &positions));
            } else {
                qWarning() << "MainWindow::loadConfigFile: Failed to parse data on line: " << line;
            }
        } else if (!line.trimmed().isEmpty()) {
             qWarning() << "MainWindow::loadConfigFile: Malformed line (expected 3 values): " << line;
        }
    }
    file.close();

    if (positions.empty()) {
        QMessageBox::warning(this, "Warning", "No sensor points loaded from config file.");
    } else {
        QMessageBox::information(this, "Success", QString::number(positions.size()) + " sensor points loaded.");
        canvas->updateDisplay(); // Initial draw with loaded data
    }
}

void MainWindow::startSimulation()
{
    if (agents.empty()) {
        QMessageBox::warning(this, "Warning", "No sensor data loaded. Please load data first.");
        return;
    }

    for (const auto& agent : agents) {
        agent->start(); // Start each sensor's thread
    }
    qInfo() << "Simulation started for" << agents.size() << "agents.";
}

void MainWindow::stopSimulation()
{
    if (agents.empty()) return;

    for (const auto& agent : agents) {
        agent->stop(); // Request each agent's thread to stop
    }
    qInfo() << "Simulation stopped.";
}

void MainWindow::updateCanvasDisplay()
{
    canvas->updateDisplay(); // This will call canvas->update() in the main thread
}
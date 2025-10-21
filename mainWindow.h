#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton> // For buttons
#include <QLabel>      // For displaying threshold etc.

#include "Canvas.h"
#include "voronoi.h"
#include <vector>
#include <atomic>
#include <memory> // For std::unique_ptr

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
    void loadConfigFile();
    void startSimulation();
    void stopSimulation();
    void updateCanvasDisplay(); // Slot for QTimer to update the canvas

private:
    void clearSimulationData(); // Helper to clear vectors and stop agents

    Ui::MainWindow *ui; // If you're using a .ui file, otherwise remove

    Canvas *canvas;

    // Data structures owned by MainWindow
    std::vector<std::atomic<float>> temperatures;
    std::vector<QPointF> positions;
    std::vector<QColor> colors; // Base colors for each sensor

    float distanceThreshold = 0.0f; // Loaded from config file

    std::vector<std::unique_ptr<voronoi>> agents; // Manages sensor threads

    QTimer *guiUpdateTimer; // Timer to periodically call updateCanvasDisplay
};
#endif // MAINWINDOW_H
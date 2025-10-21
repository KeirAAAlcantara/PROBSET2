#include <QApplication>
#include "Canvas.h"
#include <atomic>
#include <chrono>
#include <thread> 

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    vector<atomic<float>> temp;
    vector<QPointF> positions;
    vector<QColor> colors;

    Canvas canvas(&temp, &positions, &colors);
    canvas.resize(800,600);
    canvas.show();

    thread update([&]){
        while(true){
            for (auto &t:temp)
                t.store(rand())
            QMetaObject::invokeMethod(&canvas, "update", Qt::QueuedConnection);
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    int result = app.exec();
    update.detach();
    return result;
}
#include <fstream> 
#include <iostream>
#include <QFile>
#include <QColor>
#include <sstream>
#include <vector> 
#include <QDebug> 
#include <QString> 

using namespace std;

struct SensorConfig{
    int x;
    int y;
    float temperature;
};

float distanceThreshold;
vector<SensorConfig> sensors;
vector<QColor> colors;

void reader(const QString& fileName){
    ifstream file(fileName.toStdString());
    if(!file.is_open()){
        qWarning() << "Error" << fileName;
        return;
    }

    string line;
    bool firstLine = true; 

    while(getline(file, line)){
        if(line.empty()) continue;
        istringstream strS(line); 

        if(firstLine) {
            strS >> distanceThreshold; 
            firstLine = false;
        } else {
            int x, y;
            float temp;
            if(strS >> x >> y >> temp) {
                sensors.push_back({x, y, temp});

                QColor color (rand() %256, rand() % 256, rand() % 256);
                colors.push_back(color);
            }
        }
    }
    file.close();
}
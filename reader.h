
#pragma once 
#include <QString>
#include <QColor>
#include <vector>

using namespace std;

struct SensorConfig{
    int x;
    int y;
    float temperature;
};

extern float distanceThreshold;
extern vector<SensorConfig> sensors;
vector<QColor> colors;
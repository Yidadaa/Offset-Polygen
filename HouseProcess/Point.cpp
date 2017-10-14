#pragma once
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <minmax.h>
//#include "House.h"

#define MIN_ERR 0.00001 // ������С��������ȼ���
using std::string;
using std::vector;
using namespace std;

Point::Point()
{
    isNULL = true;
}

Point::Point(float x_val, float y_val, float bulge_val, string id_val) {
    x = x_val;
    y = y_val;
    z = 0;
    bulge = bulge_val;
    id = id_val;
    isNULL = false;
}

Point::Point(float x_val, float y_val) {
    x = x_val;
    y = y_val;
    z = 0;
    bulge = 0;
    id = "No ID";
    isNULL = false;
}

/* �жϸõ�������һ�����Ƿ������� */
bool Point::isEqualTo(Point p) {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2)) < MIN_ERR;
}

/* �ж�ĳ���Ƿ���ĳ������ */
bool Point::isInRegion(Region r) {
    Point zeroPoint(0, 0);
    //Segment line = Segment(zeroPoint, *this);
    // TODO: ���Segment���getCurWithRegion����
    return true;
}


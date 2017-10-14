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

Segment::Segment(Point sp, Point ep, string id_val) {
    startPoint = sp;
    endPoint = ep;
    id = id_val;
    a = sp.y - ep.y;
    b = sp.x - ep.x;
    c = sp.x * ep.y - sp.y * ep.x;
    distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
    xRange.min = min(sp.x, ep.x);
    xRange.max = max(sp.x, ep.x);
    yRange.min = min(sp.y, ep.y);
    yRange.max = max(sp.y, ep.y);
}

Segment::Segment(Point sp, Point ep) {
    startPoint = sp;
    endPoint = ep;
    id = "No ID";
    a = sp.y - ep.y;
    b = sp.x - ep.x;
    c = sp.x * ep.y - sp.y * ep.x;
    distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
    xRange.min = min(sp.x, ep.x);
    xRange.max = max(sp.x, ep.x);
    yRange.min = min(sp.y, ep.y);
    yRange.max = max(sp.y, ep.y);
}


/* �ж��Ƿ�����һ���߶�ƽ�� */
bool Segment::isParalWith(Segment s) {
    return abs(a * s.b - b * s.a) < MIN_ERR;
}

/* ��������һ���߶εĽ��� */
Point Segment::getCorWith(Segment s) {
    auto isInRange = [](float n, Range range) { // �����ж�ĳ��ֵ�Ƿ��ڷ�Χ��
        return n >= range.min && n <= range.max;
    };
    if (this->isParalWith(s)) {
        return Point(); // �����ƽ�еģ��Ͳ����ڽ���
    }
    float der = a * s.b - b * s.a;
    float x = (b * s.c - c * s.b) / der;
    float y = (a * s.c - c * s.a) / der; // �������������ֵ
    if (isInRange(x, xRange)
        && isInRange(y, yRange)
        && isInRange(x, s.xRange)
        && isInRange(y, s.yRange)) {
        Point p(x, y);
        return p;
    }
    else {
        return Point(); // ������㲻���߶η�Χ�ڣ�Ҳ������
    }
}

/* �����߶�������Ľ��� */
vector<Point> Segment::getCorWithRegion(Region r) {
    //return vector<Point>();
}